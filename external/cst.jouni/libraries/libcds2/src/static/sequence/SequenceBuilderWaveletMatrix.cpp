/*  SequenceBuilderWaveletMatrix.cpp
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

#include <sequence/SequenceBuilderWaveletMatrix.h>
#include <stdexcept>

namespace cds_static
{

	SequenceBuilderWaveletMatrix::SequenceBuilderWaveletMatrix(BitSequenceBuilder * _bsb, Mapper * _am) {
		this->nbsb = 1;
		this->bsb = new BitSequenceBuilder*[this->nbsb];
		this->bsb[0] = _bsb;
		this->am = _am;
		this->bsb[0]->use();
		_am->use();
	}

	SequenceBuilderWaveletMatrix::SequenceBuilderWaveletMatrix(BitSequenceBuilder ** _bsb, uint _nbsb, Mapper * _am) {
		this->nbsb = _nbsb;
		this->bsb = new BitSequenceBuilder*[this->nbsb];
		for (uint i=0;i<this->nbsb;i++){
			this->bsb[i] = _bsb[i];
			this->bsb[i]->use();
		}
		this->am = _am;
		_am->use();
	}

	SequenceBuilderWaveletMatrix::~SequenceBuilderWaveletMatrix() {
        for (uint i=0;i<this->nbsb;i++)
            delete this->bsb[i];
        delete [] this->bsb;
		am->unuse();
	}

	Sequence * SequenceBuilderWaveletMatrix::build(uint * sequence, size_t len) {
		if (this->nbsb==1){
			return new WaveletMatrix(sequence,len, bsb[0], am);
		}else{
			return new WaveletMatrix(sequence, len, bsb, nbsb, am);
		}
		
	}

	Sequence * SequenceBuilderWaveletMatrix::build(const Array & seq) {
		if (this->nbsb==1){
			return new WaveletMatrix(seq, bsb[0], am);
		}else{
			throw runtime_error("Building a WaveletMatrix using several bitmaps per level (>1) is only implemented when the input sequence is an array of uints");
		}
	}
};
