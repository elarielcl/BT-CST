#ifndef NIBBLEVECTOR_H
#define NIBBLEVECTOR_H

#include <fstream>

#include "bitvector.h"


namespace TextIndexes
{


/*
  This class is used to construct a NibbleVector.
*/

class NibbleEncoder : public VectorEncoder
{
  public:
    NibbleEncoder(usint block_bytes, usint superblock_size = VectorEncoder::SUPERBLOCK_SIZE);
    ~NibbleEncoder();

    void setBit(usint value);
    void setRun(usint start, usint len);

    // FIXME for gap encoding
    inline void nibbleEncode(usint diff, usint len)
    {
      this->size += diff + len - 1;
      this->items += len;
      this->buffer->writeNibbleCode(diff);
      this->buffer->writeNibbleCode(len);
    }

  protected:

    // These are not allowed.
    NibbleEncoder();
    NibbleEncoder(const NibbleEncoder&);
    NibbleEncoder& operator = (const NibbleEncoder&);
};


/*
  This bit vector uses nibble coding. Each block is either run-length encoded or
  gap encoded, depending on the first nibble.

  // FIXME reverting to gap encoding not implemented yet
*/

class NibbleVector : public BitVector
{
  public:
    typedef NibbleEncoder Encoder;

    NibbleVector(std::ifstream& file);
    NibbleVector(Encoder& encoder, usint universe_size);
    ~NibbleVector();

//--------------------------------------------------------------------------

    usint reportSize() const;

//--------------------------------------------------------------------------

    class Iterator : public BitVector::Iterator
    {
      public:
        Iterator(const NibbleVector& par);
        ~Iterator();

        usint rank(usint value, bool at_least = false);

        usint select(usint index);
        usint selectNext();

        pair_type valueAfter(usint value);
        pair_type nextValue();

        pair_type selectRun(usint index, usint max_length);
        pair_type selectNextRun(usint max_length);

        bool isSet(usint value);

        // Counts the number of 1-bit runs.
        usint countRuns();

        static const usint GAP_ENCODING = 0;
        static const usint RUN_LENGTH_ENCODING = 1;

      protected:

        // FIXME for gap encoding
        inline void valueLoop(usint value)
        {
          this->getSample(this->sampleForValue(value));

          if(this->val >= value) { return; }
          while(this->cur < this->block_items)
          {
            this->val += this->buffer.readNibbleCode();
            this->cur++;
            this->run = this->buffer.readNibbleCode() - 1;
            if(this->val >= value) { break; }

            this->cur += this->run;
            this->val += this->run;
            if(this->val >= value)
            {
              this->run = this->val - value;
              this->val = value;
              this->cur -= this->run;
              break;
            }
            this->run = 0;
          }
        }

        inline void getSample(usint sample_number)
        {
          BitVector::Iterator::getSample(sample_number);
          this->run = 0;
//           this->use_rle = this->buffer.readNibble();
        }

        bool use_rle;

        // These are not allowed.
        Iterator();
        Iterator(const Iterator&);
        Iterator& operator = (const Iterator&);
    };

//--------------------------------------------------------------------------

  protected:

    // These are not allowed.
    NibbleVector();
    NibbleVector(const NibbleVector&);
    NibbleVector& operator = (const NibbleVector&);
};


} // namespace TextIndexes


#endif // NIBBLEVECTOR_H
