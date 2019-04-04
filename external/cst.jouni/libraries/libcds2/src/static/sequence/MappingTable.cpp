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

#include <sequence/MappingTable.h>
#include <vector>
#include <sequence/MappingAcelerators.h>

namespace cds_static
{
	
	MappingTable::MappingTable():MappingSymbols(){}
	
	MappingTable::MappingTable(tuple * list, size_t sigma): MappingSymbols(list,sigma){
		
		symbol2code = new uint [uint_len(sigma+1,this->maxLevel)];
		for (uint i=0;i<sigma; i++){
			set_field(symbol2code, this->maxLevel,list[i].symbol, list[i].code);
			assert(list[i].len<=this->maxLevel);
			assert(list[i].symbol<=sigma);
		}

		this->minBitsMaxSymbol = bits(sigma+1); 
		code2symbol = new uint[uint_len(sigma+1, this->minBitsMaxSymbol)];
	
		
		for (uint i=0;i<sigma;i++){
			assert(list[i].len<=this->maxLevel);
			assert(nCodesUpToLevel[list[i].len-1]+list[i].code-this->minCodePerLevel[list[i].len]<=sigma);
			set_field(code2symbol,this->minBitsMaxSymbol,nCodesUpToLevel[list[i].len-1]+list[i].code-this->minCodePerLevel[list[i].len], list[i].symbol); 
		}

	}
	
	MappingTable::~MappingTable() {	
		delete [] symbol2code; 
		delete [] code2symbol; 
	}

	uint MappingTable::getSymbol(uint code){
		uint len = getCodeLen(code);
		return getSymbol(code,len);
	}

	 uint MappingTable::getSymbol(uint code, uint len){
		 return get_field(code2symbol, minBitsMaxSymbol, nCodesUpToLevel[len-1] + code-minCodePerLevel[len]);
	 }
	
	 void MappingTable::getCode(uint symbol, uint &code, uint &len, uint &leaf){
		code = get_field(symbol2code,maxLevel,symbol); 
		len = getCodeLen(code);
//         uint len2 = acelerator.getCodeLen(code);
//         assert(len==len2);
//         uint reverted_code = LibcdsBasicsGeneric::invertWord(code);
//         len2 = acelerator_start.getCodeLen(code,reverted_code>> (32-len));
//         if (len!=len2){
//             len = getCodeLen(code);
//             len2 = acelerator_start.getCodeLen(code,reverted_code>> (32-len));
//         }
//         assert(len==len2);
//         uint len3 = acelerator_extended.getCodeLen(code,reverted_code);
//         if (len!=len3){
//             len = getCodeLen(code);
//             len3 = acelerator_extended.getCodeLen(code,reverted_code);
//         }
//         assert(len==len3);
		// for (uint i=firstLevelWithTerminals;i<=maxLevel;i++){
		// 	uint nCodes = nCodesUpToLevel[i]-nCodesUpToLevel[i-1] ;
		// 	if (nCodes==0) continue;
		// 	if (code <(nCodes+ minCodePerLevel[i])){
		// 		len = i; 
		// 		break; 
		// 	}
		// }
	 } 
	 
	  size_t MappingTable::getSize(){
		  //~ cout << "Sigma: " << sigma << ", minBitsMaxSymbol: " << minBitsMaxSymbol << ", maxLevel: " << maxLevel << endl; 
	  	return sizeof(MappingTable)+4*sizeof(uint)+(maxLevel+1)*sizeof(uint)*2+ sizeof(uint) * (uint_len(sigma+1,maxLevel)); 
		  //return sizeof(MappingTable)+4*sizeof(uint)+(maxLevel+1)*sizeof(uint)*2+ sizeof(uint) * (((sigma+1)*maxLevel+31)/32 +uint_len(sigma+1, minBitsMaxSymbol) ); 
	  }

	 void MappingTable::save(ofstream & fp){
		 uint r = MAPPING_TABLE; 
		saveValue(fp,r); 
		
		saveValue(fp, sigma); 
		saveValue(fp, maxLevel); 
		saveValue(fp, firstLevelWithTerminals);
		saveValue(fp,nCodesUpToLevel, maxLevel+1);
		saveValue(fp,minCodePerLevel, maxLevel+1);
		
		saveValue(fp,minBitsMaxSymbol); 
		saveValue(fp,symbol2code,((sigma+1)*maxLevel+31)/32); 
		saveValue(fp,code2symbol,uint_len(sigma+1, minBitsMaxSymbol)); 

	 }

	 MappingTable * MappingTable::load(ifstream & fp){
		
		uint type = loadValue<uint>(fp);
		if (type!=MAPPING_TABLE) return NULL; 
		
		MappingTable *mp = new MappingTable(); 
		
		mp->sigma = loadValue<uint>(fp); 
		mp->maxLevel = loadValue<uint>(fp); 
		mp->firstLevelWithTerminals = loadValue<uint>(fp); 
		mp->nCodesUpToLevel = loadValue<uint>(fp, mp->maxLevel+1);
		mp->minCodePerLevel = loadValue<uint>(fp,mp->maxLevel+1);
		
		mp->minBitsMaxSymbol = loadValue<uint>(fp); 
		mp->symbol2code =loadValue<uint>(fp,((mp->sigma+1)*mp->maxLevel+31)/32); 
		mp->code2symbol =loadValue<uint>(fp,uint_len(mp->sigma, mp->minBitsMaxSymbol)); 

		return mp; 
	 }

	uint MappingTable::invertCode(uint word, uint len){
		//~ cout << "word: " << word; 
		uint newWord = 0;
		for (uint i=0;i<len/2;i++){
			if (bitget(&word,i)) bitset(&newWord,len-i-1); 
			if (bitget(&word,len-i-1)) bitset(&newWord,i); 
		}	
		if (len%2!=0) if (bitget(&word,len/2)) bitset(&newWord,len/2); 
		//~ cout << ", inverted: " << newWord << endl; 
		return newWord; 
	}
	

};


