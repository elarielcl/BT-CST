/* WaveletTreeNoptrs.h
 * Copyright (C) 2008, Francisco Claude.
 * Copyright (C) 2011, Matthias Petri.
 *
 * WaveletTreeNoptrs definition
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

#ifndef _WVTREE_NOPTRS_HYBRID_H
#define _WVTREE_NOPTRS_HYBRID_H

#include <iostream>
#include <cassert>
#include <utils/libcdsBasics.h>
#include <bitsequence/BitSequence.h>
#include <bitsequence/BitSequenceBuilder.h>
#include <sequence/Sequence.h>
#include <mapper/Mapper.h>
#include <set>

using namespace std;

namespace cds_static
{

	class WaveletTreeNoptrsHybrid : public Sequence
	{
		public:

			/** Builds a Wavelet Tree for the string stored in a.
			 * @param bmb builder for the bitmaps in each level.
			 * @param am alphabet mapper (we need all symbols to be used).
			 * */
			//~ WaveletTreeNoptrsHybrid(const Array & a, BitSequenceBuilder * bmb, Sequencebuilder * sb, Mapper * am);
			/** Builds a Wavelet Tree for the string
			 * pointed by symbols assuming its length, choosing for each level the best 
			 * bitmap representation among the nbmb given.
			 * equals n and uses bmb to build the bitsequence
			 * @param bmb builders for the bitmaps in each level.
			 * @param nbmb number of bitmap builders
			 * @param am alphabet mapper (we need all symbols to be used).
			 * */
			WaveletTreeNoptrsHybrid(uint * symbols, size_t n, BitSequenceBuilder ** bmb, uint nbmb, Mapper * am, bool deleteSymbols = false);

			//
			/** Builds a Wavelet Tree for the string
			 * pointed by symbols is an array of elements of "width" bits and length
			 * n.
			 * @param bmb builder for the bitmaps in each level.
			 * @param am alphabet mapper (we need all symbols to be used).
			 * */
			//~ WaveletTreeNoptrsHybrid(uint * symbols, size_t n, uint width, BitSequenceBuilder * bmb, Mapper * am, bool deleteSymbols = false);

			/** Destroys the Wavelet Tree */
			virtual ~WaveletTreeNoptrsHybrid();

			virtual size_t rank(uint symbol, size_t pos) const;
			virtual size_t select(uint symbol, size_t j) const;
			virtual uint access(size_t pos) const;
			virtual uint access(size_t pos, size_t &r) const;
			virtual size_t getSize() const;

			size_t range(int i1, int i2, int j1, int j2, int leftb, int rightb, int symb, int level, vector<pair<int,int> > *res, bool addRes);
			virtual void range(int i1, int i2, int j1, int j2, vector<pair<int,int> > *res);
			virtual size_t rangeCount(size_t xs, size_t xe, uint ys, uint ye);
			
			int trackUp(int i, int leftb, int level);

			/* find the q-th smallest element in T[l..r] */
			virtual uint quantile(size_t left,size_t right,uint q);

			/* find the q-th smallest element in T[l..r] and return the freq */
			pair<uint32_t,size_t> quantile_freq(size_t left,size_t right,uint q);

			virtual size_t count(uint symbol) const;
			virtual void save(ofstream & fp) const;
			static WaveletTreeNoptrsHybrid * load(ifstream & fp);

		protected:
			WaveletTreeNoptrsHybrid();

			Mapper * am;
			/** Only one bit-string for the Wavelet Tree. */
			BitSequence **bitstring;
			uint *OCC;

			/** Length of the string. */
			size_t n;

			/** Height of the Wavelet Tree. */
			uint height, max_v;

			/** Obtains the maximum value from the string
			 * symbols of length n */
			uint max_value(uint * symbols, size_t n);
			uint max_value(uint * symbols, unsigned width, size_t n);

			/** How many bits are needed to represent val */
			uint bits(uint val);

			/** Returns true if val has its ind-th bit set
			 * to one. */
			bool is_set(uint val, uint ind) const;

			/** Recursive function for building the Wavelet Tree. */
			void build_level(uint **bm, uint *symbols, uint level, uint length, uint offset);
			void build_level(uint **bm, uint *symbols, unsigned width, uint level, uint length, uint offset);
	};
};
#endif
