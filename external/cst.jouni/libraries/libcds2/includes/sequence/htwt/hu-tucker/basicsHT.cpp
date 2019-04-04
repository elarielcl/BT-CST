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
#ifndef _BASICHT_H_
#define _BASICHT_H_

#include <basicsHT.h>

    /** popcount array for uchars */
    const unsigned char __popcount_tab_F[] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
    };

    /** select array for uchars */
    const unsigned char select_tab_F[] = {
        0, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
        6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
        7, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
        6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
        8, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
        6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
        7, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
        6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
    };

    /** prev array for uchars */
    const unsigned char prev_tab_F[] = {
        0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    };


    /** bits needed to represent a number between 0 and n */
     uint bits_F(uint n) {
        uint b = 0;
        while (n) { b++; n >>= 1; }
        return b;
    }


    /** sets bit p in e */
     void bitset_F(uint * e, size_t p) {
        e[p/W_F] |= (1<<(p%W_F));
    }

    /** cleans bit p in e */
     void bitclean_F(uint * e, size_t p) {
        e[p/W_F] &= ~(1<<(p%W_F));
    }

    /** uints required to represent n integers of e bits each */
     uint uint_len_F(const uint e, const size_t n) {
        return ((unsigned long)e*n+W_F-1)/W_F; //+((unsigned long long)e*n%W>0));
    }

    /** Retrieve a given index from array A where every value uses len bits
     * @param A Array
     * @param len Length in bits of each field
     * @param index Position to be retrieved
     */
     uint get_field_basic(const uint *A, const size_t len, const size_t index) {
        if(len==0) return 0;
        size_t i=index*len/W_F, j=index*len-W_F*i;
        uint result;
        if (j+len <= W_F)
            result = (A[i] << (W_F-j-len)) >> (W_F-len);
        else {
            result = A[i] >> j;
            result = result | (A[i+1] << (WW_F-j-len)) >> (W_F-len);
        }
        return result;
    }

    /** Store a given value in index into array A where every value uses len bits
     * @param A Array
     * @param len Length in bits of each field
     * @param index Position to store in
     * @param x Value to be stored
     */
     void set_field_F(uint *A, const size_t len, const size_t index, const uint x) {
        if(len==0) return;
        size_t i=index*len/W_F, j=index*len-i*W_F;
        uint mask = ((j+len) < W_F ? ~0u << (j+len) : 0)
            | ((W_F-j) < W_F ? ~0u >> (W_F-j) : 0);
        A[i] = (A[i] & mask) | x << j;
        if (j+len>W_F) {
            mask = ((~0u) << (len+j-W_F));
            A[i+1] = (A[i+1] & mask)| x >> (W_F-j);
        }
    }

    /** Retrieve a given bitsequence from array A
     * @param A Array
     * @param ini Starting position
     * @param fin Retrieve until end-1
     */
     uint get_var_field_F(const uint *A, const size_t ini, const size_t fin) {
        if(ini==fin+1) return 0;
        size_t i=ini/W_F, j=ini-W_F*i;
        uint result;
        uint len = (uint)(fin-ini+1);
        if (j+len <= W_F)
            result = (A[i] << (W_F-j-len)) >> (W_F-len);
        else {
            result = A[i] >> j;
            result = result | (A[i+1] << (WW_F-j-len)) >> (W_F-len);
        }
        return result;
    }

    /** Stores a given bitsequence into array A
     * @param A Array
     * @param ini Starting position
     * @param fin Store until end-1
     * @param x Value to be stored
     */
     void set_var_field_F(uint *A, const size_t ini, const size_t fin, const uint x) {
        if(ini==fin+1) return;
        uint i=ini/W_F, j=ini-i*W_F;
        uint len = (fin-ini+1);
        uint mask = ((j+len) < W_F ? ~0u << (j+len) : 0)
            | ((W_F-j) < W_F ? ~0u >> (W_F-j) : 0);
        A[i] = (A[i] & mask) | x << j;
        if (j+len>W_F) {
            mask = ((~0u) << (len+j-W_F));
            A[i+1] = (A[i+1] & mask)| x >> (W_F-j);
        }
    }

    /** Retrieve a given index from array A where every value uses 4 bits
     * @param A Array
     * @param index Position to be retrieved
     */
     uint get_field4_F(const uint *A, const size_t index) {
        size_t i=index/8, j=(index&0x7)<<2;
        return (A[i] << (28-j)) >> (28);
    }

    /** Counts the number of 1s in x */
     uint popcount_F(const int x) {
        return __popcount_tab_F[(x >>  0) & 0xff]  + __popcount_tab_F[(x >>  8) & 0xff]
            + __popcount_tab_F[(x >> 16) & 0xff] + __popcount_tab_F[(x >> 24) & 0xff];
    }

    /** Counts the number of 1s in the first 16 bits of x */
     uint popcount16_F(const int x) {
        return __popcount_tab_F[x & 0xff]  + __popcount_tab_F[(x >>  8) & 0xff];
    }

    /** Counts the number of 1s in the first 8 bits of x */
     uint popcount8_F(const int x) {
        return __popcount_tab_F[x & 0xff];
    }

#endif
