#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include <rlcsa/sasamples.h>
#include <rlcsa/utils.h>

#ifdef MULTITHREAD_SUPPORT
#ifndef _GLIBCXX_PARALLEL
#include <mcstl.h>
#endif
#endif


namespace TextIndexes
{


SASamples::SASamples(std::ifstream& sample_file, usint sample_rate, bool _weighted) :
  weighted(_weighted),
  rate(sample_rate)
{
  if(this->weighted)
  {
    sample_file.read((char*)&(this->size), sizeof(this->size));
    sample_file.read((char*)&(this->items), sizeof(this->items));

    pair_type* sample_pairs = new pair_type[this->items];
    sample_file.read((char*)sample_pairs, this->items * sizeof(pair_type));
    this->buildSamples(sample_pairs, false);
    this->buildSamples(sample_pairs, true);
    delete[] sample_pairs;
  }
  else
  {
    this->indexes = new DeltaVector(sample_file);
    this->size = indexes->getSize();
    this->items = indexes->getNumberOfItems();
    this->samples = new ReadBuffer(sample_file, this->items, length(this->items - 1));
    this->buildInverseSamples();
  }
}

SASamples::SASamples(uint* array, usint data_size, usint sample_rate) :
  weighted(false),
  rate(sample_rate),
  size(data_size)
{
  this->items = (this->size - 1) / this->rate + 1;

  WriteBuffer sample_buffer(this->items, length(this->items - 1));
  DeltaEncoder encoder(INDEX_BLOCK_SIZE);
  for(usint i = 0; i < this->size; i++)
  {
    if(array[i] % sample_rate == 0)
    {
      encoder.setBit(i);
      sample_buffer.writeItem(array[i] / sample_rate);
    }
  }

  this->indexes = new DeltaVector(encoder, this->size);
  this->samples = sample_buffer.getReadBuffer();
  this->buildInverseSamples();
}

SASamples::SASamples(uint* inverse, DeltaVector* end_points, usint data_size, usint sample_rate) :
  weighted(false),
  rate(sample_rate),
  items(0)
{
  usint sequences = end_points->getNumberOfItems();

  DeltaVector::Iterator iter(*(end_points));

  // Determine the samples, insert them into a vector, and sort them.
  usint start = 0, end = iter.select(0);  // Closed range in padded collection.
  usint seq_start = 0, seq_end = end;            // Closed range in inverse SA.
  std::vector<pair_type>* vec = new std::vector<pair_type>();
  for(usint i = 0; i < sequences; i++)
  {
    for(usint j = seq_start; j <= seq_end; j += this->rate)
    {
      vec->push_back(pair_type(inverse[j] - sequences - 1, this->items));
      this->items++;
    }
    start = nextMultipleOf(this->rate, end);
    end = iter.selectNext();
    seq_start = seq_end + 2;
    seq_end = seq_start + end - start;
  }
  #ifdef MULTITHREAD_SUPPORT
    #ifdef _GLIBCXX_PARALLEL
      std::sort(vec->begin(), vec->end(), __gnu_parallel::sequential_tag());
    #else
      std::sort(vec->begin(), vec->end(), mcstl::sequential_tag());
    #endif
  #else
    std::sort(vec->begin(), vec->end());
  #endif

  // Compress the samples.
  this->size = end + 1;
  WriteBuffer sample_buffer(this->items, length(this->items - 1));
  DeltaEncoder encoder(INDEX_BLOCK_SIZE);
  for(usint i = 0; i < this->items; i++)
  {
    pair_type sample = (*vec)[i];
    encoder.setBit(sample.first);
    sample_buffer.writeItem(sample.second);
  }
  delete vec;

  this->indexes = new DeltaVector(encoder, this->size);
  this->samples = sample_buffer.getReadBuffer();
  this->buildInverseSamples();
}

SASamples::SASamples(uint* inverse, WeightedSampler& sampler) :
  weighted(true),
  rate(1), size(sampler.getSize()), items(sampler.getItems())
{
  pair_type* sample_pairs = sampler.getSamples(inverse, 1); // FIXME multiple sequences?
  this->buildSamples(sample_pairs, false);
  this->buildSamples(sample_pairs, true);
  delete[] sample_pairs;
}

SASamples::SASamples(SASamples& index, SASamples& increment, usint* positions, usint number_of_positions, usint number_of_sequences) :
  weighted(false),
  rate(index.rate),
  size(index.size + increment.size),
  items(index.items + increment.items)
{
  this->mergeSamples(index, increment, positions, number_of_positions, number_of_sequences);
  this->buildInverseSamples();
}

SASamples::~SASamples()
{
  delete this->indexes; this->indexes = 0;
  delete this->samples; this->samples = 0;
  delete this->inverse_indexes; this->inverse_indexes = 0;
  delete this->inverse_samples; this->inverse_samples = 0;
}

//--------------------------------------------------------------------------

void
SASamples::writeTo(std::ofstream& sample_file) const
{
  if(this->weighted)
  {
    sample_file.write((char*)&(this->size), sizeof(this->size));
    sample_file.write((char*)&(this->items), sizeof(this->items));

    DeltaVector::Iterator iter(*(this->indexes));
    pair_type buffer(iter.select(0), this->samples->readItem(0));
    for(usint i = 0; i < this->items; i++)
    {
      sample_file.write((char*)&buffer, sizeof(buffer));
      buffer.first = iter.selectNext();
      buffer.second = this->samples->readItem();
    }
  }
  else
  {
    this->indexes->writeTo(sample_file);
    this->samples->writeBuffer(sample_file);
  }
}

usint
SASamples::reportSize() const
{
  usint bytes = sizeof(*this);
  bytes += this->indexes->reportSize();
  bytes += this->samples->reportSize();
  if(this->weighted) { bytes += this->inverse_indexes->reportSize(); }
  bytes += this->inverse_samples->reportSize();
  return bytes;
}

//--------------------------------------------------------------------------

pair_type
SASamples::inverseSA(usint value) const
{
  if(value >= this->size) { return pair_type(this->size, this->size); }

  pair_type result;
  if(this->weighted)
  {
    DeltaVector::Iterator iter(*(this->inverse_indexes));
    result.first = iter.rank(value) - 1;
    result.second = this->inverse_samples->readItemConst(result.first);
    result.first = iter.select(result.first);
  }
  else
  {
    DeltaVector::Iterator iter(*(this->indexes));
    result.first = value - (value % this->rate);
    result.second = iter.select(this->inverse_samples->readItemConst(value / this->rate));
  }

  return result;
}

//--------------------------------------------------------------------------

void
SASamples::buildInverseSamples()
{
  WriteBuffer inverse_buffer(this->items, length(this->items - 1));
  this->samples->goToItem(0);
  for(usint i = 0; i < this->items; i++)
  {
    inverse_buffer.goToItem(this->samples->readItem());
    inverse_buffer.writeItem(i);
  }

  this->inverse_indexes = 0;
  this->inverse_samples = inverse_buffer.getReadBuffer();
}

void
SASamples::buildSamples(pair_type* sample_pairs, bool inverse)
{
  if(inverse)
  {
    for(usint i = 0; i < this->items; i++)
    {
      std::swap(sample_pairs[i].first, sample_pairs[i].second);
    }
    std::sort(sample_pairs, sample_pairs + this->items);
  }

  DeltaEncoder encoder(INDEX_BLOCK_SIZE);
  WriteBuffer sample_buffer(this->items, length(this->size - 1));
  for(usint i = 0; i < this->items; i++)
  {
    encoder.setBit(sample_pairs[i].first);
    sample_buffer.writeItem(sample_pairs[i].second);
/*    if(sample_pairs[i].first >= this->size || sample_pairs[i].second >= this->size)
    {
      std::cout << "Sample " << i << ": " << sample_pairs[i] << std::endl;
    }*/
  }

  if(inverse)
  {
    this->inverse_indexes = new DeltaVector(encoder, this->size);
    this->inverse_samples = sample_buffer.getReadBuffer();
  }
  else
  {
    this->indexes = new DeltaVector(encoder, this->size);
    this->samples = sample_buffer.getReadBuffer();
  }
}

//--------------------------------------------------------------------------

void
SASamples::mergeSamples(SASamples& index, SASamples& increment, usint* positions, usint n, usint skip)
{
  if(index.isWeighted() || increment.isWeighted())
  {
    std::cerr << "Error: Cannot merge weighted samples!" << std::endl;
    return;
  }

  DeltaVector::Iterator first(*(index.indexes));
  DeltaVector::Iterator second(*(increment.indexes));
  ReadBuffer* first_samples = index.samples;
  ReadBuffer* second_samples = increment.samples;

  delete index.inverse_samples; index.inverse_samples = 0;
  delete increment.inverse_samples; increment.inverse_samples = 0;

  usint first_bit = first.select(0);
  bool first_finished = false;
  usint second_bit = second.select(0);
  usint sum = index.items;
  first_samples->goToItem(0);
  second_samples->goToItem(0);

  DeltaEncoder encoder(INDEX_BLOCK_SIZE);
  WriteBuffer sample_buffer(this->items, length(this->items - 1));
  for(usint i = 0; i < n; i++, first_bit++)
  {
    while(!first_finished && first_bit < positions[i] - skip)
    {
      encoder.setBit(first_bit);
      sample_buffer.writeItem(first_samples->readItem());
      if(first.hasNext())
      {
        first_bit = first.selectNext() + i;
      }
      else
      {
        first_finished = true;
      }
    }

    if(i == second_bit) // positions[i] is one
    {
      encoder.setBit(positions[i] - skip);
      sample_buffer.writeItem(second_samples->readItem() + sum);
      second_bit = second.selectNext();
    }
  }

  while(!first_finished)
  {
    encoder.setBit(first_bit);
    sample_buffer.writeItem(first_samples->readItem());
    if(!first.hasNext()) { break; }
    first_bit = first.selectNext() + n;
  }

  delete index.indexes; index.indexes = 0;
  delete index.samples; index.samples = 0;
  delete increment.indexes; increment.indexes = 0;
  delete increment.samples; increment.samples = 0;

  this->indexes = new DeltaVector(encoder, size);
  this->samples = sample_buffer.getReadBuffer();
}


} // namespace TextIndexes
