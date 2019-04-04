/* Sequence.h
 * Copyright (C) 2012, Alberto Ordóñez, all rights reserved.
 *
 * Sequence definition
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

#ifndef _MAPPING_WTMBSC_H
#define _MAPPING_WTMBSC_H

#include <sequence/MappingWM.h>

namespace cds_static
{
	
class MappingWMBSC : public MappingWM{

		public:
			
			MappingWMBSC(uint sigma, uint minLevel, uint maxLevel, uint *symb2code, uint *symbSortByCode, uint *lastCode, uint *codes, uint *nCodesAtLevel, uint *bitsLevel);
	
			virtual ~MappingWMBSC();

			virtual uint getSymbol(uint code, uint len) ; 
			
			virtual void getCode(uint symbol, uint &code, uint &len) ; 
			
			virtual bool finishCode(uint code, uint len) ;
			
			virtual size_t getSize(); 
			
			virtual void save(ofstream & fp);

			static MappingWMBSC * load(ifstream & fp);

		protected:
			MappingWMBSC();
			uint minBitsCode;
			uint *lastCode;
			/**Used with the bitmap**/
			uint *code2symbol;
			/**Given a symbol returns its code. It takes O(sigma * maxLevel) but now O(sigma * W)**/
			uint *symbol2code;
			uint **codes;
			uint *nCodesAtLevel;
			uint *posFirstCode;
			uint *bitsLevel;
			uint binarySearch(uint *values, uint level, uint len, uint code) const;
	};

};



#endif						
