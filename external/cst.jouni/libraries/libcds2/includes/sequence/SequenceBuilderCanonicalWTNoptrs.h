/* static_sequence_builder_wvtree_noptrs.h
 * Copyright (C) 2012, Alberto Ordóñez, all rights reserved.
 *
 * Sequence builder wavelet tree without pointers
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

#ifndef _SEQUENCE_BUILDER_CANONICAL_WT_NOPTRS_H
#define _SEQUENCE_BUILDER_CANONICAL_WT_NOPTRS_H

#include <utils/libcdsBasics.h>
#include <sequence/wt_coder.h>
#include <mapper/Mapper.h>
#include <sequence/CanonicalWTNoptrs.h>
#include <sequence/SequenceBuilder.h>
#include <bitsequence/BitSequenceBuilder.h>
#include <sequence/MappingBuilder.h>

namespace cds_static{
	
class SequenceBuilderCanonicalWTNoptrs : public SequenceBuilder {
  public:
    
    SequenceBuilderCanonicalWTNoptrs(BitSequenceBuilder *bmb, Mapper * am, MappingBuilder *msb);
    //~ SequenceBuilderCanonicalWTNoptrs(BitSequenceBuilder *bmb, Mapper * am, MappingBuilder *msb, wt_coder *wc);
    virtual ~SequenceBuilderCanonicalWTNoptrs() {}
    virtual Sequence * build(uint * seq, uint len);
    virtual Sequence * build(uint * seq, uint len, wt_coder_huff *coder);
    //~ virtual static_sequence * build(uint * seq, uint len, static_bitsequence_builder *bsb);
    
  protected:
	wt_coder * wc; 
    Mapper * am;
    BitSequenceBuilder *bmb;
    MappingBuilder *msb; 
};
};

#endif
