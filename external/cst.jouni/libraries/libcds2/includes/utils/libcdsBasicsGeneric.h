/* basics.h
 * Copyright (C) 2005, Rodrigo Gonzalez, all rights reserved.
 *
 * Some preliminary stuff
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

#ifndef _LIBCDS_BASICS_64_H_
#define _LIBCDS_BASICS_64_H_

#include <sys/types.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <sstream>
#include <cassert>
#include <stdint.h>

namespace cds_utils
{

    using namespace std;

    class LibcdsBasicsGeneric {
    public:

        /** number of bits in a ulong */
        static constexpr size_t W = sizeof(size_t)*8;

        /** W-1 */
        static constexpr size_t Wminusone = sizeof(size_t)*8-1;

        /** 2W*/
        static constexpr size_t WW = W*2;

        static const     size_t ZERO = (size_t)0;
        static const     size_t ONE = (size_t)1;
        static constexpr size_t NOT_ZERO = ~ZERO;

        static inline size_t bits(size_t n) {
            size_t b = (size_t)0;
            while (n) {
                b++;
                n >>= 1;
            }
            return b;
        }

        /** reads bit p from e */
        static inline size_t bitget(size_t *e, size_t p){
            return (e[p/W] >> (p%W)) & ONE;
        }

        /**Included in the libcdsBasics of Trees**/
#define bitget1(e, p) ((e)[(p)/W] & (1<<((p)%W)))

        /** sets bit p in e */
        static inline void bitset(size_t *e, size_t p) {
            e[p / W] |= (ONE << (p % W));
        }

        /** cleans bit p in e */
        static inline void bitclean(size_t *e, size_t p) {
            e[p / W] &= ~(ONE << (p % W));
        }

        /** uints required to represent n integers of e bits each */
        static inline size_t sizetLen(const size_t e, const size_t n) {
            //+((unsigned long long)e*n%W>0));
            return ((size_t) e * n + W - 1) / W;
        }

        /** Retrieve a given index from array A where every value uses len bits
    * @param A Array
    * @param len Length in bits of each field
    * @param index Position to be retrieved
    */
        static inline size_t get_field(const size_t *A, const size_t len, const size_t index) {
            if(len==0) return 0;
            size_t i = index * len / W, j = index * len - W * i;
            size_t result;
            if (j + len <= W)
                result = (A[i] << (W - j - len)) >> (W - len);
            else {
                result = A[i] >> j;
                result = result | (A[i + 1] << (WW - j - len)) >> (W - len);
            }
            return result;
        }

        /** Store a given value in index into array A where every value uses len bits
    * @param A Array
    * @param len Length in bits of each field
    * @param index Position to store in
    * @param x Value to be stored
    */
    static inline void set_field(size_t *A, const size_t len, const size_t index, const size_t x) {
        if(len==0) return;
            size_t i = index * len / W, j = index * len - i * W;
            size_t mask = ((j + len) < W ? NOT_ZERO << (j + len) : ZERO)
                    | ((W - j) < W ? NOT_ZERO >> (W - j) : ZERO);
            A[i] = (A[i] & mask) | x << j;
            if (j + len > W) {
                mask = ((NOT_ZERO) << (len + j - W));
                A[i + 1] = (A[i + 1] & mask) | x >> (W - j);
            }
        }

        /** Retrieve a given bitsequence from array A
    * @param A Array
    * @param ini Starting position
    * @param fin Retrieve until end-1
    */
    inline size_t get_var_field(const size_t *A, const size_t ini, const size_t fin) {
        if(ini== fin+1) return 0;
        size_t i=ini/ W, j=ini-W*i;
        size_t result;
        size_t len = (fin-ini+ 1);
        if (j+len <= W)
            result =(A[i] << (W - j-len)) >> (W -len);
        else {
            result = A[i] >> j;
            result = result | (A[i +1] << (WW - j -len)) >> (W -len);
        }
        return result;
    }

        /** Stores a given bitsequence into array A
    * @param A Array
    * @param ini Starting position
    * @param fin Store until end-1
    * @param x Value to be stored
    */
        static inline void set_var_field(size_t *A, const size_t ini, const size_t fin, const size_t x) {
        if(ini== fin+1) return;
        size_t i=ini/W, j=ini - i*W;
        size_t len = (fin-ini + 1);
        size_t mask = ((j+len) < W ? NOT_ZERO << (j+len) : 0)
                | ((W-j) < W ? NOT_ZERO>> (W-j) : ZERO);
        A[i] = (A[i] & mask) | x << j;
        if (j+len>W) {
            mask = ((NOT_ZERO) << (len+j-W));
            A[i+1] = (A[i+1] &mask)| x >> (W-j);
        }
    }

    static inline size_t invertWord(size_t x){

        x = ((x & 0x5555555555555555ULL) << 1) | ((x & 0xAAAAAAAAAAAAAAAAULL) >> 1);
        x = ((x & 0x3333333333333333ULL) << 2) | ((x & 0xCCCCCCCCCCCCCCCCULL) >> 2);
        x = ((x & 0x0F0F0F0F0F0F0F0FULL) << 4) | ((x & 0xF0F0F0F0F0F0F0F0ULL) >> 4);
        x = ((x & 0x00FF00FF00FF00FFULL) << 8) | ((x & 0xFF00FF00FF00FF00ULL) >> 8);
        x = ((x & 0x0000FFFF0000FFFFULL) <<16) | ((x & 0xFFFF0000FFFF0000ULL) >>16);
        x = ((x & 0x00000000FFFFFFFFULL) <<32) | ((x & 0xFFFFFFFF00000000ULL) >>32);
        return x;
    }
    static inline uint invertWord(uint x){

        x = ((x & 0x55555555U) << 1) | ((x & 0xAAAAAAAAU) >> 1);
        x = ((x & 0x33333333U) << 2) | ((x & 0xCCCCCCCCU) >> 2);
        x = ((x & 0x0F0F0F0FU) << 4) | ((x & 0xF0F0F0F0U) >> 4);
        x = ((x & 0x00FF00FFU) << 8) | ((x & 0xFF00FF00U) >> 8);
        x = ((x & 0x0000FFFFU) <<16) | ((x & 0xFFFF0000U) >>16);
        return x;
    }

    };
};

#include <utils/cppUtils.h>
#endif							 /* _BASICS_H */
