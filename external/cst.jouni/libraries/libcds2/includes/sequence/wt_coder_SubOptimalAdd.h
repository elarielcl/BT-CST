/* wt_coder_SubOptimalAdd.h
 * Copyright (C) 2014-current_year, Alberto Ordóñez, all rights reserved.
 *
 * Builds a suboptimal code limiting the height of longest code to L. To do 
 * so we follow the idea of Milidiú & Laber [Algorithmica(2001)31: 513-529]. 
 * Besides, the returned set of codes is not optimal but it is cannonical, and 
 * all the symbols at each level of the tree of codes are sorted in increasing order. 
 * Thus, if codeLen(symbol_i)=codeLen(symbol_j), then if (symbol_i<symbol_j) then 
 * code(symbol_i)<code(symbol_j), the opposite otherwise. 
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

#ifndef wt_coder_SubOptimal_Add_h
#define wt_coder_SubOptimal_Add_h

#include <utils/libcdsBasics.h>
#include <coders/HuffmanCoder.h>
#include <sequence/wt_coder_huff_can.h>
#include <mapper/Mapper.h>
#include <sequence/Tuple.h>
#include <sequence/MappingSymbols.h>
#include <sequence/MappingBuilder.h>

namespace cds_static
{

	//~ class wt_coder_huff_can;
    /** Uses huffman codes to determine the shape of the wavelet tree
     *
     *  @author Alberto Ordóñez
     */
    class wt_coder_SubOptimalAdd: public wt_coder_huff_can
    {
        public:
            /** Buils a wt_coder_SubOptimalAdd using the sequence of length n and the alphabet_mapper
             *  to determine the huffman codes */
            wt_coder_SubOptimalAdd(const Array & a, Mapper *am);
            wt_coder_SubOptimalAdd(uint *symbs, size_t n, Mapper * am);
            wt_coder_SubOptimalAdd(wt_coder_huff *wc, uint max_v, uint L, MappingBuilder *mb);
            wt_coder_SubOptimalAdd(wt_coder_huff *wc, uint max_v, uint L, BitSequenceBuilder *);
            wt_coder_SubOptimalAdd(wt_coder_huff *wc, uint max_v, float L);
            wt_coder_SubOptimalAdd(uchar *symbs, size_t n, Mapper * am);
            virtual ~wt_coder_SubOptimalAdd();
            virtual uint getCode(uint symbol, uint &code) const;
            virtual uint getSymbols(uint code, uint &n_symbol, vector<uint> &symbols) const;
            virtual uint getSymbol(uint code) const;
            virtual uint getSymbol(uint code, uint len) const;
            virtual bool is_set(uint symbol, uint l) const;
            virtual bool is_set(uint *symbol, uint l) const;
            virtual bool done(uint symbol, uint l) const;
            virtual size_t getSize() const;
			virtual uint * get_symbol(uint symbol) const;
           virtual void save(ofstream & fp) const;
            static wt_coder_SubOptimalAdd * load(ifstream & fp);
            
           	friend class CanonicalWTNoptrs;

            //uint * get_buffer(uint symbol, uint *n);
			//~ uint get_max_code_length() {return maxCodeLength;}
			//~ uint get_min_code_length() {return minCodeLength;}
			//~ uint *get_ncodes() {return nCodesAtLevel;}
        protected:
            MappingSymbols *model;
            wt_coder_SubOptimalAdd();
            void rebuildBalanced(uint *posSymbolsHoles, uint *posSymbolsRebalance, uint n,uint nholes, uint L);
            void rebuildBalanced2(uint *posSymbolsHoles, uint *posSymbolsRebalance, uint n,uint nholes, uint L);
            uint recAsignCodes(uint *posSymbolsRebalance, uint n);
			void build(wt_coder_huff *wc, uint max_v, uint L);
            bool testCodeDecode() const;

    };
};

#endif
