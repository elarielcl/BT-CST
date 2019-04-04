/* Builder for delta encoding bitvectors.h
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * Builder for delta encoding bitvectors definition
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

#ifndef _BITSEQUENCE_BUILDER_DELTA_H
#define _BITSEQUENCE_BUILDER_DELTA_H

#include <utils/libcdsBasics.h>
#include <bitsequence/BitSequenceBuilder.h>
namespace cds_static
{
class BitSequenceBuilderDelta : public BitSequenceBuilder {
  public:
    /** Defines the sample rate used to build the bitmaps (brw32) */
    BitSequenceBuilderDelta(uint sampling) ; 
    virtual ~BitSequenceBuilderDelta() {}
    virtual BitSequence * build(uint * bitsequence, size_t len) const;
    virtual BitSequence * build(const BitString & bs) const;

  protected:
    uint sample_rate;
    BitSequencePlain::BuilderSequencePlain *builder;
};
};
#endif 
