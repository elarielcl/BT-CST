//~ #ifndef _LIBCDSBASICSHT_INCLUDED
//~ #define _LIBCDSBASICSHT_INCLUDED

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


#include <sys/types.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>


		//~ typedef unsigned char uchar; 
    	/** mask for obtaining the first 5 bits */
    #define mask31_F = 0x0000001F;

    /** number of bits in a uint */
    #define W_F  32
    /** W-1 */
    #define Wminusone_F  31

    /** 2W*/
    #define WW_F  64

    /** number of bits per uchar */
    #define bitsM_F  8

    /** number of bytes per uint */
    #define BW_F  4

    /** number of different uchar values 0..255 */
    #define size_uchar_F  256


		/** reads bit p from e */
    #define bitget_F(e,p) ((((e)[(p)/W_F] >> ((p)%W_F))) & 1)

    /** bits needed to represent a number between 0 and n */
     uint bits_F(uint n);
	
    /** sets bit p in e */
     void bitset_F(uint * e, size_t p);

    /** cleans bit p in e */
     void bitclean_F(uint * e, size_t p);

    /** uints required to represent n integers of e bits each */
      uint uint_len_F(const uint e, const size_t n);

    /** Retrieve a given index from array A where every value uses len bits
     * @param A Array
     * @param len Length in bits of each field
     * @param index Position to be retrieved
     */
      uint get_field_basic(const uint *A, const size_t len, const size_t index);

    /** Store a given value in index into array A where every value uses len bits
     * @param A Array
     * @param len Length in bits of each field
     * @param index Position to store in
     * @param x Value to be stored
     */
      void set_field_F(uint *A, const size_t len, const size_t index, const uint x);

    /** Retrieve a given bitsequence from array A
     * @param A Array
     * @param ini Starting position
     * @param fin Retrieve until end-1
     */
      uint get_var_field_F(const uint *A, const size_t ini, const size_t fin);

    /** Stores a given bitsequence into array A
     * @param A Array
     * @param ini Starting position
     * @param fin Store until end-1
     * @param x Value to be stored
     */
      void set_var_field_F(uint *A, const size_t ini, const size_t fin, const uint x);

    /** Retrieve a given index from array A where every value uses 4 bits
     * @param A Array
     * @param index Position to be retrieved
     */
      uint get_field4_F(const uint *A, const size_t index);

    /** Counts the number of 1s in x */
      uint popcount_F(const int x);
    /** Counts the number of 1s in the first 16 bits of x */
      uint popcount16_F(const int x);

    /** Counts the number of 1s in the first 8 bits of x */
      uint popcount8_F(const int x);


//~ #endif
