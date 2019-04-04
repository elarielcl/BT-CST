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

#ifndef _MAPPING_SYMBOLS_H
#define _MAPPING_SYMBOLS_H


#include <utils/cppUtils.h>
#include <utils/libcdsBasics.h>
#include <vector>
#include <algorithm>
#include <utils/Tuples.h>
using namespace std;
using namespace cds_utils; 

namespace cds_static
{

	#define MAPPING_PERM 1
	#define MAPPING_TABLE 2
	typedef struct stuple{
		uint symbol;
		uint code;
		uint len;
		uint occs;
	}tuple;
	class MappingSymbols
	{

		public:
		
			MappingSymbols(tuple * list, size_t sigma); 

			virtual ~MappingSymbols(); 

			virtual uint getSymbol(uint code, uint len){return -1;}

			virtual uint getSymbol(uint code){return -1;}

			virtual uint getSymbols(uint code, uint &nSymbols, vector<uint> &symbols);

			virtual void getCode(uint symbol, uint &code, uint &len, uint &leaf){} 
			
			virtual size_t getSize(){return -1;}
			
			virtual void save(ofstream & fp){} 

			static MappingSymbols * load(ifstream & fp);

		protected:
			MappingSymbols();
			//the most significant bit is in the leftmost position in code
			uint getCodeLen(uint code);
			//the first bit of the code (the most significant) is in the rightmost position
			//returns in ret_code the reverted code
			uint getCodeLen(uint code, uint &ret_code);
			uint sigma;
			uint maxLevel; 
			uint firstLevelWithTerminals; 
			uint *nCodesUpToLevel; 
			uint *minCodePerLevel;
            vector<uint> nCodesAtlevel;

	};

};

#include <sequence/MappingTable.h>
#include <sequence/MappingPermutations.h>
#endif						
