#ifndef BITBUFFER_H
#define BITBUFFER_H

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include "definitions.h"


namespace TextIndexes
{


class ReadBuffer
{
  public:
    ReadBuffer(std::ifstream& file, usint words) :
      size(words),
      item_bits(1),
      items(0),
      free_buffer(true)
    {
      usint* buffer = new usint[this->size];
      memset(buffer, 0, this->size * sizeof(usint));
      file.read((char*)buffer, this->size * sizeof(usint));
      this->data = buffer;
      this->reset();
    }

    ReadBuffer(std::ifstream& file, usint _items, usint item_size) :
      item_bits(item_size),
      items(_items),
      free_buffer(true)
    {
      this->size = bitsToWords(this->items * this->item_bits);
      usint* buffer = new usint[this->size];
      memset(buffer, 0, this->size * sizeof(usint));
      file.read((char*)buffer, this->size * sizeof(usint));
      this->data = buffer;
      this->reset();
    }

    // This version does not delete the data when deleted.
    ReadBuffer(const usint* buffer, usint words) :
      size(words),
      item_bits(1),
      items(0),
      free_buffer(false)
    {
      this->data = buffer;
      this->reset();
    }

    // This version does not delete the data when deleted.
    ReadBuffer(const usint* buffer, usint _items, usint item_size) :
      item_bits(item_size),
      items(_items),
      free_buffer(false)
    {
      this->size = bitsToWords(this->items * this->item_bits);
      this->data = buffer;
      this->reset();
    }

    // This version does not delete the data when deleted.
    ReadBuffer(const ReadBuffer& original) :
      data(original.data),
      size(original.size),
      item_bits(original.item_bits),
      items(original.items),
      free_buffer(false)
    {
      this->reset();
    }

    ~ReadBuffer()
    {
      if(this->free_buffer)
      {
        delete[] this->data;
      }
    }

//--------------------------------------------------------------------------

    void claimData()
    {
      this->free_buffer = true;
    }

    void writeBuffer(std::ofstream& file) const
    {
      file.write((const char*)this->data, this->size * sizeof(usint));
    }

    // The buffer will no longer own the data.
    void moveBuffer(const usint* buffer)
    {
      if(this->free_buffer)
      {
        delete[] this->data;
      }
      this->free_buffer = false;

      this->data = buffer;
      this->reset();
    }

    usint reportSize() const
    {
      usint bytes = sizeof(*this);
      if(this->free_buffer) { bytes += this->size * sizeof(usint); }
      return bytes;
    }

//--------------------------------------------------------------------------

    inline void reset()
    {
      this->pos = 0;
      this->bits = WORD_BITS;
      this->current = 0;
    }

    inline void skipBits(usint count)
    {
      if(count < this->bits)
      {
        this->bits -= count;
        return;
      }

      count -= this->bits;
      this->pos += 1 + count / WORD_BITS;
      this->bits = WORD_BITS - count % WORD_BITS;
    }

//--------------------------------------------------------------------------

    inline usint bitsLeft() const
    {
      return this->bits + WORD_BITS * (this->size - this->pos - 1);
    }

    // Returns nonzero if bit is 1
    inline usint readBit()
    {
      this->bits--;
      usint bit = this->data[this->pos] & ((usint)1 << this->bits);

      if(this->bits == 0) { this->pos++; this->bits = WORD_BITS; }

      return bit;
    }

    inline usint readBits(usint count)
    {
      usint value = 0;

      if(count >= this->bits)
      {
        count -= this->bits;
        value |= HIGHER(GET(this->data[this->pos], this->bits), count);
        this->pos++; this->bits = WORD_BITS;
      }
      if(count > 0)
      {
        this->bits -= count;
        value |= GET(LOWER(this->data[this->pos], this->bits), count);
      }

      return value;
    }

//--------------------------------------------------------------------------

    /*
      These operations work on 4-bit nibbles.
      Do not use these with the bit-level operations.
    */

    inline usint readNibble()
    {
      this->bits -= 4;
      usint value = GET(LOWER(this->data[this->pos], this->bits), 4);

      if(this->bits == 0) { this->pos++; this->bits = WORD_BITS; }

      return value;
    }

    // Nibble code for positive integers.
    inline usint readNibbleCode()
    {
      usint temp, value = 0, shift = 0;
      do
      {
        temp = this->readNibble();
        value |= (temp & 0x7) << shift;
        shift += 3;
      }
      while((temp & 0x8) == 0);

      return value + 1;
    }

//--------------------------------------------------------------------------

    /*
      These operations work on fixed-size items. No sanity checks are made
      for parameter values.
    */

    inline usint getItemSize() const
    {
      return this->item_bits;
    }

    inline void goToItem(usint item)
    {
      usint b = item * this->item_bits;
      this->pos = b / WORD_BITS;
      this->bits = WORD_BITS - b % WORD_BITS;
      this->current = item;
    }

    inline usint readItem()
    {
      this->current++;
      return this->readBits(this->item_bits);
    }

    inline usint readItem(usint item)
    {
      this->goToItem(item);
      return this->readItem();
    }

    inline usint readFirstItem()
    {
      return this->readItem(0);
    }

    inline usint readItemConst(usint item) const
    {
      usint b = item * this->item_bits;
      usint p = b / WORD_BITS;
      b = WORD_BITS - b % WORD_BITS;

      usint c = this->item_bits;
      usint value = 0;

      if(c >= b)
      {
        c -= b;
        value |= HIGHER(GET(this->data[p], b), c);
        p++; b = WORD_BITS;
      }
      if(c > 0)
      {
        b -= c;
        value |= GET(LOWER(this->data[p], b), c);
      }

      return value;
    }

    inline bool hasNextItem() const
    {
      return (this->current < this->items);
    }

    inline void skipItem()
    {
      this->skipBits(this->item_bits);
      this->current++;
    }

//--------------------------------------------------------------------------

    /*
      Delta coding for positive integers
    */

    inline usint readDeltaCode()
    {
      usint len = 0;
      while(this->readBit() == 0) { len++; }

      usint temp = (((usint)1 << len) | this->readBits(len)) - 1;
      temp = ((usint)1 << temp) | this->readBits(temp);
      return temp;
    }

//--------------------------------------------------------------------------

  private:
    const usint* data;
    usint size, item_bits, items;
    bool  free_buffer;

    // Iterator data
    usint pos, bits, current;

    inline static usint bitsToWords(usint _bits) { return (_bits + WORD_BITS - 1) / WORD_BITS; }

    // These are not allowed.
    ReadBuffer();
    ReadBuffer& operator = (const ReadBuffer&);
};


//--------------------------------------------------------------------------


class WriteBuffer
{
  public:
    WriteBuffer(usint words) :
      size(words),
      item_bits(1),
      items(0),
      free_buffer(true)
    {
      this->data = new usint[words];
      memset(this->data, 0, this->size * sizeof(usint));
      this->reset();
    }

    WriteBuffer(usint _items, usint item_size) :
      item_bits(item_size),
      items(_items),
      free_buffer(true)
    {
      this->size = bitsToWords(this->items * this->item_bits);
      this->data = new usint[this->size];
      memset(this->data, 0, this->size * sizeof(usint));
      this->reset();
    }

    // This version does not delete the data when deleted.
    WriteBuffer(usint* buffer, usint words) :
      size(words),
      item_bits(1),
      items(0),
      free_buffer(false)
    {
      this->data = buffer;
      this->reset();
    }

    // This version does not delete the data when deleted.
    WriteBuffer(usint* buffer, usint _items, usint item_size) :
      item_bits(item_size),
      items(_items),
      free_buffer(false)
    {
      this->size = bitsToWords(this->items * this->item_bits);
      this->data = buffer;
      this->reset();
    }

    ~WriteBuffer()
    {
      if(this->free_buffer)
      {
        delete[] this->data;
      }
    }

//--------------------------------------------------------------------------

    // This transfers the ownership of the data to the read buffer.
    ReadBuffer* getReadBuffer()
    {
      ReadBuffer* buffer;
      if(this->items > 0)
      {
        buffer = new ReadBuffer(this->data, this->items, this->item_bits);
      }
      else
      {
        buffer = new ReadBuffer(this->data, this->size);
      }

      if(this->free_buffer)
      {
        buffer->claimData();
        this->free_buffer = false;
      }

      return buffer;
    }

    void writeBuffer(std::ofstream& file) const
    {
      file.write((char*)this->data, this->size * sizeof(usint));
    }

    // The buffer will no longer own the data.
    void moveBuffer(usint* buffer)
    {
      if(this->free_buffer)
      {
        delete[] this->data;
      }
      this->free_buffer = false;

      this->data = buffer;
      this->reset();
    }

    usint reportSize() const
    {
      usint bytes = sizeof(*this);
      if(this->free_buffer) { bytes += this->size * sizeof(usint); }
      return bytes;
    }

//--------------------------------------------------------------------------

    inline void reset()
    {
      this->pos = 0;
      this->bits = WORD_BITS;
      this->current = 0;
    }

    inline void skipBits(usint count)
    {
      if(count < this->bits)
      {
        this->bits -= count;
        return;
      }

      count -= this->bits;
      this->pos += 1 + count / WORD_BITS;
      this->bits = WORD_BITS - count % WORD_BITS;
    }

//--------------------------------------------------------------------------

    inline usint bitsLeft() const
    {
      return this->bits + WORD_BITS * (this->size - this->pos - 1);
    }

    inline void writeBits(usint value, usint count)
    {
      if(count >= this->bits)
      {
        count -= this->bits;
        this->data[this->pos] |= GET(LOWER(value, count), this->bits);
        this->pos++; this->bits = WORD_BITS;
      }
      if(count > 0)
      {
        this->bits -= count;
        this->data[this->pos] |= HIGHER(GET(value, count), this->bits);
      }
    }

//--------------------------------------------------------------------------

    /*
      These operations work on fixed-size items.
    */

    inline usint getItemSize() const
    {
      return this->item_bits;
    }

    inline void goToItem(usint item)
    {
      usint b = item * this->item_bits;
      this->pos = b / WORD_BITS;
      this->bits = WORD_BITS - b % WORD_BITS;
      this->current = item;
    }

    inline bool hasNextItem() const
    {
      return (this->current < this->items);
    }

    inline void writeItem(usint item)
    {
      this->writeBits(item, this->item_bits);
      this->current++;
    }

    inline void skipItem()
    {
      this->skipBits(this->item_bits);
      this->current++;
    }

//--------------------------------------------------------------------------

    /*
      Nibble coding for positive integers.
    */

    inline usint nibbleCodeLength(usint value) const
    {
      usint b = 0;
      value--;

      do
      {
        b += 4;
        value >>= 3;
      }
      while(value > 0);

      return b;
    }

    // Something breaks very badly if value > 15.
    inline void writeNibble(usint value)
    {
      this->bits -= 4;
      this->data[this->pos] |= HIGHER(value, this->bits);
      if(this->bits == 0) { this->pos++; this->bits = WORD_BITS; }
    }

    // It is assumed that there is enough space for the code.
    inline void writeNibbleCode(usint value)
    {
      value--;
      while(value > 0x7)
      {
        this->writeNibble(value & 0x7);
        value >>= 3;
      }
      this->writeNibble(value | 0x8);
    }

//--------------------------------------------------------------------------

    /*
      Delta coding for positive integers
    */

    inline bool canDeltaCode(usint value) const
    {
      return this->deltaCodeLength(value) <= this->bitsLeft();
    }

    inline usint deltaCodeLength(usint value) const
    {
      usint len = length(value);
      usint llen = length(len);
      return (len + llen + llen - 2);
    }

    // This version returns false if there is no space left for the encoding.
    inline bool writeDeltaCode(usint value)
    {
      usint len = length(value);
      usint llen = length(len);

      if(len + llen + llen - 2 > this->bitsLeft()) { return false; }

      // this->writeBits(0, llen - 1); // Now included in the next writeBits()
      this->writeBits(len, llen + llen - 1);
      this->writeBits(value, len - 1);
      return true;
    }

    // This version assumes the code fits into the buffer.
    inline void writeDeltaCodeDirect(usint value)
    {
      usint len = length(value);
      usint llen = length(len);

      // this->writeBits(0, llen - 1); // Now included in the next writeBits()
      this->writeBits(len, llen + llen - 1);
      this->writeBits(value, len - 1);
    }

    // We assume the code fits into usint:
    //  32-bit:  value < 2^24
    //  64-bit:  value < 2^54
    inline void writeDeltaCodeFast(usint value)
    {
      usint len = length(value);

      value ^= ((usint)1 << (len - 1));
      this->writeBits((len << (len - 1)) | value, len + 2 * length(len) - 2);
    }

//--------------------------------------------------------------------------

  private:
    usint* data;
    usint size, item_bits, items;
    bool free_buffer;

    // Iterator data
    usint pos, bits, current;

    inline static usint bitsToWords(usint _bits) { return (_bits + WORD_BITS - 1) / WORD_BITS; }

    // These are not allowed.
    WriteBuffer();
    WriteBuffer(const WriteBuffer&);
    WriteBuffer& operator = (const WriteBuffer&);
};


//--------------------------------------------------------------------------


} // namespace TextIndexes


#endif // BITBUFFER_H
