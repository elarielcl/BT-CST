/*  MappingBuilder.h
 * Copyright (C) 2012, Alberto Ordóñez, all rights reserved.
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



#ifndef MAPPING_WM_BUILDER_H
#define MAPPING_WM_BUILDER_H


#include <utils/libcdsBasics.h>
#include <sequence/MappingWM.h>

using namespace cds_utils;

namespace cds_static
{

	class MappingWMBuilder
	{
		public:
			MappingWMBuilder() { userCount=0; }
			virtual ~MappingWMBuilder() {}
			virtual  MappingWM * build(uint sigma, uint minLevel, uint maxLevel, uint *symb2code, uint *symbSortByCode, uint *lastCode, uint *codes, uint *nCodesAtLevel)=0;
			virtual void use() { userCount++; }
			virtual void unuse() { userCount--; assert(userCount>=0); if(userCount==0) delete this; }

		protected:
			int userCount;
	};
};
#include <sequence/MappingWMBSBuilder.h>
#include <sequence/MappingWMBSCBuilder.h>
#include <sequence/MappingWMBlocksBuilder.h>

#endif
