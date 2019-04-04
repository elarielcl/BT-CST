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

#ifndef _BASICS_H
#define _BASICS_H

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
	#ifndef uchar
		#define uchar unsigned char
	#endif
	#ifndef ull
		#define ull unsigned long long
	#endif
	/** mask for obtaining the first 5 bits */
	const uint mask31 = 0x0000001F;

	/** number of bits in a uint */
	const uint W = 32;
	/** W-1 */
	const uint Wminusone = 31;

	/** 2W*/
	const uint WW = 64;

	/** number of bits per uchar */
	const uint bitsM = 8;

	/** number of bytes per uint */
	const uint BW = 4;

	/** number of different uchar values 0..255 */
	const uint size_uchar = 256;

	/** popcount array for uchars */
	const unsigned char __popcount_tab[] = {
		0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
	};
	
	const unsigned char popc[] =
		{
		0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
		1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
		1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
		1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
		3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8,
		};
	const unsigned char invertByte[] = 
		{
			0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 
			112, 240, 8, 136, 72, 200, 40, 168, 104, 232, 24, 152, 88, 
			216, 56, 184, 120, 248, 4, 132, 68, 196, 36, 164, 100, 228, 
			20, 148, 84, 212, 52, 180, 116, 244, 12, 140, 76, 204, 44, 
			172, 108, 236, 28, 156, 92, 220, 60, 188, 124, 252, 2, 130, 
			66, 194, 34, 162, 98, 226, 18, 146, 82, 210, 50, 178, 114, 
			242, 10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 
			58, 186, 122, 250, 6, 134, 70, 198, 38, 166, 102, 230, 22, 150, 
			86, 214, 54, 182, 118, 246, 14, 142, 78, 206, 46, 174, 110, 238, 
			30, 158, 94, 222, 62, 190, 126, 254, 1, 129, 65, 193, 33, 161, 97, 
			225, 17, 145, 81, 209, 49, 177, 113, 241, 9, 137, 73, 201, 41, 169, 
			105, 233, 25, 153, 89, 217, 57, 185, 121, 249, 5, 133, 69, 197, 37, 
			165, 101, 229, 21, 149, 85, 213, 53, 181, 117, 245, 13, 141, 
			77, 205, 45, 173, 109, 237, 29, 157, 93, 221, 61, 189, 125, 
			253, 3, 131, 67, 195, 35, 163, 99, 227, 19, 147, 83, 211, 51, 
			179, 115, 243, 11, 139, 75, 203, 43, 171, 107, 235, 27, 155, 
			91, 219, 59, 187, 123, 251, 7, 135, 71, 199, 39, 167, 103, 231, 
			23, 151, 87, 215, 55, 183, 119, 247, 15, 143, 79, 207, 47,
			 175, 111, 239, 31, 159, 95, 223, 63, 191, 127, 255,0,
		};
	
	/** select array for uchars */
	const unsigned char select_tab[] = {
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
	const unsigned char prev_tab[] = {
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
	inline uint bits(uint n) {
		uint b = 0;
		while (n) { b++; n >>= 1; }
		return b;
	}
	inline uint bits64(unsigned long long n) {
		uint b = 0;
		while (n) { b++; n >>= 1; }
		return b;
	}
	/** reads bit p from e */
//	#define bitget(e,p)
    inline uint bitget(uint *e, size_t p){
        return ((((e)[(p)/W] >> ((p)%W))) & 1);
    }
    inline uint bitget(ulong *e, size_t p){
        return ((((e)[(p)/WW] >> ((p)%WW))) & 1);
    }

	/**Included in the libcdsBasics of Trees**/
	#define bitget1(e,p) ((e)[(p)/W] & (1<<((p)%W)))
	/** sets bit p in e */
	inline void bitset(uint * e, size_t p) {
		e[p/W] |= (1<<(p%W));
	}
    inline void bitset(ulong * e, size_t p) {
        e[p/WW] |= (1<<(p%WW));
    }

	/** cleans bit p in e */
	inline void bitclean(uint * e, size_t p) {
		e[p/W] &= ~(1<<(p%W));
	}

    /** cleans bit p in e */
    inline void bitclean(ulong * e, size_t p) {
        e[p/WW] &= ~(1<<(p%WW));
    }
	/** uints required to represent n integers of e bits each */
	inline uint uint_len(const uint e, const size_t n) {
								 //+((unsigned long long)e*n%W>0));
		return ((unsigned long long)e*n+W-1)/W;
	}

	/** Retrieve a given index from array A where every value uses len bits
	 * @param A Array
	 * @param len Length in bits of each field
	 * @param index Position to be retrieved
	 */
	inline uint get_field(const uint *A, const size_t len, const size_t index) {
		if(len==0) return 0;
		size_t i=index*len/W, j=index*len-W*i;
		uint result;
		if (j+len <= W)
			result = (A[i] << (W-j-len)) >> (W-len);
		else {
			result = A[i] >> j;
			result = result | (A[i+1] << (WW-j-len)) >> (W-len);
		}
		return result;
	}

	/** Store a given value in index into array A where every value uses len bits
	 * @param A Array
	 * @param len Length in bits of each field
	 * @param index Position to store in
	 * @param x Value to be stored
	 */
	inline void set_field(uint *A, const size_t len, const size_t index, const uint x) {
		if(len==0) return;
		size_t i=index*len/W, j=index*len-i*W;
		uint mask = ((j+len) < W ? ~0u << (j+len) : 0)
			| ((W-j) < W ? ~0u >> (W-j) : 0);
		A[i] = (A[i] & mask) | x << j;
		if (j+len>W) {
			mask = ((~0u) << (len+j-W));
			A[i+1] = (A[i+1] & mask)| x >> (W-j);
		}
	}

	/** Retrieve a given bitsequence from array A
	 * @param A Array
	 * @param ini Starting position
	 * @param fin Retrieve until end-1
	 */
	inline uint get_var_field(const uint *A, const size_t ini, const size_t fin) {
		if(ini==fin+1) return 0;
		size_t i=ini/W, j=ini-W*i;
		uint result;
		uint len = (uint)(fin-ini+1);
		if (j+len <= W)
			result = (A[i] << (W-j-len)) >> (W-len);
		else {
			result = A[i] >> j;
			result = result | (A[i+1] << (WW-j-len)) >> (W-len);
		}
		return result;
	}

	/** Stores a given bitsequence into array A
	 * @param A Array
	 * @param ini Starting position
	 * @param fin Store until end-1
	 * @param x Value to be stored
	 */
	inline void set_var_field(uint *A, const size_t ini, const size_t fin, const uint x) {
		if(ini==fin+1) return;
		uint i=ini/W, j=ini-i*W;
		uint len = (fin-ini+1);
		uint mask = ((j+len) < W ? ~0u << (j+len) : 0)
			| ((W-j) < W ? ~0u >> (W-j) : 0);
		A[i] = (A[i] & mask) | x << j;
		if (j+len>W) {
			mask = ((~0u) << (len+j-W));
			A[i+1] = (A[i+1] & mask)| x >> (W-j);
		}
	}

	/** Retrieve a given index from array A where every value uses 4 bits
	 * @param A Array
	 * @param index Position to be retrieved
	 */
	inline uint get_field4(const uint *A, const size_t index) {
		size_t i=index/8, j=(index&0x7)<<2;
		return (A[i] << (28-j)) >> (28);
	}

	/** Counts the number of 1s in x */
	inline uint popcount(const int x) {
		return __popcount_tab[(x >>  0) & 0xff]  + __popcount_tab[(x >>  8) & 0xff]
			+ __popcount_tab[(x >> 16) & 0xff] + __popcount_tab[(x >> 24) & 0xff];
	}

	/** Counts the number of 1s in the first 16 bits of x */
	inline uint popcount16(const int x) {
		return __popcount_tab[x & 0xff]  + __popcount_tab[(x >>  8) & 0xff];
	}

	/** Counts the number of 1s in the first 8 bits of x */
	inline uint popcount8(const int x) {
		return __popcount_tab[x & 0xff];
	}

	inline uint invertWord(uint w){
			return        ((invertByte[w&0xff] 	  		<< 24) & 0xff000000) 
					| ((invertByte[(w>>8)  & 0xff]  << 16) & 0x00ff0000) 
					| ((invertByte[(w>>16)  & 0xff]  <<  8) & 0x0000ff00) 
					| ((invertByte[(w>>24)  & 0xff]       ) & 0x000000ff);
	}
	
	/********** some definition for bp_gonzalo ***********/
// returns e[p..p+len-1], assuming len <= W
#define bitsW 5 // OJO
typedef unsigned char byte;

inline uint bitget_go(uint *e, uint p, uint len){ 
	uint answ;
	e += p >> bitsW; p &= (1<<bitsW)-1;
	answ = *e >> p;
	if (len == W)
	{ 
		if(p) 
			answ |= (*(e+1)) << (W-p);
	}
	else{ 
		if (p+len > W) 
			answ |= (*(e+1)) << (W-p);
		answ &= (1<<len)-1;
	}
	return answ;
}
// writes e[p..p+len-1] = s, len <= W
inline void bitput (register uint *e, register uint p, register uint len, register uint s){ 
	e += p >> bitsW; 
	p &= (1<<bitsW)-1;
	if (len == W){ 
		*e |= (*e & ((1<<p)-1)) | (s << p);
		if (!p) 
			return;
		e++;
		*e = (*e & ~((1<<p)-1)) | (s >> (W-p));
	}
	else{ 
		if (p+len <= W){ 
			*e = (*e & ~(((1<<len)-1)<<p)) | (s << p);
			return;
		}
		*e = (*e & ((1<<p)-1)) | (s << p);
		e++; 
		len -= W-p;
		*e = (*e & ~((1<<len)-1)) | (s >> (W-p));
	}
}

inline int createEmptyBitmap(uint **bmp, ulong nBits){
	
	(*bmp) = new uint[uint_len(nBits,1)];
	if (!(*bmp)) return 1;
	for (uint i=0;i<uint_len(nBits,1);i++)
		(*bmp)[i]=0;
	return 0;
}

inline int clearBitmap(uint *bmp, uint nbits){
	uint slots = uint_len(nbits,1);
	for (uint i=0;i<slots;i++)
		bmp[i]=0;
	return 0;
}
/****************************************************/

};

#include <utils/cppUtils.h>
#endif							 /* _BASICS_H */
