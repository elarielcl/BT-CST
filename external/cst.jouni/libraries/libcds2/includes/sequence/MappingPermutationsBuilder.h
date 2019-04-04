/*  SequenceBuilderWaveletTreeNoptrs.h
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

#include <utils/libcdsBasics.h>
#include <sequence/MappingBuilder.h>
#include <bitsequence/BitSequenceBuilder.h>

#ifndef MAPPING_PERMUTATIONS_BUILDER_H
#define MAPPING_PERMUTATIONS_BUILDER_H

namespace cds_static
{
	class MappingPermutationsBuilder : public MappingBuilder
	{
		public:
			MappingPermutationsBuilder(BitSequenceBuilder *bsb);
			virtual ~MappingPermutationsBuilder();
			virtual  MappingSymbols * build(tuple * list, size_t sigma);
			
		protected: 
			BitSequenceBuilder *bsb; 
	};
};
#endif
