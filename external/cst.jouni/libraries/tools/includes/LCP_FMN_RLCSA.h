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

#ifndef _LCP_FMN_RLCSA_H
#define	_LCP_FMN_RLCSA_H

#include <bitsequence/BitSequence.h>
#include <bitsequence/BitSequenceDelta.h>
#include <LCP_RLCSA.h>

using namespace cds_static;
using namespace TextIndexes;

namespace cds_static{

    class LCP_FMN_RLCSA: public LCP_RLCSA{
        private:
                LCP_FMN_RLCSA();
                virtual void generate_OZ(BitSequence *U, uint **O, uint **Z, uint length);
                BitSequence *O, *Z;
                size_t b_length; //length of the bitmaps
                size_t OZ_type;

        public:
                size_t lcp_type; 
                /*Johannes, Veli & Gonzalo LCP encode
                 * @param csa The compressed suffix array of the text
                 * @param text The text
                 * @param n  The length of the text
                 * @param op_rs Choose rank/select implementation that will be used (must be RRR02_HDR, SDARRAY_HDR or DELTA)
                 * */	
		        LCP_FMN_RLCSA(TextIndex *csa, char *text, size_t n, size_t op_rs=SDARRAY_HDR);
                LCP_FMN_RLCSA(RLCSA *csa, char *text, size_t n, size_t op_rs=SDARRAY_HDR);
                LCP_FMN_RLCSA(TextIndex *csa, uint *lcp_verb, char *text, size_t n, size_t op_rs=SDARRAY_HDR);
                LCP_FMN_RLCSA(RLCSA *csa, uint *lcp_verb, char *text, size_t n, size_t op_rs=SDARRAY_HDR);
               // LCP_FMN_RLCSA(LCP *lcp, TextIndex *csa, size_t n, size_t op_rs=RRR02_HDR);

                /**Return LCP[i]*/
                virtual size_t get_LCP(size_t i, TextIndex *csa) const;

                /**Return LCP[i]*/
                virtual size_t get_LCP(size_t i, size_t sa_i) const;

                /**Return LCP[i], being faster if sequential acceses to the LCP had been done*/
                virtual size_t get_seq_LCP(size_t i, TextIndex *csa, size_t **next_pos, size_t *n_next, bool dir) const;

                /** Returns the size of the structure in bytes */
                virtual size_t getSize() const;

                /** Stores the structure given a file pointer, return 0 in case of success */
                virtual void save(ofstream & fp) const;

                /** Reads a LCP determining the type */
                static LCP_FMN_RLCSA * load(ifstream & fp);

                virtual ~LCP_FMN_RLCSA();
		
		//for testing
		//for testing
			 virtual int lcp_access()const;

    };
};

#endif	/* _LCP_FMN_RLCSA_RLCSA_H */
