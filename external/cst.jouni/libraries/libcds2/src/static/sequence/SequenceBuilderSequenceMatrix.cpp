/*  SequenceBuilderSequenceMatrix.cpp
 * Copyright (C) 2014, Alberto Ordonez, all rights reserved.
 *
 * Alberto Ordonez <alberto.ordonez@udc.es>
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

#include <sequence/SequenceBuilderSequenceMatrix.h>

namespace cds_static
{

	SequenceBuilderSequenceMatrix::SequenceBuilderSequenceMatrix(BitSequenceBuilder * bsb, Mapper * am) {
		this->bsb = bsb;
		this->am = am;
		bsb->use();
		am->use();
	}
	
	SequenceBuilderSequenceMatrix::~SequenceBuilderSequenceMatrix() {
		if (bsb) bsb->unuse();
		am->unuse();
	}

	Sequence * SequenceBuilderSequenceMatrix::build(uint * sequence, size_t len) {
		return new SequenceMatrix(sequence, len, bsb, am);
	}

	Sequence * SequenceBuilderSequenceMatrix::build(const Array & seq) {
		return new SequenceMatrix(seq, bsb, am);
	}
};
