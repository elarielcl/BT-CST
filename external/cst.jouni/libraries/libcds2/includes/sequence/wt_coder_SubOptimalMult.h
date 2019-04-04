/* wt_coder_SubOptimalMult.h
 * Copyright (C) 2014-current_year, Alberto Ordóñez, all rights reserved.
 *
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

#ifndef _WT_CODER_SUBOPTIMAL_MULT_H
#define _WT_CODER_SUBOPTIMAL_MULT_H

#include <utils/libcdsBasics.h>
#include <coders/HuffmanCoder.h>
#include <sequence/wt_coder_huff_can.h>
#include <mapper/Mapper.h>
#include <sequence/Tuple.h>
#include <sequence/MappingSymbols.h>
#include <utils/HashTable.h>
#include <utils/Array.h>

namespace cds_static
{

	
    /** Uses huffman codes to determine the shape of the wavelet tree
     *
     *  @author Alberto Ordóñez
     */
    class wt_coder_SubOptimalMult: public wt_coder
    {
        public:

            wt_coder_SubOptimalMult(wt_coder_huff_can *wc, uint max_v, float c, uint lmax);
            wt_coder_SubOptimalMult(uchar *symbs, size_t n, Mapper * am);
            virtual ~wt_coder_SubOptimalMult();
            virtual uint getCode(uint symbol, uint &code) const; 
            virtual uint getSymbol(uint code, uint len) const;
            virtual uint getSymbols(uint code, uint &n_symbol, vector<uint> &symbols) const;
            virtual uint getSymbol(uint code) const;
            virtual bool is_set(uint symbol, uint l) const;
            virtual bool is_set(uint *symbol, uint l) const;
            virtual bool done(uint symbol, uint l) const;
            virtual size_t getSize() const;
			virtual uint * get_symbol(uint symbol) const;
            virtual void save(ofstream & fp) const;
            static wt_coder_SubOptimalMult * load(ifstream & fp);
    
        protected:
            wt_coder_SubOptimalMult();
            //number of symbols
            uint nsymbols;

            //tells the last level (inclusive) that contains frequenteSymbols;
            uint levelFreq;

            HashTable<int> *hashFreq;
            //inverse hash
            Array *ihash;
            //number of codes at each frequent level
            uint *sR;
            //first code of each frequent level
            uint *fst;
            //last freq code +1 and filled with x 0's until len(baseCodeUnFreq)=lmax+1.
            uint baseCodeUnFreq;

            int firstLevelWithCodes;

            uint *nCodes;

            //length of the unfrequent codes
            uint lmax;
            //tests
            bool testHashFreqSymbols(wt_coder_huff_can *wc, uint max_v, uint nShort);
            bool testEncode(wt_coder_huff_can *wc);
            bool testDecode(wt_coder_huff_can *wc);
            uint fillHash(wt_coder_huff_can *wc, uint max_v);
            uint buildUnfreq(wt_coder_huff_can *wc, uint max_v);


    };
};

#endif
