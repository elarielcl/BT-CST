/*  SequenceBuilderWaveletTreeNoptrs.cpp
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

#include <sequence/SequenceBuilderWaveletTreeNoptrs.h>
#include <stdexcept>

namespace cds_static
{

	SequenceBuilderWaveletTreeNoptrs::SequenceBuilderWaveletTreeNoptrs(BitSequenceBuilder * _bsb, Mapper * _am) {
		this->nbsb = 1;
		this->bsb = new BitSequenceBuilder*[this->nbsb];
		this->bsb[0] = _bsb;
		this->am = _am;
		bsb[0]->use();
		am->use();
	}
	
	SequenceBuilderWaveletTreeNoptrs::SequenceBuilderWaveletTreeNoptrs(BitSequenceBuilder ** _bsb, uint _nbsb, Mapper * _am) {
		this->nbsb = _nbsb;
		this->bsb = new BitSequenceBuilder*[this->nbsb];
		for (uint i=0;i<this->nbsb;i++){
			this->bsb[i] = _bsb[i];
			this->bsb[i]->use();
		}
		this->am = _am;
		am->use();
	}

	SequenceBuilderWaveletTreeNoptrs::~SequenceBuilderWaveletTreeNoptrs() {
		for (uint i=0;i<nbsb;i++)
			bsb[i]->unuse();
		am->unuse();
	}

	Sequence * SequenceBuilderWaveletTreeNoptrs::build(uint * sequence, size_t len) {
		if (nbsb==1){
			return new WaveletTreeNoptrs(sequence, len, bsb[0], am);
		}else{
			return new WaveletTreeNoptrs(sequence, len, bsb, nbsb, am);
		}
	}

	Sequence * SequenceBuilderWaveletTreeNoptrs::build(const Array & seq) {
		if (nbsb==1){
			return new WaveletTreeNoptrs(seq, bsb[0], am);
		}else{
			throw runtime_error("Building a WaveletMatrix using several bitmaps per level (>1) is only implemented when the input sequence is an array of uints");
		}
	}
};
