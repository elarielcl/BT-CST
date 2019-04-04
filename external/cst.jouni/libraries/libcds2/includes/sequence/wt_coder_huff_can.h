/* wt_coder_huff_can.h
 * Copyright (C) 2014-current_year, Alberto Ordóñez, all rights reserved.
 *
 * Builds a cannonincal Huffman set of codes
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

#ifndef wt_coder_huff_can_h
#define wt_coder_huff_can_h

#include <utils/libcdsBasics.h>
#include <sequence/wt_coder.h>
#include <coders/HuffmanCoder.h>
#include <mapper/Mapper.h>
#include <sequence/Tuple.h>
#include <sequence/MappingSymbols.h>
namespace cds_static
{

	class wt_coder_SubOptimalAdd;
    
    /** Uses huffman codes to determine the shape of the wavelet tree
     *
     *  @author Alberto Ordóñez
     */
    class wt_coder_huff_can: public wt_coder
    {
        public:
            /** Buils a wt_coder_huff_can using the sequence of length n and the alphabet_mapper
             *  to determine the huffman codes */
            wt_coder_huff_can(const Array & a, Mapper *am);
            wt_coder_huff_can(uint *symbs, size_t n, Mapper * am);
            wt_coder_huff_can(wt_coder_huff *wc, uint max_v);
            wt_coder_huff_can(uchar *symbs, size_t n, Mapper * am);
            virtual ~wt_coder_huff_can();
            virtual uint getCode(uint symbol, uint &code) const; 
            virtual bool is_set(uint symbol, uint l) const;
            virtual bool is_set(uint *symbol, uint l) const;
            virtual bool done(uint symbol, uint l) const;
            virtual size_t getSize() const;
			virtual uint * get_symbol(uint symbol) const;
            virtual void save(ofstream & fp) const;
           
            static wt_coder_huff_can * load(ifstream & fp);
            tuple *getCopyTableSymbols();
           	friend class CanonicalWTNoptrs;
            friend class wt_coder_SubOptimalMult;

            //uint * get_buffer(uint symbol, uint *n);
			//~ uint get_max_code_length() {return maxCodeLength;}
			//~ uint get_min_code_length() {return minCodeLength;}
			//~ uint *get_ncodes() {return nCodesAtLevel;}
        protected:
            wt_coder_huff_can();
           
            uint *firstCodeAtLevel;
            uint *symbol2code; 
            uint table_len;
            uint maxCodeLength;
			uint minCodeLength;
			uint *nCodesAtLevel;
			tuple *table_symbols_codes;
			void buildCanonical(uint max_v, uint minCode, uint maxCode);
			void computeAvgCodeLength(tuple *table, uint nsymbols, float &avgCode);
            bool testCanonicalSorted();
};
};

#include <sequence/wt_coder_SubOptimalAdd.h>
#include <sequence/wt_coder_Suboptimal.h>
#include <sequence/wt_coder_Balanced.h>
#include <sequence/wt_coder_Pmin.h>
#endif
