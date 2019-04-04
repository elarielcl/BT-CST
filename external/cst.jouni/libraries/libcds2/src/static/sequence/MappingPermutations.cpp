/* 
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

#include <sequence/MappingPermutations.h>

namespace cds_static
{
	

	MappingPermutations::MappingPermutations():MappingSymbols(){}
	
	MappingPermutations::MappingPermutations(tuple * list, size_t sigma, BitSequenceBuilder *bsbp):MappingSymbols(list,sigma){
		
		uint *permData = new uint[sigma];
		int *runSize = new int[maxLevel+1];
		mappingLeaves = new uint[maxLevel+1];
		for (uint i=0;i<maxLevel+1;i++) runSize[i]=0;
		for (uint i=0;i<sigma;i++){
			permData[i] = list[i].symbol;
			runSize[list[i].len]++;
		}
		uint lastFree=0;
		for (uint i=0;i<=maxLevel;i++){ 
			if (runSize[i]!=0){
				runSize[lastFree]=runSize[i];
				mappingLeaves[lastFree]=i;	
				lastFree++;
			} 
		}
		 
		this->perm = new HuTuckerPermPtrs(runSize, lastFree, permData, sigma,bsbp);
		delete [] permData; 
		delete [] runSize; 
	}

	MappingPermutations::~MappingPermutations() {}

	uint MappingPermutations::getSymbol(uint code){
		uint len = getCodeLen(code);
		return getSymbol(code,len);
	}

	uint MappingPermutations::getSymbol(uint code, uint len){
		return perm->directApplication(nCodesUpToLevel[len-1] + code-minCodePerLevel[len]); 
	}
	
	void MappingPermutations::getCode(uint symbol, uint &code, uint &len, uint &leaf){
		 uint map = perm->inverseApplication(symbol,leaf)+1;
		 len = mappingLeaves[leaf];
		 code = minCodePerLevel[len] + map - nCodesUpToLevel[len-1]-1;
	} 
	 
	 size_t MappingPermutations::getSize(){
		 return perm->size()+(maxLevel+1)*3*(sizeof(uint))+3*sizeof(uint)+sizeof(MappingPermutations); 
	 }

	 void MappingPermutations::save(ofstream & fp){
		uint r = MAPPING_PERM; 
		saveValue(fp,r); 
		
		saveValue(fp, sigma); 
		saveValue(fp, maxLevel); 
		saveValue(fp, firstLevelWithTerminals);
		saveValue(fp,nCodesUpToLevel, maxLevel+1);
		saveValue(fp,minCodePerLevel, maxLevel+1);
		saveValue(fp,mappingLeaves, maxLevel+1);
		perm->save(fp); 

	 }

	 MappingPermutations * MappingPermutations::load(ifstream & fp){
		 uint type = loadValue<uint>(fp);
		if (type!=MAPPING_PERM) return NULL; 
		MappingPermutations *mp = new MappingPermutations(); 
		
		mp->sigma = loadValue<uint>(fp); 
		mp->maxLevel = loadValue<uint>(fp); 
		mp->firstLevelWithTerminals = loadValue<uint>(fp); 
		mp->nCodesUpToLevel = loadValue<uint>(fp, mp->maxLevel+1);
		mp->minCodePerLevel = loadValue<uint>(fp,mp->maxLevel+1);
		mp->mappingLeaves = loadValue<uint>(fp,mp->maxLevel+1);
		mp->perm = PermWT::load(fp); 
	     
	    return mp; 
	 }

};


