#ifndef SASAMPLES_H
#define SASAMPLES_H

#include <fstream>

#include "sampler.h"
#include "definitions.h"
#include "bitbuffer.h"
#include "deltavector.h"


namespace TextIndexes
{


class SASamples
{
  public:
    const static usint INDEX_BLOCK_SIZE = 16;

    SASamples(std::ifstream& sample_file, usint sample_rate, bool _weighted);

    // These assume < 2 GB data. Use the second one when multiple_sequences is true
    // in RLCSA construction.
    SASamples(uint* array, usint data_size, usint sample_rate);
    SASamples(uint* inverse, DeltaVector* end_points, usint data_size, usint sample_rate);

    // Use the given samples.
    SASamples(uint* inverse, WeightedSampler& sampler);

    ~SASamples();

    // Destroys contents of index and increment.
    // We assume index and increment have same sample rate.
    // positions must not containt the positions of end of sequence markers.
    // number_of_sequences is subtracted from each position before the value is used.
    SASamples(SASamples& index, SASamples& increment, usint* positions, usint number_of_positions, usint number_of_sequences);

    void writeTo(std::ofstream& sample_file) const;

    // Returns (i, inverseSA(i)) such that i is the last sampled position up to value.
    // Value is actual 0-based suffix array value.
    // Returns (size, size) if value is too large.
    pair_type inverseSA(usint value) const;

    // Returns the value of ith sample in suffix array order.
    inline usint getSample(usint i) const
    {
      return std::min(this->samples->readItemConst(i) * this->rate, this->size - 1);
    }

    // Returns (ind, sample number) where ind >= index or (size, ???).
    inline pair_type getFirstSampleAfter(usint index) const
    {
      DeltaVector::Iterator iter(*(this->indexes));
      return iter.valueAfter(index);
    }
    inline usint getSampleRate() const { return this->rate; }
    inline usint getNumberOfSamples() const { return this->items; }
    inline bool isWeighted() const { return this->weighted; }

    usint reportSize() const;

  private:
    bool weighted;
    usint rate, size, items;

    DeltaVector* indexes;
    ReadBuffer*  samples;

    DeltaVector* inverse_indexes;
    ReadBuffer*  inverse_samples;

    void buildInverseSamples();

    // Weighted case.
    void buildSamples(pair_type* sample_pairs, bool inverse);

    // Note: contents of original samples are deleted.
    void mergeSamples(SASamples& index, SASamples& increment, usint* positions, usint n, usint skip);

    // These are not allowed.
    SASamples();
    SASamples(const SASamples&);
    SASamples& operator = (const SASamples&);
};


} // namespace TextIndexes


#endif // SASAMPLES_H
