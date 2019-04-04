/*  SequenceBuilderWaveletMatrixHuff.cpp
 * Copyright (C) 2010, Francisco Claude, all rights reserved.
 *
 * Francisco Claude <fclaude@cs.uwaterloo.ca>
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

#include <sequence/SequenceBuilderWaveletMatrixHuff.h>

namespace cds_static
{

	SequenceBuilderWaveletMatrixHuff::SequenceBuilderWaveletMatrixHuff(wt_coder_huff *coder, BitSequenceBuilder * bsb, Mapper * am, MappingWMBuilder *wmb) {
		this->bsb = bsb;
		this->am = am;
		this->wc = coder;
		this->wmb = wmb;
		bsb->use();
		am->use();
		wc->use();
	}

	SequenceBuilderWaveletMatrixHuff::~SequenceBuilderWaveletMatrixHuff() {
		bsb->unuse();
		am->unuse();
		wc->unuse();
	}

	Sequence * SequenceBuilderWaveletMatrixHuff::build(uint * sequence, size_t len) {
		return new WaveletMatrixHuff(sequence, len, wc, bsb, am, wmb);
	}

	Sequence * SequenceBuilderWaveletMatrixHuff::build(const Array & seq) {
		return new WaveletMatrixHuff(seq,wc,bsb, am);
	}
};
