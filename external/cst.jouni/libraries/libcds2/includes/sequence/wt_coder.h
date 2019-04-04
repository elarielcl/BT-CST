/* wt_coder.h
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * wt_coder definition
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

#ifndef wt_coder_h
#define wt_coder_h

#include <utils/libcdsBasics.h>
#include <fstream>
#include <stdexcept>
 
using namespace std;

namespace cds_static
{

    #define WT_CODER_HUFF_HDR 2
    #define WT_CODER_BINARY_HDR 3
    #define WT_CODER_HUFF_CAN_HDR 4
    #define WT_CODER_SUBOPTIMAL_ADD 5
    #define WT_CODER_BALANCED 6
    #define WT_CODER_PMIN 7
    #define WT_CODER_SUBOPTIMAL 8
    #define WT_CODER_HUTUCKER 9

    class wt_coder_binary;
    class wt_coder_huff;
	class wt_coder_huff_can;
	class wt_coder_SubOptimalAdd;
    /** Coder that defines the shape of a wavelet tree
     *
     *  @author Francisco Claude
     */
    class wt_coder
    {
        public:
            wt_coder();
            virtual void use();
            virtual void unuse();
            virtual ~wt_coder() {};
            virtual uint getCode(uint symbol, uint &code) const
            {
				return -1;
			}
            
			virtual uint getSymbol(uint code) const
            {
                throw runtime_error("getSymbol from code not implemented by default");
            }

            //code contains the most significant bit in the rightmost position
            //returns the number of symbols it decods (n_symbol) storing them in
            //symbols
            virtual uint getSymbols(uint code, uint &n_symbol, vector<uint> &symbols) const{
                throw runtime_error("getSymbols not implemented by default");
            }
            virtual uint getSymbol(uint code, uint len) const{
                throw runtime_error("getSymbol not implemented by default");
            }
            /** Tells if at level l the symbol is represented by a one or a zero */
            virtual bool is_set(uint symbol, uint l) const=0;
            virtual bool is_set(uint *symbol, uint l) const=0;
            virtual bool is_set(uint *symbol,uint l, uint length) const{
				return is_set(symbol,l);
			}
            /** Tells if the path of symbol becomes unique at level l */
            virtual bool done(uint symbol, uint l) const =0;
			virtual uint * get_symbol(uint symbol) const = 0;
            /** Returns the size of the coder */
            virtual size_t getSize() const = 0;
            /** Returns the depth of the tree */
            virtual uint depth() const
            {
                return -1;       // Implemented in wt_coder_binary
            }
            virtual float avgCodeLength() const;
            /** Saves the coder to a file, returns 0 in case of success */
            virtual void save(ofstream & fp) const = 0;
            /** Loads a coder from a file, returns NULL in case of error */
            static wt_coder * load(ifstream & fp);
        protected:
            uint user_count;
            uint sigma;
            //sum_i=0 ^{sigma-1}n_i/n*length(code_i)
            double pli;
    };

};
#include <sequence/wt_coder_huff_can.h>
#include <sequence/wt_coder_huff.h>
#include <sequence/wt_coder_binary.h>
#endif
