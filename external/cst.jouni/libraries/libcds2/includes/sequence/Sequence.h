/* Sequence.h
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * Sequence definition
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

#ifndef _SEQUENCE_H
#define _SEQUENCE_H

#include <utils/cppUtils.h>
#include <utils/libcdsBasics.h>

using namespace cds_utils;
using namespace std;

namespace cds_static
{

	#define BS_HDR 1
	#define WVTREE_HDR 2
	#define WVTREE_NOPTRS_HDR 3
	#define GMR_HDR 4
	#define GMR_CHUNK_HDR 5
	#define WVMATRIX_HDR 7
	#define WVTREE_NOPTRSS_HDR 8
	#define CANONICALWT_NOPTRS_HDR 9
	#define WVMATRIX_HUFF_HDR 10
	#define SEQUENCE_WT_NOPTRS_HYBRID 15
	#define SEQUENCE_MATRIX_HDR 16
	#define SEQUENCE_LZEND_HDR 18
	#define SEQUENCE_BINARY_HDR 20
	#define SEQUENCE_MULTI_ARY_WT_HDR 21
	
	/** Sequence base class
	 *
	 *  @author Francisco Claude
	 */
	class Sequence
	{

		public:
			/** Builds and sets the length */
			Sequence(size_t length);
			virtual ~Sequence() {}

			/** Count the number of occurrences of c up to position i included
			 */
			virtual size_t rank(uint c, size_t i) const;

			/** return value is rank of symbol (less or equal to the given symbol) that has rank > 0, 
                the parameter symbol is updated accordinly (Only implemented in WaveletTree)*/
			virtual uint rankLessThan(uint &symbol, uint pos) const;
			/** Retrieve the i-th position where c appears.
			 * It returns (size_t)-1 if i=0 and length if the number of
			 * c's is less than i
			 */
			virtual size_t select(uint c, size_t i) const;

			/** Retrieves the next occurrence of a c starting at position i (included).
			 * It return length if there are no occurrences of c after position i.
			 */
			virtual size_t selectNext(uint c, size_t i) const;
			/**
			* Access operator
			*/
			virtual uint operator[](size_t i) const;
			/** Retrieve the symbol at position i.
			 */
			virtual uint access(size_t i) const;

			/** Retrieve the symbol at position i and its rank.
			 */
			virtual uint access(size_t i, size_t & r) const;

			/** Size of the structure in bytes.
			 */
			virtual size_t getSize() const = 0;

			/** Length of the sequence.
			 */
			virtual size_t getLength() const { return length; }

			/** Saves the structure to the stream
			 */
			virtual void save(ofstream & fp) const = 0;
			
			virtual size_t rangeCount(size_t xs, size_t xe, uint ys, uint ye) const;

			virtual void range(int i1, int i2, int j1, int j2, vector<pair<int,int> > *res);
			/** Reads a bitmap determining the type */
			static Sequence * load(ifstream & fp);

		protected:
			size_t length;
			uint sigma;

	};

};

#include <sequence/WaveletTree.h>
#include <sequence/WaveletTreeNoptrs.h>
#include <sequence/WaveletMatrixHuff.h>
#include <sequence/WaveletMatrix.h>
#include <sequence/SequenceGMR.h>
#include <sequence/SequenceGMRChunk.h>
#include <sequence/WaveletTreeNoptrsS.h>
#include <sequence/CanonicalWTNoptrs.h>
#include <sequence/SequenceBuilder.h>
#include <sequence/WaveletTreeNoptrsHybrid.h>
#include <sequence/SequenceMatrix.h>
//#include <sequence/SequenceLZEnd.h>
//#include <sequence/SequenceLZBlock.h>
#include <sequence/SequenceBinary.h>
#include <sequence/MultiArityWT.h>

#endif							 /* _SEQUENCE_H */
