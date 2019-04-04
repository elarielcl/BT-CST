/* CanonicalWTNoptrs.cpp
 * Copyright (C) 2012, Alberto Ordóñez, all rights reserved.
 *
 *  Implementation of a pointerless huffman (canonical) shaped wavelet tree
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

#ifndef _STATIC_SEQUENCE_CANONICAL_WT_H
#define _STATIC_SEQUENCE_CANONICAL_WT_H

#include <iostream>
#include <cassert>
#include <utils/libcdsBasics.h>
#include <bitsequence/BitSequence.h>
#include <bitsequence/BitSequenceBuilder.h>
#include <sequence/Sequence.h>
#include <mapper/Mapper.h>
#include <sequence/htwt/PermWT.h>
#include <utils/cppUtils.h>
#include <utils/Tuples.h>
#include <sequence/MappingSymbols.h>
#include <sequence/MappingBuilder.h>

using namespace std;
using namespace cds_utils; 

namespace cds_static{
	
class CanonicalWTNoptrs : public Sequence {
	
  public:
  
	CanonicalWTNoptrs(uint * symbols, uint n, wt_coder_huff *wc, BitSequenceBuilder * bmb, Mapper * am, MappingBuilder *msb,  bool deleteSymbols=false); 
	
	CanonicalWTNoptrs(uint * symbols, uint n, wt_coder_huff_can *wc, BitSequenceBuilder * bmb, Mapper * am, MappingBuilder *msb,  bool deleteSymbols=false); 

    CanonicalWTNoptrs(uint * symbols, uint n,  BitSequenceBuilder * bmb, Mapper * am,  MappingBuilder *msb, bool deleteSymbols = false);

    virtual ~CanonicalWTNoptrs();

    virtual size_t rank(uint c, size_t i) const;
    
    virtual size_t select(uint c, size_t j) const;
    
    virtual size_t selectNext(uint c, size_t i) const;
    
    virtual uint access(size_t i) const;
    
    virtual uint access(size_t i, size_t & r) const;
    
	virtual size_t getSize() const ;
	
	virtual void save(ofstream & fp) const ;
	
    static CanonicalWTNoptrs * load(ifstream  & fp) ;
    
    virtual uint rankLessThan(uint i, uint j);

	virtual uint forward(uint pos, uint & value); 
	
  protected:

    CanonicalWTNoptrs();
    
    MappingSymbols *ms; 
    
    BitSequence *occ, *bitmapwt;
	
	BitSequence **bitstring;
	
	//~ PermWT *permVoc;
    /** Length of the string. */
    uint n;
    uint sigma;
	/** Lengths of the shortest/largests codeword*/
	uint maxCodeLength;
	
	/** Greatest codeword at each level (useful (not necessary) for navigation)*/
	uint *maxValuePerLevel;

	/**Number of symbols that finishes at each level. Necessary to manage the pointer to the bitmaps.*/
	uint *finishAtLevel;
	/** Pointers from each level to the bitmapwt. Requires O(log^2(n)) bits*/
	uint *starts;
	
	uint *C, *fstCode, *iniC,*nCodesLevel;
	
	uint firstLevelWithTerminals;
	
    /** Obtains the maximum value from the string
     * symbols of length n */
    uint max_value(uint * symbols, uint n);

    /** Returns true if val has its ind-th bit set
     * to one. */
    bool is_set(uint val, uint ind) const;

	uint recursiveSelect(uint code, uint j, uint start, uint end, int len, uint level) const;
	uint recursiveForward(uint pos, uint code, uint start, uint end, uint &codePos, uint level);
	void build(uint *symbols, uint max_v, wt_coder_huff_can *wc, BitSequenceBuilder * bmb, Mapper * am,  MappingBuilder *msb);

};
};
#endif
