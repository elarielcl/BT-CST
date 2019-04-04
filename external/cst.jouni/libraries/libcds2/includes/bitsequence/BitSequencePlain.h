/* static_bitsequence_naive.h
 * Copyright (C) 2012, Alberto Ordóñez, all rights reserved.
 *
 * Naive Bitsequence - don't use, only for testing
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef _BITSEQUENCE_PLAIN_H
#define	_BITSEQUENCE_PLAIN_H

#include <utils/libcdsBasics.h>
#include <bitsequence/BitSequence.h>

/** Class used for testing, should not be used with long bitmaps
 *  @author Gonzalo Navarro for rank operations. Alberto Ordóñez in select
 */
 namespace cds_static
{
class BitSequencePlain: public BitSequence {
public:
	
			class BuilderSequencePlain {
				public:
					bool fastselect1_;
					bool fastselect0_;
					uint sonesamp_;
					uint szerosamp_;
					uint *bs_;
					size_t len_; 
					uint samplingRate;
					
					BuilderSequencePlain fastselect1() {
						fastselect1_ = true;
						// cout << "fastselect1_ = " << fastselect1_ << endl;
						return *this;
					}
					BuilderSequencePlain fastselect0() {
						fastselect0_ = true;
						return *this;
					}
					BuilderSequencePlain s1factor(int val) {
						fastselect1_ = true;
						// cout << "fastselect1_ = " << fastselect1_ << endl;
						sonesamp_ = val;
						return *this;
					}
					BuilderSequencePlain s0factor(int val) {
						fastselect0_ = true;
						// cout << "fastselect1_ = " << fastselect1_ << endl;

						szerosamp_ = val;
						return *this;
					}
					
					static BuilderSequencePlain *createBuilderSequencePlain(uint *bitseq, size_t _len, uint _samplingRate) {
						BuilderSequencePlain *b = new BuilderSequencePlain();
						// cout << "fastselect1_ = " << fastselect1_ << endl;
						b->bs_=bitseq;
						b->len_=_len;
						b->samplingRate=_samplingRate;
						b->fastselect0_=false;
						b->fastselect1_=false;
						return b;
					}
					BitSequencePlain *build() {
						// cout << "fastselect1_ = " << fastselect1_ << endl;
						return new BitSequencePlain(*this);
					}
					protected:
						BuilderSequencePlain(){}

			};
			BitSequencePlain(uint * bitseq, uint length, uint samplingRate);
			BitSequencePlain(const BitString & bs);
			BitSequencePlain(uint * string, uint n, BuilderSequencePlain &b);
			~BitSequencePlain();
			virtual bool access(const size_t i) const;
								 //Nivel 1 bin, nivel 2 sec-pop y nivel 3 sec-bit
			virtual size_t rank1(const size_t i) const;
			virtual size_t rank0(const size_t i) const;
			virtual size_t select0(size_t x) const;
								 // gives the position of the x:th 1.
			virtual size_t select1(size_t x) const;
			virtual size_t getSize() const;
			/** Stores the bitmap given a file pointer*/
			virtual void save(ofstream & fp) const;

			/** Reads a bitmap determining the type */
			static BitSequencePlain * load(ifstream & fp);
			

			//static BuilderSequencePlain builder;
			BitSequencePlain(BuilderSequencePlain &b);

	protected:

BitSequencePlain();
void build(uint * string, uint n, uint samplingSelect);
//borrar cuando termine
	
	 uint factor; 
	 uint integers;
	 uint *data;
	 uint b; 
     uint n;       // # of bits
     uint nb; 
     uint ns; 
     uint numones; 
     uint *sdata;   // superblock counters
     uchar *bdata;
     uint popcount (const uint x) const;

     bool fastSelect0;
     bool fastSelect1;
     uint select1sampling;
     uint select0sampling;
     uint *Stones;
     uint *Stzero;
     size_t select0_s(size_t x) const;
	 size_t select1_s(size_t x) const;

     size_t select0_t(size_t x) const;
	 size_t select1_t(size_t x) const;
};
};
#endif	/* _STATIC_BITSEQUENCE_plain_H */

