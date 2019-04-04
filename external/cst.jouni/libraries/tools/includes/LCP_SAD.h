/* Copyright (C) 2010, Rodrigo Cánovas, all rights reserved.
 *
 *This library is free software; you can redistribute it and/or
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

#ifndef _LCP_SAD_H
#define	_LCP_SAD_H

#include <bitsequence/BitSequence.h>
#include <iostream>
#include <utils/libcdsTrees.h>
#include <LCP_RLCSA.h>
#include <TextIndex.h>


using namespace TextIndexes;

namespace cds_static {

    class LCP_SAD: public LCP_RLCSA {
            private:
                    LCP_SAD();
                    size_t U_type;

            public:
                    size_t lcp_type;
                    BitSequence *U;
                    size_t U_length;  //length of bitmap U

                    /*Create Sadakane encoding of the LCP, that use 2n+o(n) bits,
                     * @param csa The compressed suffix array of the text
                     * @param text The text
                     * @param n  The length of the text
                     * */
                    LCP_SAD(TextIndex *csa, char *text, size_t n, size_t op_rs = BRW32_HDR);
                    LCP_SAD(RLCSA *csa, char *text, size_t n, size_t op_rs = BRW32_HDR);

                    LCP_SAD(TextIndex *csa, uint *lcp_verb, char *text, size_t n, size_t op_rs = BRW32_HDR);
                    LCP_SAD(RLCSA *csa, uint *lcp_verb, char *text, size_t n, size_t op_rs = BRW32_HDR);
                    /*Create the LCP representation starting from a precomputed LCP array*/
//                    LCP_SAD(LCP_RLCSA *lcp, TextIndex *csa, size_t n, size_t op_rs = BRW32_HDR);
//                    LCP_SAD(LCP_RLCSA *lcp, RLCSA *csa, size_t n, size_t op_rs = BRW32_HDR);
                    /**Return LCP[i]
                    * Important: LCP[i] corresponds to the i+1 leaf of the suffix tree!!
                    * */
                    virtual size_t get_LCP(size_t i, TextIndex *csa) const;
                    virtual size_t get_LCP(size_t i, size_t sa_i) const;

                    /**Return LCP[i], being faster if sequential acceses to the LCP had been done*/
                    virtual size_t get_seq_LCP(size_t i, TextIndex *csa, size_t **next_pos, size_t *n_next, bool dir) const;

                    /** Returns the size of the structure in bytes */
                    virtual  size_t getSize() const;

                    /** Stores the structure given a file pointer*/
                    virtual void save(ofstream & fp) const;

                    /** Reads a LCP determining the type */
                    static LCP_SAD * load(ifstream & fp);

                    virtual ~LCP_SAD();
		    
		    //for testing
			 virtual int lcp_access()const{return 0;};
	
    };

};
#endif	/* _LCP_SAD_H */



