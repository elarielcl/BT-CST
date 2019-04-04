/* SequenceMatrix.h
 * Copyright (C) 2008, Francisco Claude.
 * Copyright (C) 2011, Matthias Petri.
 *
 * SequenceMatrix definition
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

#ifndef _SEQUENCE_MATRIX_H
#define _SEQUENCE_MATRIX_H

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
	/**
		It stores a bitmap of length n for each symbol. The bitmap j is set to 1 at
		position i <=> input[i]==j. 
		Not suitable for large alphabets!!.
	*/

	class SequenceMatrix : public Sequence
	{
		public:

			/** Builds a Matrix for the string stored in a.
			 * @param bmb builder for the bitmaps in each level.
			 * @param am alphabet mapper (we need all symbols to be used).
			 * */
			SequenceMatrix(const Array & a, BitSequenceBuilder * bmb, Mapper * am);
			/** Builds a Matrix for the string
			 * pointed by symbols assuming its length
			 * equals n and uses bmb to build the bitsequence
			 * @param bmb builder for the bitmaps in each level.
			 * @param am alphabet mapper (we need all symbols to be used).
			 * */
			SequenceMatrix(uint * symbols, size_t n, BitSequenceBuilder * bmb, Mapper * am, bool deleteSymbols = false);
			
			/** Destroys the Wavelet Tree */
			virtual ~SequenceMatrix();

			virtual size_t rank(uint symbol, size_t pos) const;
			virtual size_t select(uint symbol, size_t j) const;
			virtual uint access(size_t pos) const;
			virtual uint access(size_t pos, size_t &r) const;
			virtual size_t getSize() const;

			virtual void save(ofstream & fp) const;
			static SequenceMatrix * load(ifstream & fp);
			
		protected:
			SequenceMatrix();
			void build(uint *data, size_t n, BitSequenceBuilder * bmb, Mapper * am, bool deleteSymbols = false);

			Mapper * am;
			/** Only one bit-string for the Wavelet Tree. */
			BitSequence **bitstring;

			/** Length of the string. */
			size_t n;
			/** Max value of a symbol */
			uint max_v;
			
			bool selfTest();

			
			
	};
};
#endif
