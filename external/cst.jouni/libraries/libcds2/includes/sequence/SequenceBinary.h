/* SequenceBinary.h
 * Copyright (C) 2014, Alberto Ordóñez, all rights reserved.
 *
 * Encapsulates a Bitmap --> just can handle sigma=2 (0,1)
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

#ifndef _SEQUENCEBINARY_H
#define _SEQUENCEBINARY_H

#include <sequence/Sequence.h>
#include <bitsequence/BitSequence.h>
#include <bitsequence/BitSequenceBuilder.h>

using namespace cds_utils;
using namespace std;

namespace cds_static
{

    class SequenceBinary : public Sequence
    {

    public:

        SequenceBinary(uint * seq, size_t n, BitSequenceBuilder *bsb);
        SequenceBinary(const Array & values, BitSequenceBuilder *bsb);
        SequenceBinary();

        virtual ~SequenceBinary();

        virtual size_t rank(uint c, size_t i) const;

        virtual size_t select(uint c, size_t i) const;

        virtual uint access(size_t i) const;

        virtual size_t getSize() const;

        virtual size_t getLength() const { return length; }

        virtual void save(ofstream & fp) const;

        static SequenceBinary * load(ifstream & fp);

    protected:
        BitSequence *bs;
    };

};

#endif							 /* _SequenceBinaryREP_H */
