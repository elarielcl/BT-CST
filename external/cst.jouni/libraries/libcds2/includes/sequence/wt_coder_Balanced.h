/* wt_coder_Balanced.h
 * Copyright (C) 2014-current_year, Alberto Ordóñez, all rights reserved.
 *
 * Given a canonical set of codes, builds a balanced code such that no symbol is 
   larger than a given limit l_max. Of course, l_max should be at least \lceil log nsymbols \rceil.

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

#ifndef wt_coder_Balanced_h
#define wt_coder_Balanced_h

#include <utils/libcdsBasics.h>
#include <coders/HuffmanCoder.h>
#include <sequence/wt_coder_huff_can.h>
#include <mapper/Mapper.h>
#include <sequence/Tuple.h>
#include <sequence/MappingSymbols.h>
#include <sequence/MappingBuilder.h>

namespace cds_static
{

	
    /** Uses huffman codes to determine the shape of the wavelet tree
     *
     *  @author Alberto Ordóñez
     */
    class wt_coder_Balanced: public wt_coder_huff_can
    {
        public:
            /** Buils a wt_coder_Balanced using the sequence of length n and the alphabet_mapper
             *  to determine the huffman codes */
            wt_coder_Balanced(const Array & a, Mapper *am);
            wt_coder_Balanced(uint *symbs, size_t n, Mapper * am);
            wt_coder_Balanced(wt_coder_huff *wc, uint max_v, uint L, MappingBuilder *mb);
            wt_coder_Balanced(wt_coder_huff *wc, uint max_v, uint L, BitSequenceBuilder *);
            wt_coder_Balanced(wt_coder_huff *wc, uint max_v, float L);
            wt_coder_Balanced(uchar *symbs, size_t n, Mapper * am);
            virtual ~wt_coder_Balanced();
            virtual uint getCode(uint symbol, uint &code) const; 
            virtual uint getSymbol(uint code) const;
            virtual uint getSymbol(uint code, uint len) const;
            virtual bool is_set(uint symbol, uint l) const;
            virtual bool is_set(uint *symbol, uint l) const;
            virtual bool done(uint symbol, uint l) const;
            virtual size_t getSize() const;
			virtual uint * get_symbol(uint symbol) const;
           virtual void save(ofstream & fp) const;
            static wt_coder_Balanced * load(ifstream & fp);
            
           	friend class CanonicalWTNoptrs;

            //uint * get_buffer(uint symbol, uint *n);
			//~ uint get_max_code_length() {return maxCodeLength;}
			//~ uint get_min_code_length() {return minCodeLength;}
			//~ uint *get_ncodes() {return nCodesAtLevel;}
        protected:
            MappingSymbols *model;
            wt_coder_Balanced();
			void build(wt_coder_huff *wc, uint max_v, uint L);
            uint recAsignCodes(uint *posSymbolsRebalance, uint n);
            bool testCodeDecode() const;
            uint nCodesUnderLevel(uint level);
            uint getLeftMostCodeLength(uint node, uint depth);

    };
};

#endif
