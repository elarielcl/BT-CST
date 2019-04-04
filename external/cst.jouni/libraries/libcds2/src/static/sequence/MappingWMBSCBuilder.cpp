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

#include <sequence/MappingWMBSCBuilder.h>

namespace cds_static
{

	MappingWMBSCBuilder::MappingWMBSCBuilder(uint *bitsLevel) {this->bitsLevel = bitsLevel;}

	MappingWMBSCBuilder::~MappingWMBSCBuilder() {}

	MappingWM * MappingWMBSCBuilder::build(uint sigma, uint minLevel, uint maxLevel, uint *symb2code, uint *symbSortByCode, uint *lastCode, uint *codes, uint *nCodesAtLevel) {
		return new MappingWMBSC(sigma,minLevel,maxLevel,symb2code,symbSortByCode,lastCode,codes,nCodesAtLevel,bitsLevel);
	}

};
