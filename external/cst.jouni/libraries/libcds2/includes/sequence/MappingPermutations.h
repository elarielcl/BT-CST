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

#ifndef _MAPPING_PERMUTATIONS_H
#define _MAPPING_PERMUTATIONS_H



#include <sequence/MappingSymbols.h>
#include <sequence/htwt/PermWT.h>
#include <bitsequence/BitSequenceBuilder.h>


using namespace std;

namespace cds_static
{
	
class MappingPermutations : public MappingSymbols{

		public:
			
			MappingPermutations(tuple * list, size_t sigma,BitSequenceBuilder *bsbp); 
					
			virtual ~MappingPermutations();

			virtual uint getSymbol(uint code, uint len); 

			virtual uint getSymbol(uint code);

			virtual void getCode(uint symbol, uint &code, uint &len, uint &leaf);

			virtual size_t getSize(); 
			
			virtual void save(ofstream & fp);

			static MappingPermutations * load(ifstream & fp);

		protected:
		
			MappingPermutations(); 
			PermWT * perm; 
			//there could exist levels with no symbols, but we have only runs for those levels that have symbols. 
			//we need to map those runs to the actual level it corresponds.
			uint *mappingLeaves;


	};

};



#endif						
