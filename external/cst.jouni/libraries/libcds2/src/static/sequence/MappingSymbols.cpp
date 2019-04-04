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

#include <sequence/MappingSymbols.h>
#include <climits>

namespace cds_static
{
	
	MappingSymbols::MappingSymbols(tuple * list, size_t _sigma){
		
		sigma = _sigma; 
		maxLevel = 0; 
		firstLevelWithTerminals = INT_MAX; 
		for (uint i=0;i<sigma;i++){
			maxLevel = max(maxLevel,list[i].len);
			firstLevelWithTerminals = min(firstLevelWithTerminals,list[i].len);
		}

		nCodesUpToLevel = new uint[this->maxLevel+1];
		for (uint i=0;i<=maxLevel;i++) {
            nCodesUpToLevel[i] = 0;
            nCodesAtlevel.push_back(0);
        }
		for (uint i=0;i<sigma;i++) {
            nCodesAtlevel[list[i].len]++;
            nCodesUpToLevel[list[i].len]++;
        }
		for (uint i=1;i<=maxLevel;i++) 
			nCodesUpToLevel[i]+=nCodesUpToLevel[i-1];

		minCodePerLevel = new uint[maxLevel+1];
		for (uint i=0;i<=maxLevel;i++)
			minCodePerLevel[i]=INT_MAX;
		
		for (uint i=0;i<sigma;i++){	
			if (list[i].code< minCodePerLevel[list[i].len]) 
				minCodePerLevel[list[i].len] = list[i].code;
		}
	}


	uint MappingSymbols::getCodeLen(uint code){
		uint len=-1;
		for (uint i=firstLevelWithTerminals;i<=maxLevel;i++){
			uint nCodes = nCodesUpToLevel[i]-nCodesUpToLevel[i-1] ;
            assert(nCodes==nCodesAtlevel[i]);
			if (nCodes==0) continue;
			if (code <(nCodes+ minCodePerLevel[i])){
				len = i; 
				break; 
			}
		}
		return len;
	}

	uint MappingSymbols::getCodeLen(uint code, uint &ret_code){
		uint len=-1;
		ret_code = invertWord(code)>>(W-firstLevelWithTerminals);
		for (uint i=firstLevelWithTerminals;i<=maxLevel;i++){
			uint nCodes = nCodesUpToLevel[i]-nCodesUpToLevel[i-1];
			if (nCodes>0) {
				if (ret_code < (nCodes + minCodePerLevel[i])) {
					len = i;
					break;
				}
			}
			ret_code = (ret_code<<1)|((code>>i)&1);
		}
		return len;
	}


	uint MappingSymbols::getSymbols(uint code, uint &nSymbols, vector<uint> &symbols) {
		uint rev_code=0;
		uint len   = getCodeLen(code,rev_code);
		symbols[0] = getSymbol(rev_code,len);
		nSymbols   = 1;
		return len;
	}

	MappingSymbols::MappingSymbols(){}
	
	MappingSymbols::~MappingSymbols(){
		delete [] nCodesUpToLevel;
		delete [] minCodePerLevel; 
	}
	
	MappingSymbols * MappingSymbols::load(ifstream & fp){
		
		uint type = loadValue<uint>(fp);
		size_t pos = fp.tellg();
		fp.seekg(pos-sizeof(uint),ios::beg);
		switch(type) {
			case MAPPING_PERM: return MappingPermutations::load(fp);
			case MAPPING_TABLE: return MappingTable::load(fp);
		}
		return NULL;
	}


	};



					
