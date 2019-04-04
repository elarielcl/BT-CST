#ifndef TEXT_INDEX_RLCSA_H
#define TEXT_INDEX_RLCSA_H

#include <fstream>
#include <vector>

#include <rlcsa/deltavector.h>
#include <rlcsa/rlevector.h>
#include <rlcsa/nibblevector.h>

#include <rlcsa/sasamples.h>
#include <rlcsa/parameters.h>
#include <rlcsa/sampler.h>


namespace TextIndexes
{


const std::string SA_EXTENSION = ".sa";
const std::string PSI_EXTENSION = ".psi";
const std::string ARRAY_EXTENSION = ".rlcsa.array";
const std::string SA_SAMPLES_EXTENSION = ".rlcsa.sa_samples";
const std::string PARAMETERS_EXTENSION = ".rlcsa.parameters";
const std::string LCP_SAMPLES_EXTENSION = ".lcp_samples";
const std::string PLCP_EXTENSION = ".plcp";


const parameter_type RLCSA_BLOCK_SIZE  = parameter_type("RLCSA_BLOCK_SIZE", 32);
const parameter_type SAMPLE_RATE       = parameter_type("SAMPLE_RATE", 128);
const parameter_type SUPPORT_LOCATE    = parameter_type("SUPPORT_LOCATE", 1);
const parameter_type SUPPORT_DISPLAY   = parameter_type("SUPPORT_DISPLAY", 1);
const parameter_type WEIGHTED_SAMPLES  = parameter_type("WEIGHTED_SAMPLES", 0);


#ifdef USE_NIBBLE_VECTORS
typedef NibbleVector  PsiVector;
#else
typedef RLEVector     PsiVector;
#endif


class RLCSA
{
  public:

//--------------------------------------------------------------------------
//  CONSTRUCTION
//--------------------------------------------------------------------------

    const static usint ENDPOINT_BLOCK_SIZE = 16;

    RLCSA(const std::string& base_name, bool print = false);

    /*
      If multiple_sequences is true, each 0 is treated as a end of sequence marker.
      There must be nonzero characters between the 0s. data[bytes - 1] must also be 0.
      FIXME Crashes if bytes >= 2 GB.
    */ 
    RLCSA(uchar* data, usint bytes, usint block_size, usint sa_sample_rate, bool multiple_sequences, bool delete_data);

    /*
      Build an RLCSA for a single sequence, and use the samples given by sampler.
      FIXME This cannot be merged. Maybe should enforce it somehow.
    */
    RLCSA(uchar* data, usint bytes, usint block_size, WeightedSampler& sampler, bool delete_data);

    // Destroys contents of index and increment.
    // threads has no effect unless MULTITHREAD_SUPPORT is defined
    RLCSA(RLCSA& index, RLCSA& increment, usint* positions, usint block_size, usint threads = 1);
    ~RLCSA();

    void writeTo(const std::string& base_name) const;

    bool isOk() const;

//--------------------------------------------------------------------------
//  QUERIES
//--------------------------------------------------------------------------

    // These queries use SA ranges.

    // Returns the closed range containing the matches.
    pair_type count(const std::string& pattern) const;

    // Used when merging CSAs.
    void reportPositions(uchar* data, usint length, usint* positions) const;

    // Returns SA[range]. User must free the buffer. Latter version uses buffer provided by the user.
    usint* locate(pair_type range) const;
    usint* locate(pair_type range, usint* data) const;

    // Returns SA[index].
    usint locate(usint index) const;

    // Returns T^{sequence}[range]. User must free the buffer.
    // Third version uses buffer provided by the user.
    uchar* display(usint sequence) const;
    uchar* display(usint sequence, pair_type range) const;
    uchar* display(usint sequence, pair_type range, uchar* data) const;

    // Displays the intersection of T[position - context, position + len + context - 1]
    // and T^{getSequenceForPosition(position)}.
    // This is intended for displaying an occurrence of a pattern of length 'len'
    // with 'context' extra characters on both sides.
    // The actual length of the returned string is written into result_length.
    uchar* display(usint position, usint len, usint context, usint& result_length) const;

    // Returns the actual length of the prefix. User must provide the buffer.
    usint displayPrefix(usint sequence, usint len, uchar* data) const;

    // Returns at most max_len characters starting from T[SA[index]].
    // User must provide the buffer. Returns the number of characters in buffer.
    usint displayFromPosition(usint index, usint max_len, uchar* data) const;

    // Get the range of SA values for the sequence identified by
    // a sequence number or a SA value.
    pair_type getSequenceRange(usint number) const;
    pair_type getSequenceRangeForPosition(usint value) const;

    // Get the sequence number for given SA value(s).
    // The returned array is the same as the parameter.
    usint getSequenceForPosition(usint value) const;
    usint* getSequenceForPosition(usint* value, usint salength) const;

    // Changes SA value to (sequence, offset).
    pair_type getRelativePosition(usint value) const;

    // Returns the BWT of the collection including end of sequence markers.
    uchar* readBWT() const;
    uchar* readBWT(pair_type range) const;

    // Returns the number of equal letter runs in the BWT. Runs consisting of end markers are ignored.
    usint countRuns() const;
//--------------------------------------------------------------------------
//  QUERIES FOR SUPPORTING CANOVAS SUFFIX TREE
//--------------------------------------------------------------------------
    /* Writes in numocc the number of occurrences of the substring 
     * pattern[0..length-1] found in the text indexed by index. */
    size_t count (uchar *pattern, ulong length, ulong *numocc) const;
    
    /*Get the suffix array at position i*/
    size_t getSA(size_t i) const; 

   /*Get the inverse of the suffix array at position i*/
    /*for the moment sequencial*/
    size_t getISA(size_t i) const;

    /*Get Psi at position i*/
    size_t getPsi(size_t i) const;
    
    /*Applies psi d times starting at i*/
    size_t getPsi(size_t i, uint d) const;
   
    /*Get the first letter of the i-th suffix in the suffix array*/
     uchar getT(size_t i) const;

    /*Return the length of the Index*/
    size_t index_length() const;

    size_t getSize() const;

    static RLCSA * load(const std::string& base_name);
//--------------------------------------------------------------------------
//  BASIC OPERATIONS
//--------------------------------------------------------------------------

    // The return values of these functions are BWT indexes/ranges.
    // To get SA indexes/ranges, use
    //   RLCSA::convertToSAIndex(bwt_index)
    //   RLCSA::convertToSARange(bwt_range).

    inline usint psi(usint index) const
    {
      if(index >= this->data_size)
      {
        return this->data_size + this->number_of_sequences;
      }

      usint c = this->getCharacter(index);
      return this->psiUnsafe(index, c);
    }

    // This version returns a run.
    inline pair_type psi(usint index, usint max_length) const
    {
      if(index >= this->data_size)
      {
        return pair_type(this->data_size + this->number_of_sequences, 0);
      }

      usint c = this->getCharacter(index);
      PsiVector::Iterator iter(*(this->array[c]));
      return iter.selectRun(index - this->index_ranges[c].first, max_length);
    }

    inline usint LF(usint index, usint c) const
    {
      if(c >= CHARS)
      {
        return this->data_size + this->number_of_sequences;
      }
      if(this->array[c] == 0)
      {
        if(c < this->text_chars[0]) { return this->number_of_sequences - 1; }
        return this->index_ranges[c].first + this->number_of_sequences - 1;
      }
      index += this->number_of_sequences;

      PsiVector::Iterator iter(*(this->array[c]));
      return this->LF(index, c, iter);
    }

    inline void convertToSAIndex(usint& bwt_index) const { bwt_index -= this->number_of_sequences; }
    inline void convertToBWTIndex(usint& sa_index) const { sa_index += this->number_of_sequences; }

//--------------------------------------------------------------------------
//  SUPPORT FOR LCP CONSTRUCTION
//--------------------------------------------------------------------------
    
    size_t NextinverseSA(usint prev,  PsiVector::Iterator** iters ) const;
    // Creates an array of iterators for every vector in this->array.
    PsiVector::Iterator** getIterators() const;
    void deleteIterators(PsiVector::Iterator** iters) const;

//--------------------------------------------------------------------------
//  SUPPORT FOR APPROXIMATE MATCHING MODULES
//--------------------------------------------------------------------------

    pair_type getSARange() const;
    pair_type getBWTRange() const;
    pair_type getCharRange(usint c) const;

    void convertToBWTRange(pair_type& sa_range) const;
    void convertToSARange(pair_type& bwt_range) const;
    void convertToSARange(std::vector<pair_type>& bwt_ranges) const;

    // This is an unsafe function that does not check its parameters.
    pair_type LF(pair_type bwt_range, usint c) const;

    // User must free the returned vector.
    std::vector<usint>* locateRange(pair_type range) const;
    std::vector<usint>* locateRanges(std::vector<pair_type>& ranges) const;

//--------------------------------------------------------------------------
//  REPORTING
//--------------------------------------------------------------------------

    inline bool supportsLocate() const { return this->support_locate; }
    inline bool supportsDisplay() const { return this->support_display; }
    inline usint getLength() const { return this->data_size; }
    inline usint getNumberOfSequences() const { return this->number_of_sequences; }
    inline usint getBlockSize() const { return this->array[this->text_chars[0]]->getBlockSize(); }

    // Returns the size of the data structure.
    usint reportSize(bool print = false) const;
        usint reportSizeCount(bool print = false) const;
    void printInfo() const;



//--------------------------------------------------------------------------
//  INTERNAL VARIABLES
//--------------------------------------------------------------------------

  private:
    
    bool ok;

    PsiVector* array[CHARS];
    SASamples* sa_samples;

    usint number_of_sequences;

    // index_ranges[c] is SA range for suffixes starting with 'c'.
    pair_type index_ranges[CHARS];
    usint data_size;

    usint text_chars[CHARS];  // which characters are present in the text
    usint chars;

    usint index_pointers[CHARS]; // which of the above is at i * index_rate
    usint index_rate;

    bool support_locate, support_display;

    // A sequence starts at the next multiple of sample_rate after the end of previous sequence.
    usint sample_rate;
    DeltaVector* end_points;

//--------------------------------------------------------------------------
//  INTERNAL VERSIONS OF QUERIES
//--------------------------------------------------------------------------

    void locateUnsafe(pair_type range, usint* data) const;
    bool processRun(pair_type run, usint* data, usint* offsets, bool* finished, PsiVector::Iterator** iters) const;
    void displayUnsafe(pair_type range, uchar* data) const;

    void locateRange(pair_type range, std::vector<usint>& vec) const;

//--------------------------------------------------------------------------
//  INTERNAL VERSIONS OF BASIC OPERATIONS
//--------------------------------------------------------------------------

    inline usint psi(usint index, PsiVector::Iterator** iters) const
    {
      usint c = this->getCharacter(index);
      return iters[c]->select(index - this->index_ranges[c].first);
    }

    inline pair_type psi(usint index, usint max_length, PsiVector::Iterator** iters) const
    {
      usint c = this->getCharacter(index);
      return iters[c]->selectRun(index - this->index_ranges[c].first, max_length);
    }

    // Returns psi(index), assuming the suffix of rank index begins with c.
    inline usint psiUnsafe(usint index, usint c) const
    {
      PsiVector::Iterator iter(*(this->array[c]));
      return this->psiUnsafe(index, c, iter);
    }

    // As above, but with a given iterator.
    inline usint psiUnsafe(usint index, usint c, PsiVector::Iterator& iter) const
    {
      return iter.select(index - this->index_ranges[c].first);
    }

    // As above, but returns the next value of psi.
    inline usint psiUnsafeNext(usint c, PsiVector::Iterator& iter) const
    {
      return iter.selectNext();
    }

    inline usint LF(usint index, usint c, PsiVector::Iterator& iter) const
    {
      return this->index_ranges[c].first + this->number_of_sequences + iter.rank(index) - 1;
    }

//--------------------------------------------------------------------------
//  INTERNAL STUFF
//--------------------------------------------------------------------------

    inline usint getCharacter(usint pos) const
    {
      const usint* curr = &(this->text_chars[this->index_pointers[pos / this->index_rate]]);
      while(pos > this->index_ranges[*curr].second) { curr++; }
      return *curr;
    }

    void mergeEndPoints(RLCSA& index, RLCSA& increment);
    void mergeSamples(RLCSA& index, RLCSA& increment, usint* positions);

    void buildCharIndexes(usint* distribution);

    // These are not allowed.
    RLCSA();
    RLCSA(const RLCSA&);
    RLCSA& operator = (const RLCSA&);
};


// Returns the total number of characters.
usint buildRanges(usint* distribution, pair_type* index_ranges);


} // namespace TextIndexes


#endif // RLCSA_H
