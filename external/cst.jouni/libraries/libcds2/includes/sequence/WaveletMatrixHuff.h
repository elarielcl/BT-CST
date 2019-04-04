/* WaveletMatrixHuff.h
 * Copyright (C) 2012, Alberto Ordóñez, all rights reserved.
 *
 * Huffman Shaped Wavelet Matrix
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

#ifndef _WVTREE_MATRIX_HUFF_H
#define _WVTREE_MATRIX_HUFF_H

#include <iostream>
#include <cassert>
#include <utils/libcdsBasics.h>
#include <bitsequence/BitSequence.h>
#include <bitsequence/BitSequenceBuilder.h>
#include <sequence/Sequence.h>
#include <mapper/Mapper.h>
#include <sequence/MappingWM.h>
#include <sequence/MappingWMBuilder.h>
using namespace std;

namespace cds_static
{

	class WaveletMatrixHuff : public Sequence
	{
		public:

			/** Builds a Wavelet Tree for the string
			 * pointed by symbols assuming its length
			 * equals n and uses bmb to build the bitsequence
			 * @param bmb builder for the bitmaps in each level.
			 * @param am alphabet mapper (we need all symbols to be used).
			 * */
			WaveletMatrixHuff(uint * symbols, size_t n,  wt_coder_huff * coder, BitSequenceBuilder * bmb, Mapper * am, MappingWMBuilder *wmb, bool deleteSymbols = false);
			WaveletMatrixHuff(uint * symbols, size_t n,  wt_coder_huff * coder, BitSequenceBuilder ** bmb, uint nbmb, Mapper * am, MappingWMBuilder *wmb, bool deleteSymbols = false);
			WaveletMatrixHuff(const Array &symbols2,  wt_coder_huff * coder, BitSequenceBuilder * bmb, Mapper * am);

			/** Destroys the Wavelet Tree */
			virtual ~WaveletMatrixHuff();

			virtual size_t rank(uint symbol, size_t pos) const;
			virtual size_t select(uint symbol, size_t j) const;

			virtual uint access(size_t pos) const;
			virtual size_t getSize() const;

			virtual void save(ofstream & fp) const;
			static WaveletMatrixHuff * load(ifstream & fp);

		protected:
			WaveletMatrixHuff();
			
			Mapper * am;

			BitSequence **bitstring;

			/** Length of the string. */
			size_t n;

			/** Height of the Wavelet Tree. */
			uint minLength, height, max_v;
			uint *C, *OCC;
			
			MappingWM *huffModel;
			//~ MappingWM *m2;
			//~ MappingWM *m2c;
			//~ MappingWM *mblocks;
			/** Obtains the maximum value from the string
			 * symbols of length n */
			uint max_value(uint *symbols, size_t n);

			/** How many bits are needed to represent val */
			uint bits(uint val);

			/** Returns true if val has its ind-th bit set
			 * to one. */
			bool is_set(uint val, uint ind) const;

			/** Sets the ind-th bit in val */
			uint set(uint val, uint ind) const;

			void buildBitmaps(BitSequenceBuilder **, uint nbsb, uint **_bm, uint *bmpLengths);
			/** Recursive function for building the Wavelet Tree. */
			void build_level(uint **bm, uint *symbols, uint length, uint sigma, uint *occs, uint *lengths, uint *codes, uint *codeLen, uint *lastCode);
	};
};
#endif
