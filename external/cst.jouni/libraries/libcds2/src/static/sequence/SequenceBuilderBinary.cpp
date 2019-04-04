/*  SequenceBuilerBinary.cpp
 * Copyright (C) 2014, Alberto Ordóñez, all rights reserved.
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <sequence/SequenceBuilderBinary.h>

namespace cds_static
{

    SequenceBuilderBinary::SequenceBuilderBinary(BitSequenceBuilder *bsb) {
        this->bsb = bsb;
        this->bsb->use();
    }

    SequenceBuilderBinary::~SequenceBuilderBinary() {
        this->bsb->unuse();
    }

    Sequence * SequenceBuilderBinary::build(uint * sequence, size_t len) {
        return new SequenceBinary(sequence,len,bsb);
    }

    Sequence * SequenceBuilderBinary::build(const Array & seq) {
        return new SequenceBinary(seq,bsb);
    }
};
