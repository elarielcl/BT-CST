#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>

#include <rlcsa/rlcsa.h>
#include <rlcsa/utils.h>
#include <rlcsa/qsufsort.h>
#include <rlcsa/vectors.h>


#ifdef MULTITHREAD_SUPPORT
#include <omp.h>
#endif



namespace TextIndexes
{


RLCSA::RLCSA(const std::string& base_name, bool print) :
  ok(false),
  sa_samples(0),
  end_points(0)
{
  for(usint c = 0; c < CHARS; c++) { this->array[c] = 0; }

  std::string array_name = base_name + ARRAY_EXTENSION;
  std::cerr <<"Trying to open "<< array_name.c_str() << " in RLCSA" << std::endl;
  std::ifstream array_file(array_name.c_str(), std::ios_base::binary);
  if(!array_file)
  {
    std::cerr << "RLCSA: Error opening Psi array file!" << std::endl;
    return;
  }

  usint distribution[CHARS];
  array_file.read((char*)distribution, CHARS * sizeof(usint));
  this->buildCharIndexes(distribution);

  Parameters parameters;
  parameters.read(base_name + PARAMETERS_EXTENSION);
  for(usint c = 0; c < CHARS; c++)
  {
    if(!isEmpty(this->index_ranges[c])) { 
        this->array[c] = new PsiVector(array_file);
    }
  }
  this->end_points = new DeltaVector(array_file);
  this->number_of_sequences = this->end_points->getNumberOfItems();

  array_file.read((char*)&(this->sample_rate), sizeof(this->sample_rate));
  array_file.close();
  this->support_locate = parameters.get(SUPPORT_LOCATE);
  this->support_display = parameters.get(SUPPORT_DISPLAY);

  if(this->support_locate || this->support_display)
  {
    std::string sa_sample_name = base_name + SA_SAMPLES_EXTENSION;
    std::ifstream sa_sample_file(sa_sample_name.c_str(), std::ios_base::binary);
    std::cerr <<"Trying to open "<< sa_sample_name << " in RLCSA" << std::endl;
    if(!sa_sample_file)
    {
      std::cerr << "RLCSA: Error opening suffix array sample file!" << std::endl;
      return;
    }
    bool weighted = parameters.get(WEIGHTED_SAMPLES);
    this->sa_samples = new SASamples(sa_sample_file, this->sample_rate, weighted);
    sa_sample_file.close();
  }

  if(print) { parameters.print(); }

  this->ok = true;
}

    RLCSA::RLCSA(uchar* data, usint bytes, usint block_size, usint sa_sample_rate, bool multiple_sequences, bool delete_data) :
  ok(false),
  sa_samples(0),
  sample_rate(sa_sample_rate),
  end_points(0)
{
  //counter for each char
  for(usint c = 0; c < CHARS; c++) { this->array[c] = 0; }

  if(!data || bytes == 0)
  {
    std::cerr << "RLCSA: No input data given!" << std::endl;
    if(delete_data) { delete[] data; }
    return;
  }
  if(block_size < 2 * sizeof(usint) || block_size % sizeof(usint) != 0)
  {
    std::cerr << "RLCSA: Block size must be a multiple of " << sizeof(usint) << " bytes!" << std::endl;
    if(delete_data) { delete[] data; }
    return;
  }


  // Do we store SA samples?
  if(this->sample_rate > 0)
  {
    this->support_locate = this->support_display = true;
  }
  else
  {
    this->support_locate = this->support_display = false;
    this->sample_rate = 1;
  }


  // Determine the number of sequences and mark their end points.
  if(multiple_sequences)
  {
    DeltaEncoder endings(RLCSA::ENDPOINT_BLOCK_SIZE);
    this->number_of_sequences = 0;
    usint marker = 0;
    usint padding = 0, chars_encountered = 0;

    for(usint i = 0; i < bytes; i++)
    {
      if(data[i] == 0)
      {
        if(i == marker) { break; }  // Empty sequence.
        this->number_of_sequences++;
        marker = i + 1;

        usint pos = chars_encountered + padding - 1;
        endings.setBit(pos);
        padding = ((pos + this->sample_rate) / this->sample_rate) * this->sample_rate - chars_encountered;
      }
      else
      {
        chars_encountered++;
      }
    }

    if(this->number_of_sequences == 0 || marker != bytes)
    {
      std::cerr << "RLCSA: Collection must consist of 0-terminated nonempty sequences!" << std::endl;
      if(delete_data) { delete[] data; }
      return;
    }
    this->end_points = new DeltaVector(endings, chars_encountered + padding);
  }
  else
  {
	
    this->number_of_sequences = 1;
    //maybe the samples
    DeltaEncoder endings(RLCSA::ENDPOINT_BLOCK_SIZE, RLCSA::ENDPOINT_BLOCK_SIZE);
    usint pos = bytes - 1;
    endings.setBit(pos);
    usint padding = ((pos + this->sample_rate) / this->sample_rate) * this->sample_rate - bytes;
    //creates a bitVector
    this->end_points = new DeltaVector(endings, bytes + padding);
  }


  // Build character tables etc.
  usint distribution[CHARS];
  sint low = CHARS, high = 0;
  for(usint c = 0; c < CHARS; c++) { distribution[c] = 0; }
  for(usint i = 0; i < bytes; i++)
  {
    if(data[i] < low) { low = data[i]; }
    if(data[i] > high) { high = data[i]; }
    distribution[(usint)data[i]]++;
  }
  if(multiple_sequences)
  {
    distribution[0] = 0;
    low = 0;
    high = high + this->number_of_sequences;
  }
  //stores in a vector where each range of the SA starts
  this->buildCharIndexes(distribution);


  // Build suffix array.
  int* inverse = new int[bytes + 1];
  if(multiple_sequences)
  {
    int zeros = 0;
    for(usint i = 0; i < bytes; i++)
    {
      if(data[i] == 0)
      {
        inverse[i] = zeros;
        zeros++;
      }
      else
      {
        inverse[i] = (int)(data[i] + this->number_of_sequences);
      }
    }
  }
  else
  {
	
    for(usint i = 0; i < bytes; i++) { inverse[i] = (int)data[i]; }
  }
  if(delete_data) { delete[] data; }
  int* sa = new int[bytes + 1];
  //builds the suffix array
  suffixsort(inverse, sa, bytes, high + 1, low);


  // Sample SA.
  if(this->support_locate || this->support_display)
  {
    if(multiple_sequences)
    {
      this->sa_samples = new SASamples((uint*)inverse, this->end_points, this->data_size, this->sample_rate);
    }
    else
    {
	  //samples the SA
      this->sa_samples = new SASamples((uint*)(sa + 1), this->data_size, this->sample_rate);
    }
  }


  // Build Psi: phi[i]=SA^-1[SA[i]+1]
  for(usint i = 0; i <= bytes; i++)
  {
    sa[i] = inverse[(sa[i] + 1) % (bytes + 1)];
  }
  delete[] inverse;


  // Build RLCSA.
  usint incr = (multiple_sequences ? this->number_of_sequences + 1 : 1);  // No e_of_s markers in SA.
  usint decr = (multiple_sequences ? 1 : 0);  // Ignore the last e_of_s marker if multiple sequences.
  for(usint c = 0; c < CHARS; c++)
  {
    if(distribution[c] == 0) { this->array[c] = 0; continue; }
	//index ranges is a pair (first,second) that points to the to endpoints of a range
    uint* curr = (uint*)(sa + index_ranges[c].first + incr);
    uint* limit = (uint*)(sa + index_ranges[c].second + incr + 1);
    //
    PsiVector::Encoder encoder(block_size);
    pair_type run(*curr, 1);
    curr++;

    for(; curr < limit; curr++)
    {
      if(*curr == run.first + run.second) { run.second++; }
      else
      {
        encoder.setRun(run.first - decr, run.second);
        run = pair_type(*curr, 1);
      }
    }
    //tells the encoder where the run starts and ends. 
    encoder.setRun(run.first - decr, run.second);
	//compress the given range
    this->array[c] = new PsiVector(encoder, this->data_size + incr - decr);
  }
  delete[] sa;


  this->ok = true;
}

RLCSA::RLCSA(uchar* data, usint bytes, usint block_size, WeightedSampler& sampler, bool delete_data) :
  ok(false),
  sa_samples(0),
  sample_rate(1),
  end_points(0)
{
  for(usint c = 0; c < CHARS; c++) { this->array[c] = 0; }

  if(!data || bytes == 0)
  {
    std::cerr << "RLCSA: No input data given!" << std::endl;
    if(delete_data) { delete[] data; }
    return;
  }
  if(block_size < 2 * sizeof(usint) || block_size % sizeof(usint) != 0)
  {
    std::cerr << "RLCSA: Block size must be a multiple of " << sizeof(usint) << " bytes!" << std::endl;
    if(delete_data) { delete[] data; }
    return;
  }
  if(sampler.getStatus() != WeightedSampler::SAMPLED)
  {
    std::cerr << "RLCSA: No samples given!" << std::endl;
    if(delete_data) { delete[] data; }
    return;
  }


  // Determine the number of sequences and mark their end points.
  this->number_of_sequences = 1;
  DeltaEncoder endings(RLCSA::ENDPOINT_BLOCK_SIZE, RLCSA::ENDPOINT_BLOCK_SIZE);
  endings.setBit(bytes - 1);
  this->end_points = new DeltaVector(endings, bytes);

  // Build character tables etc.
  usint distribution[CHARS];
  sint low = CHARS, high = 0;
  for(usint c = 0; c < CHARS; c++) { distribution[c] = 0; }
  for(usint i = 0; i < bytes; i++)
  {
    if(data[i] < low) { low = data[i]; }
    if(data[i] > high) { high = data[i]; }
    distribution[(usint)data[i]]++;
  }
  this->buildCharIndexes(distribution);


  // Build suffix array.
  int* inverse = new int[bytes + 1];
  for(usint i = 0; i < bytes; i++) { inverse[i] = (int)data[i]; }
  if(delete_data) { delete[] data; }
  int* sa = new int[bytes + 1];
  suffixsort(inverse, sa, bytes, high + 1, low);

  // Sample SA.
  this->sa_samples = new SASamples((uint*)inverse, sampler);
  this->support_locate = this->support_display = true;


  // Build Psi.
  for(usint i = 0; i <= bytes; i++)
  {
    sa[i] = inverse[(sa[i] + 1) % (bytes + 1)];
  }
  delete[] inverse;

  // Build RLCSA.
  for(usint c = 0; c < CHARS; c++)
  {
    if(distribution[c] == 0) { this->array[c] = 0; continue; }

    uint* curr = (uint*)(sa + index_ranges[c].first + 1);
    uint* limit = (uint*)(sa + index_ranges[c].second + 2);
    PsiVector::Encoder encoder(block_size);
    pair_type run(*curr, 1);
    curr++;

    for(; curr < limit; curr++)
    {
      if(*curr == run.first + run.second) { run.second++; }
      else
      {
        encoder.setRun(run.first, run.second);
        run = pair_type(*curr, 1);
      }
    }
    encoder.setRun(run.first, run.second);

    this->array[c] = new PsiVector(encoder, this->data_size + 1);
  }
  delete[] sa;


  this->ok = true;
}

RLCSA::RLCSA(RLCSA& index, RLCSA& increment, usint* positions, usint block_size, usint threads) :
  ok(false),
  sa_samples(0),
  end_points(0)
{
  for(usint c = 0; c < CHARS; c++) { this->array[c] = 0; }

  if(!index.isOk() || !increment.isOk())
  {
    return; // Fail silently. Actual error has already been reported.
  }
  if(positions == 0)
  {
    std::cerr << "RLCSA: Positions for insertions not available!" << std::endl;
    return;
  }
  if(index.sample_rate != increment.sample_rate)
  {
    std::cerr << "RLCSA: Cannot combine indexes with different sample rates!" << std::endl;
    return;
  }

  if(index.sa_samples == 0 || increment.sa_samples == 0)
  {
    this->support_locate = this->support_display = false;
  }
  else
  {
    this->support_locate = this->support_display = true;
  }


  // Build character tables etc.
  usint distribution[CHARS];
  for(usint c = 0; c < CHARS; c++)
  {
    distribution[c] = length(index.index_ranges[c]) + length(increment.index_ranges[c]);
  }
  this->buildCharIndexes(distribution);
  this->sample_rate = index.sample_rate;
  this->number_of_sequences = index.number_of_sequences + increment.number_of_sequences;

  // Merge end points, SA samples, and Psi.
  usint psi_size = this->data_size + this->number_of_sequences;
  bool should_be_ok = true;

  #ifdef MULTITHREAD_SUPPORT
  omp_set_num_threads(threads);
  #pragma omp parallel
  {
    #pragma omp for schedule(dynamic, 1)
  #endif
    for(int c = -2; c < (int)CHARS; c++)
    {
      if(c == -2)      { this->mergeEndPoints(index, increment); }
      else if(c == -1) { this->mergeSamples(index, increment, positions);  }
      else if(distribution[c] != 0)
      {
        this->array[c] = mergeVectors<PsiVector, PsiVector::Encoder, PsiVector::Iterator>(index.array[c], increment.array[c], positions, increment.data_size + increment.number_of_sequences, psi_size, block_size);
        index.array[c] = 0;
        increment.array[c] = 0;

        if(this->array[c] == 0)
        {
          std::cerr << "RLCSA: Merge failed for vectors " << c << "!" << std::endl;
          should_be_ok = false;
        }
      }
    }
  #ifdef MULTITHREAD_SUPPORT
  }
  #endif

  this->ok = should_be_ok;
}

RLCSA::~RLCSA()
{
  for(usint c = 0; c < CHARS; c++) { delete this->array[c]; }
  delete this->sa_samples;
  delete this->end_points;
}

//--------------------------------------------------------------------------

void
RLCSA::writeTo(const std::string& base_name) const
{
  std::string array_name = base_name + ARRAY_EXTENSION;
  std::ofstream array_file(array_name.c_str(), std::ios_base::binary);
  if(!array_file)
  {
    std::cerr << "RLCSA: Error creating Psi array file!" << std::endl;
    return;
  }

  usint distribution[CHARS];
  for(usint c = 0; c < CHARS; c++)
  {
    distribution[c] = length(this->index_ranges[c]);
  }
  array_file.write((char*)distribution, CHARS * sizeof(usint));
  for(usint c = 0; c < CHARS; c++)
  {
    if(this->array[c] != 0)
    {
      this->array[c]->writeTo(array_file);
    }
  }

  this->end_points->writeTo(array_file);
  array_file.write((char*)&(this->sample_rate), sizeof(this->sample_rate));
  array_file.close();

  if(this->support_locate || this->support_display)
  {
    std::string sa_sample_name = base_name + SA_SAMPLES_EXTENSION;
    std::ofstream sa_sample_file(sa_sample_name.c_str(), std::ios_base::binary);
    if(!sa_sample_file)
    {
      std::cerr << "RLCSA: Error creating suffix array sample file!" << std::endl;
      return;
    }

    this->sa_samples->writeTo(sa_sample_file);
    sa_sample_file.close();
  }

  Parameters parameters;
  parameters.set(RLCSA_BLOCK_SIZE.first, this->getBlockSize() * sizeof(usint));
  parameters.set(SAMPLE_RATE.first, this->sample_rate);
  parameters.set(SUPPORT_LOCATE.first, this->support_locate);
  parameters.set(SUPPORT_DISPLAY.first, this->support_display);
  if(this->sa_samples != 0 && this->sa_samples->isWeighted())
  {
    parameters.set(WEIGHTED_SAMPLES.first, 1);
  }
  else { parameters.set(WEIGHTED_SAMPLES); }
  parameters.write(base_name + PARAMETERS_EXTENSION);
}

bool
RLCSA::isOk() const
{
  return this->ok;
}

RLCSA * 
RLCSA::load(const std::string& base_name){

	 RLCSA *rlcsa = new RLCSA(base_name, false);
	return rlcsa;
}

//--------------------------------------------------------------------------

pair_type
RLCSA::count(const std::string& pattern) const
{
  if(pattern.length() == 0) { return this->getSARange(); }

  std::string::const_reverse_iterator iter = pattern.rbegin();
  pair_type index_range = this->getCharRange((uchar)*iter);
  if(isEmpty(index_range)) { return index_range; }

  for(++iter; iter != pattern.rend(); ++iter)
  {
    index_range = this->LF(index_range, (uchar)*iter);
    if(isEmpty(index_range)) { return EMPTY_PAIR; }
  }
  this->convertToSARange(index_range);

  return index_range;
}
//--------------------------------------------------------------------------

size_t 
RLCSA::count(uchar *pattern, ulong length, ulong *numocc) const{
    pair_type index_range;
    std::stringstream s;
    s << pattern;
    std::string p = s.str();
    index_range = count(p);
    *numocc = index_range.second - index_range.first + 1;
    return 0;
}
size_t 
RLCSA::getSA(size_t i) const{
    return locate(i);
} 

size_t 
RLCSA::getISA(size_t i) const{
    //assuming only one sequence
    pair_type seq_range = getSequenceRange(0);
    usint x = sa_samples->inverseSA(seq_range.first).second;
    if(i == 0)
        return x;

    PsiVector::Iterator** iters =getIterators();
     // Invariant: x == inverseSA(j)
    for(usint j = seq_range.first; j <= seq_range.second; j++)
    {
        x = NextinverseSA(x, iters);
        if(j==i-1)
		return x; 
    }
    return 0;
}

size_t 
RLCSA::getPsi(size_t i) const{
    usint p = psi(i);
    if(p == 0) 
	return 0;
 
    convertToSAIndex(p);
    return p;
}

size_t 
RLCSA::getPsi(size_t i, uint d) const{
	
	//uint srate = this->sa_samples->getSampleRate();
	//uint p1 = i/srate;
	//uint p2 = (i+d)/srate;
	//if d is two blocks far away from d, we solve it
	//computing SA^-1[SA]
	//if ((p2-p1)>=2){
	//	i = getISA(getSA(i)+d);
		//i = getSA(i)+d;
	//}else{
		//if the position i+d is close (is less than 2 blocks from i)
		//we iterate using psi
		while(d--) {
		    if (i == 0) return 0;
            i=psi(i)-1;
		}

	//}
	return i;
}

uchar RLCSA::getT(size_t i) const{
    
    return this->getCharacter(i);
}

size_t 
RLCSA::index_length() const{
    return this->data_size;

}

size_t 
RLCSA::getSize() const{
    return reportSize(false);
}
//--------------------------------------------------------------------------

void
RLCSA::reportPositions(uchar* data, usint length, usint* positions) const
{
  if(data == 0 || length == 0 || positions == 0) { return; }

  PsiVector::Iterator** iters = this->getIterators();

  usint current = this->number_of_sequences - 1;
  positions[length] = current; // "immediately after current"
  for(sint i = (sint)(length - 1); i >= 0; i--)
  {
    usint c = (usint)data[i];
    if(this->array[c] != 0)
    {
      current = this->LF(current, c, *(iters[c]));
    }
    else
    {
      if(c < this->text_chars[0]) // No previous characters either.
      {
        current = this->number_of_sequences - 1;
      }
      else
      {
        current = this->index_ranges[c].first - 1 + this->number_of_sequences;
      }
    }
    positions[i] = current; // "immediately after current"
  }

  this->deleteIterators(iters);
}

//--------------------------------------------------------------------------

usint*
RLCSA::locate(pair_type range) const
{
  if(!(this->support_locate) || isEmpty(range) || range.second >= this->data_size) { return 0; }

  usint* data = new usint[length(range)];
  this->locateUnsafe(range, data);

  return data;
}

usint*
RLCSA::locate(pair_type range, usint* data) const
{
  if(!(this->support_locate) || isEmpty(range) || range.second >= this->data_size || data == 0) { return 0; }
  this->locateUnsafe(range, data);
  return data;
}

void
RLCSA::locateUnsafe(pair_type range, usint* data) const
{
  this->convertToBWTRange(range);
  usint items = length(range);
  usint* offsets = new usint[items];
  bool* finished = new bool[items];  // FIXME This could be more space efficient...

  PsiVector::Iterator** iters = this->getIterators();

  for(usint i = 0, j = range.first; i < items; i++, j++)
  {
    data[i] = j;
    offsets[i] = 0;
    finished[i] = false;
  }

  bool found = false;
  while(!found)
  {
    found = true;
    pair_type run = EMPTY_PAIR;
    for(usint i = 0; i < items; i++)
    {
      if(finished[i])
      {
        continue; // The run might continue after this.
      }
      else if(isEmpty(run))
      {
        run = pair_type(i, i);
      }
      else if(data[i] - data[run.first] == i - run.first)
      {
        run.second = i;
      }
      else
      {
        found &= this->processRun(run, data, offsets, finished, iters);
        run = pair_type(i, i);
      }
    }
    if(!isEmpty(run)) { found &= this->processRun(run, data, offsets, finished, iters); }
  }

  this->deleteIterators(iters);
  delete[] offsets;
  delete[] finished;
}

bool
RLCSA::processRun(pair_type run, usint* data, usint* offsets, bool* finished, PsiVector::Iterator** iters) const
{
  bool found = true;
  usint run_start = 0, run_left = 0;
  pair_type next_sample = pair_type(0, 0);

  for(usint i = run.first; i <= run.second; i++)
  {
    if(finished[i])
    {
      if(run_left > 0) { run_left--; }
      continue;
    }
    if(data[i] < this->number_of_sequences) // Implicit sample here.
    {
      DeltaVector::Iterator iter(*(this->end_points));
      data[i] = iter.select(data[i]) + 1 - offsets[i];
      finished[i] = true;
      if(run_left > 0) { run_left--; }
      continue;
    }
    if(next_sample.first < data[i]) // Need another sample.
    {
      next_sample = this->sa_samples->getFirstSampleAfter(data[i] - this->number_of_sequences);
      next_sample.first += this->number_of_sequences;
    }
    if(data[i] < next_sample.first) // No sample found for current position.
    {
      if(run_left > 0)
      {
        data[i] = data[run_start] + i - run_start;
        run_left--;
      }
      else
      {
        pair_type value = this->psi(data[i] - this->number_of_sequences, run.second - i, iters);
        data[i] = value.first;
        run_left = value.second;
        run_start = i;
      }
      offsets[i]++;
      found = false;
    }
    else  // Sampled position found.
    {
      data[i] = this->sa_samples->getSample(next_sample.second) - offsets[i];
      finished[i] = true;
      if(run_left > 0) { run_left--; }
    }
  }
  return found;
}

usint
RLCSA::locate(usint index) const
{
  if(!(this->support_locate) || index >= this->data_size) { return this->data_size; }

  usint offset = 0;
  index += this->number_of_sequences;
  while(true)
  {
    if(index < this->number_of_sequences) // Implicit sample here
    {
      DeltaVector::Iterator iter(*(this->end_points));
      return iter.select(index) + 1 - offset;
    }
    pair_type next_sample = this->sa_samples->getFirstSampleAfter(index - this->number_of_sequences);
    next_sample.first += this->number_of_sequences;
    if(next_sample.first == index)
    {
      return this->sa_samples->getSample(next_sample.second) - offset;
    }
    index = this->psi(index - this->number_of_sequences);
    offset++;
  }
}

//--------------------------------------------------------------------------

uchar*
RLCSA::display(usint sequence) const
{
  if(!(this->support_display)) { return 0; }

  pair_type seq_range = this->getSequenceRange(sequence);
  if(isEmpty(seq_range)) { return 0; }

  uchar* data = new uchar[length(seq_range)];
  this->displayUnsafe(seq_range, data);

  return data;
}

uchar*
RLCSA::display(usint sequence, pair_type range) const
{
  if(!(this->support_display) || isEmpty(range)) { return 0; }

  pair_type seq_range = this->getSequenceRange(sequence);
  if(isEmpty(seq_range)) { return 0; }

  range.first += seq_range.first; range.second += seq_range.first;
  if(range.second > seq_range.second) { return 0; }

  uchar* data = new uchar[length(range)];
  this->displayUnsafe(range, data);

  return data;
}

uchar*
RLCSA::display(usint sequence, pair_type range, uchar* data) const
{
  if(!(this->support_display) || isEmpty(range) || data == 0) { return 0; }

  pair_type seq_range = this->getSequenceRange(sequence);
  if(isEmpty(seq_range)) { return 0; }

  range.first += seq_range.first; range.second += seq_range.first;
  if(range.second > seq_range.second) { return 0; }

  this->displayUnsafe(range, data);
  return data;
}

uchar*
RLCSA::display(usint position, usint len, usint context, usint& result_length) const
{
  if(!(this->support_display)) { return 0; }

  pair_type range = this->getSequenceRangeForPosition(position);
  if(isEmpty(range)) { return 0; }

  range.first = position - std::min(context, position - range.first);
  range.second = std::min(range.second, position + len + context - 1);
  result_length = length(range);
  if(isEmpty(range)) { return 0; }

  uchar* data = new uchar[length(range)];
  this->displayUnsafe(range, data);

  return data;
}

usint
RLCSA::displayPrefix(usint sequence, usint len, uchar* data) const
{
  if(!(this->support_display) || len == 0 || data == 0) { return 0; }

  pair_type seq_range = this->getSequenceRange(sequence);
  if(isEmpty(seq_range)) { return 0; }

  pair_type range(seq_range.first, std::min(seq_range.second, seq_range.first + len - 1));

  this->displayUnsafe(range, data);
  return length(range);
}

usint
RLCSA::displayFromPosition(usint index, usint max_len, uchar* data) const
{
  if(max_len == 0 || data == 0 || index >= this->data_size) { return 0; }

  for(usint i = 0; i < max_len; i++)
  {
    data[i] = this->getCharacter(index);
    index = this->psiUnsafe(index, data[i]);
    if(index < this->number_of_sequences) { return i + 1; }
    index -= this->number_of_sequences;
  }

  return max_len;
}

void
RLCSA::displayUnsafe(pair_type range, uchar* data) const
{
  pair_type res = this->sa_samples->inverseSA(range.first);
  usint i = res.first, pos = res.second;

  if(length(range) >= 1024)
  {
    PsiVector::Iterator** iters = this->getIterators();
    for(; i < range.first; i++)
    {
      pos = this->psi(pos, iters) - this->number_of_sequences;
    }
    for(; i <= range.second; i++)
    {
      usint c = this->getCharacter(pos);
      data[i - range.first] = c;
      pos = this->psiUnsafe(pos, c, *(iters[c])) - this->number_of_sequences;
    }
    this->deleteIterators(iters);
  }
  else
  {
    for(; i < range.first; i++)
    {
      pos = this->psi(pos) - this->number_of_sequences;
    }
    for(; i <= range.second; i++)
    {
      usint c = this->getCharacter(pos);
      data[i - range.first] = c;
      pos = this->psiUnsafe(pos, c) - this->number_of_sequences;
    }
  }
}

//--------------------------------------------------------------------------

pair_type
RLCSA::getSARange() const
{
  return pair_type(0, this->data_size - 1);
}

pair_type
RLCSA::getBWTRange() const
{
  return pair_type(this->number_of_sequences, this->number_of_sequences + this->data_size - 1);
}

pair_type
RLCSA::getCharRange(usint c) const
{
  if(c >= CHARS) { return EMPTY_PAIR; }
  pair_type index_range = this->index_ranges[c];
  this->convertToBWTRange(index_range);
  return index_range;
}

void
RLCSA::convertToSARange(pair_type& bwt_range) const
{
  bwt_range.first -= this->number_of_sequences;
  bwt_range.second -= this->number_of_sequences;
}

void
RLCSA::convertToBWTRange(pair_type& sa_range) const
{
  sa_range.first += this->number_of_sequences;
  sa_range.second += this->number_of_sequences;
}

void
RLCSA::convertToSARange(std::vector<pair_type>& bwt_ranges) const
{
  for(std::vector<pair_type>::iterator iter = bwt_ranges.begin(); iter != bwt_ranges.end(); ++iter)
  {
    this->convertToSARange(*iter);
  }
}

pair_type
RLCSA::LF(pair_type range, usint c) const
{
  if(c >= CHARS || this->array[c] == 0) { return EMPTY_PAIR; }
  PsiVector::Iterator iter(*(this->array[c]));

  usint start = this->index_ranges[c].first + this->number_of_sequences - 1;
  range.first = start + iter.rank(range.first, true);
  range.second = start + iter.rank(range.second);

  return range;
}

std::vector<usint>*
RLCSA::locateRange(pair_type range) const
{
  std::vector<usint>* results = new std::vector<usint>;
  if(!(this->support_locate)) { return results; }

  this->locateRange(range, *results);
  removeDuplicates(results);

  return results;
}

void
RLCSA::locateRange(pair_type range, std::vector<usint>& vec) const
{
  if(isEmpty(range) || range.second >= this->data_size) { return; }

  usint* data = new usint[length(range)];
  this->locateUnsafe(range, data);
  for(usint i = 0; i < length(range); i++) { vec.push_back(data[i]); }
  delete[] data;
}

std::vector<usint>*
RLCSA::locateRanges(std::vector<pair_type>& ranges) const
{
  std::vector<usint>* results = new std::vector<usint>;
  if(!(this->support_locate)) { return results; }

  for(std::vector<pair_type>::iterator iter = ranges.begin(); iter != ranges.end(); ++iter)
  {
    this->locateRange(*iter, *results);
  }
  removeDuplicates(results);

  return results;
}

//--------------------------------------------------------------------------

pair_type
RLCSA::getSequenceRange(usint number) const
{
  if(number >= this->number_of_sequences) { return EMPTY_PAIR; }

  pair_type result;
  DeltaVector::Iterator iter(*(this->end_points));
  if(number == 0)
  {
    result.first = 0;
    result.second = iter.select(number);
  }
  else
  {
    result.first = nextMultipleOf(this->sample_rate, iter.select(number - 1));
    result.second = iter.selectNext();
  }

  return result;
}

usint
RLCSA::getSequenceForPosition(usint value) const
{
  if(value == 0) { return 0; }
  DeltaVector::Iterator iter(*(this->end_points));
  return iter.rank(value - 1);
}

pair_type
RLCSA::getSequenceRangeForPosition(usint value) const
{
  return this->getSequenceRange(this->getSequenceForPosition(value));
}

usint*
RLCSA::getSequenceForPosition(usint* values, usint len) const
{
  if(values == 0) { return 0; }

  DeltaVector::Iterator iter(*(this->end_points));
  for(usint i = 0; i < len; i++)
  {
    if(values[i] > 0) { values[i] = iter.rank(values[i] - 1); }
  }

  return values;
}

pair_type
RLCSA::getRelativePosition(usint value) const
{
  DeltaVector::Iterator iter(*(this->end_points));
  pair_type result(0, value);

  if(value > 0) { result.first = iter.rank(value - 1); }
  if(result.first > 0)
  {
    result.second -= nextMultipleOf(this->sample_rate, iter.select(result.first - 1));
  }

  return result;
}

//--------------------------------------------------------------------------

uchar*
RLCSA::readBWT() const
{
  return this->readBWT(pair_type(0, this->data_size + this->number_of_sequences - 1));
}

uchar*
RLCSA::readBWT(pair_type range) const
{
  if(isEmpty(range) || range.second >= this->data_size + this->number_of_sequences) { return 0; }

  usint n = length(range);

  uchar* bwt = new uchar[n];
  memset(bwt, 0, n);

  for(usint c = 0; c < CHARS; c++)
  {
    if(this->array[c] != 0)
    {
      PsiVector::Iterator iter(*(this->array[c]));
      usint pos = iter.valueAfter(range.first).first;
      while(pos <= range.second)
      {
        bwt[pos - range.first] = c;
        pos = iter.selectNext();
      }
    }
  }

  return bwt;
}

usint
RLCSA::countRuns() const
{
  usint runs = 0;
  for(usint c = 0; c < CHARS; c++)
  {
    if(this->array[c] != 0)
    {
      PsiVector::Iterator iter(*(this->array[c]));
      runs += iter.countRuns();
    }
  }

  return runs;
}

//--------------------------------------------------------------------------

PsiVector::Iterator**
RLCSA::getIterators() const
{
  PsiVector::Iterator** iters = new PsiVector::Iterator*[CHARS];
  for(usint i = 0; i < CHARS; i++)
  {
    if(this->array[i] == 0) { iters[i] = 0; }
    else                    { iters[i] = new PsiVector::Iterator(*(this->array[i])); }
  }
  return iters;
}

void
RLCSA::deleteIterators(PsiVector::Iterator** iters) const
{
  if(iters == 0) { return; }
  for(usint i = 0; i < CHARS; i++) { delete iters[i]; }
  delete[] iters;
}

//--------------------------------------------------------------------------

usint
RLCSA::reportSize(bool print) const
{
  usint bytes = 0, temp = 0, bwt = 0;

  for(usint c = 0; c < CHARS; c++)
  {
    if(this->array[c])
    {
      bytes += this->array[c]->reportSize();
      bwt += this->array[c]->getCompressedSize();
    }
  }
  bytes += sizeof(*this) + this->end_points->reportSize();
  if(print)
  {
    std::cout << "RLCSA:           " << (bytes / (double)MEGABYTE) << " MB" << std::endl;
    std::cout << "  BWT only:      " << (bwt / (double)MEGABYTE) << " MB" << std::endl;
  }

  if(this->support_locate || this->support_display)
  {
    temp = this->sa_samples->reportSize();
    if(print) { std::cout << "SA samples:      " << (temp / (double)MEGABYTE) << " MB" << std::endl; }
    bytes += temp;
  }

  if(print)
  {
    std::cout << "Total size:      " << (bytes / (double)MEGABYTE) << " MB" << std::endl;
    std::cout << std::endl;
  }

  return bytes;
}

    usint
    RLCSA::reportSizeCount(bool print) const
    {
        usint bytes = 0, bwt = 0;

        for(usint c = 0; c < CHARS; c++)
        {
            if(this->array[c])
            {
                bytes += this->array[c]->reportSize();
                bwt += this->array[c]->getCompressedSize();
            }
        }
        bytes += sizeof(*this) + this->end_points->reportSize();
        if(print)
        {
            std::cout << "RLCSA:           " << (bytes / (double)MEGABYTE) << " MB" << std::endl;
            std::cout << "  BWT only:      " << (bwt / (double)MEGABYTE) << " MB" << std::endl;
        }

        return bytes;
    }

    void
RLCSA::printInfo() const
{
  double megabytes = this->data_size / (double)MEGABYTE;

  std::cout << "Sequences:       " << this->number_of_sequences << std::endl;
  std::cout << "Original size:   " << megabytes << " MB" << std::endl;
  std::cout << "Block size:      " << (this->getBlockSize() * sizeof(usint)) << " bytes" << std::endl;
  if(this->support_locate || this->support_display)
  {
    if(this->sa_samples->isWeighted())
    {
      std::cout << "Sample rate:     " << (this->data_size / this->sa_samples->getNumberOfSamples()) << " (weighted)" << std::endl;
    }
    else
    {
      std::cout << "Sample rate:     " << this->sample_rate << std::endl;
    }
  }
  std::cout << std::endl;
}


//--------------------------------------------------------------------------

void
RLCSA::mergeEndPoints(RLCSA& index, RLCSA& increment)
{
  DeltaEncoder* endings = new DeltaEncoder(RLCSA::ENDPOINT_BLOCK_SIZE);

  DeltaVector::Iterator index_iter(*(index.end_points));
  DeltaVector::Iterator increment_iter(*(increment.end_points));

  endings->setBit(index_iter.select(0));
  for(usint i = 1; i < index.number_of_sequences; i++)
  {
    endings->setBit(index_iter.selectNext());
  }
  usint sum = index.end_points->getSize();
  delete index.end_points; index.end_points = 0;

  endings->setBit(sum + increment_iter.select(0));
  for(usint i = 1; i < increment.number_of_sequences; i++)
  {
    endings->setBit(sum + increment_iter.selectNext());
  }
  sum += increment.end_points->getSize();
  delete increment.end_points; increment.end_points = 0;

  this->end_points = new DeltaVector(*endings, sum);
  delete endings;
}


void
RLCSA::mergeSamples(RLCSA& index, RLCSA& increment, usint* positions)
{
  if(this->support_locate || this->support_display)
  {
    positions += increment.number_of_sequences;
    this->sa_samples = new SASamples(*(index.sa_samples), *(increment.sa_samples), positions, increment.data_size, this->number_of_sequences);
  }
}

//--------------------------------------------------------------------------

void
RLCSA::buildCharIndexes(usint* distribution)
{
 
  this->data_size = buildRanges(distribution, this->index_ranges);

  usint i = 0, c = 0;
  for(; c < CHARS; c++)
  {
    if(!isEmpty(this->index_ranges[c]))
    {
      this->text_chars[i] = c;
      i++;
    }
  }
  this->chars = i;

  this->index_rate = std::max((this->data_size + CHARS - 1) / CHARS, (usint)1);
  usint current = 0;

  for(c = 0, i = 0; c < this->chars; c++)
  {
    pair_type range = this->index_ranges[this->text_chars[c]];
    while(current <= range.second)
    {
      this->index_pointers[i] = c;
      current += this->index_rate;
      i++;
    }
  }
}

usint
buildRanges(usint* distribution, pair_type* index_ranges)
{
  if(distribution == 0 || index_ranges == 0) { return 0; }

  usint sum = 0;
  for(usint c = 0; c < CHARS; c++)
  {
    if(distribution[c] == 0)
    {
      if(sum == 0) { index_ranges[c] = EMPTY_PAIR; }
      else         { index_ranges[c] = pair_type(sum, sum - 1); }
    }
    else
    {
      index_ranges[c].first = sum;
      sum += distribution[c];
      index_ranges[c].second = sum - 1;
    }
  }

  return sum;
}


size_t
RLCSA::NextinverseSA(usint x, PsiVector::Iterator** iters  ) const
{
	usint next_x;
  	usint c = this->getCharacter(x);

	// T[i,n] is lexicographically the first suffix beginning with c.
	if(x == this->index_ranges[c].first)
	{
	next_x = this->psiUnsafe(x, c, *(iters[c])) - this->number_of_sequences;
	return next_x;
	}

	// If PLCP[i] is minimal, we add the left match to the list.
	// We add pairs of type (j, inverseSA(j) + n_of_s) as inverseSA is < 0 for end markers.
	this->psiUnsafe(x - 1, c, *(iters[c]));
	next_x = this->psiUnsafeNext(c, *(iters[c])) - this->number_of_sequences;
	return next_x;
}



} // namespace TextIndexes
