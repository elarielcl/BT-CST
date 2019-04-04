/* 
 * File:   BitSequenceDelta.h
 * Author: andres
 *
 * Created on September 25, 2011, 10:23 PM
 */


#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <bitsequence/BitSequence.h>

#ifndef BITSEQUENCEDELTA_H
#define	BITSEQUENCEDELTA_H



using namespace cds_utils;

namespace cds_static
{

    class BitSequenceDelta: public BitSequence
    {

    public:
            BitSequenceDelta(unsigned int* array, unsigned int n, unsigned int sampling = 32);
            virtual ~BitSequenceDelta();
            virtual size_t  select1(const size_t total) const;
            /** computes select(total) and stores the next delta*/
            virtual size_t  select2(unsigned int total,unsigned int* delta) const;
            /** returns kind of select_0: it is actually the position of the one to the left of select_0(total) and the number of ones */
            virtual size_t  select0_rank1(unsigned int total,unsigned int* rank) const;
            virtual size_t  rank1(const size_t pos) const;
            virtual bool  access(const size_t pos) const;
            virtual size_t  rank_select(unsigned int pos, unsigned int* select) const;
            virtual size_t  getSize() const;
            virtual void  save(ofstream & fp) const;
            static BitSequenceDelta* load(ifstream & fp);

            /** bits needed to represent a number between 0 and n */
            inline unsigned int bits(unsigned int n){
              unsigned int b = 0;
              while (n) { b++; n >>= 1; }
              return b;
            }
        private:
            unsigned int* deltacodes;
            unsigned int* sampled_pointer;
            unsigned int* sampled_total;
            unsigned int n;
            unsigned int sampling;
            unsigned int total_size;
            unsigned int decodeDelta(unsigned int* deltaCodes, unsigned int* pos) const;
            void encodeDelta(unsigned int val, unsigned int* deltaCodes, unsigned int* pos);
            //unsigned int bits(unsigned int n);
            BitSequenceDelta();
    };

}

#endif	/* BITSEQUENCEDELTA_H */

