/* BitSequenceBRW32.h
   Copyright (C) 2005, Rodrigo Gonzalez, all rights reserved.
   Adapted to libcds by Alberto Ordóñez.

   New RANK, SELECT, SELECT-NEXT and SPARSE RANK implementations.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef _STATIC_BITSEQUENCE_BRW32_H
#define _STATIC_BITSEQUENCE_BRW32_H

#include <utils/libcdsBasics.h>
#include <bitsequence/BitSequence.h>
#include <iostream>
/////////////
//Rank(B,i)// 
/////////////
//_factor = 0  => s=W*lgn
//_factor = P  => s=W*P
//Is interesting to notice
//factor=2 => overhead 50%
//factor=3 => overhead 33%
//factor=4 => overhead 25%
//factor=20=> overhead 5%

/** Implementation of Rodrigo Gonzalez et al. practical rank/select solution [1]. 
 *  The interface was adapted.
 *  
 *  [1] Rodrigo Gonzalez, Szymon Grabowski, Veli Makinen, and Gonzalo Navarro.
 *      Practical Implementation of Rank and Select Queries. WEA05.
 *
 *  @author Rodrigo Gonzalez
 */
 
using namespace std;
using namespace cds_utils;

namespace cds_static{
	
class BitSequenceBRW32 : public BitSequence {
private:
  //bool owner;
	uint n,integers;
  uint factor,b,s;
  uint *Rs; //superblock array

	uint BuildRankSub(uint ini,uint fin); //uso interno para contruir el indice rank
	void BuildRank(); //crea indice para rank
  BitSequenceBRW32();
  
public:
	uint *data;
	BitSequenceBRW32(uint *bitarray, uint n, uint factor);
	~BitSequenceBRW32(); //destructor

	virtual size_t rank0(const size_t i) const;

	virtual size_t rank1(const size_t i) const;

	virtual size_t select0(const size_t i) const;

	virtual size_t select1(const size_t i) const;

	virtual bool access(const size_t i) const;

	virtual bool access(const size_t i, size_t &r) const;

	virtual size_t getSize() const;

	virtual void save(ofstream & fp) const;

	/** Reads the bitmap from a file pointer, returns NULL in case of error */
	static BitSequence * load(ifstream & fp);

  uint prev(uint start); // gives the largest index i<=start such that IsBitSet(i)=true
  uint prev2(uint start); // gives the largest index i<=start such that IsBitSet(i)=true
  uint next(uint start); // gives the smallest index i>=start such that IsBitSet(i)=true
  uint SpaceRequirementInBits() const;
  uint SpaceRequirement() const;
    
};
};
#endif
