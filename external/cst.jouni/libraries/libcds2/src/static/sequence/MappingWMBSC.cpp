/* 
 * Copyright (C) 2012, Alberto Ordóñez, all rights reserved.
 *
 * Huffman mapping used in the Huffman Shaped Wavelet Matrix. 
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

#include <sequence/MappingWMBSC.h>

namespace cds_static
{
	
		MappingWMBSC::MappingWMBSC(uint sigma, uint minLevel, uint maxLevel, uint *symb2code, uint *symbSortByCode, uint *lastCode, uint *codes, uint *nCodesAtLevel, uint *bitsLevel):MappingWM(sigma,minLevel,maxLevel){
			this->sigma = sigma;
			this->minLevel = minLevel; 
			this->maxLevel = maxLevel; 
			this->lastCode = new uint[maxLevel+1];
			for (uint i=0;i<=this->maxLevel;i++){
				this->lastCode[i]=lastCode[i];
			}	
			this->bitsLevel = new uint[maxLevel+1];
			if (bitsLevel==NULL){
				for (uint i=minLevel;i<=this->maxLevel;i++)
					this->bitsLevel[i]=i;
			}else{
				for (uint i=minLevel;i<=this->maxLevel;i++)
					this->bitsLevel[i]=bitsLevel[i];
			}
			
			
			//~ this->symbol2code = new uint[sigma+1];
			this->symbol2code = new uint[uint_len(maxLevel,sigma+1)];

			//rank on bitmap returns values in [1..sigma] --> to avoid substract 1 to 
			//the result of rank to access code2symbol we shift by one positions all its values.
			this->minBitsCode = bits(sigma+1); 
			this->code2symbol =  new uint[uint_len(this->minBitsCode,sigma+2)];
			
			for (uint i=0;i<uint_len(maxLevel,sigma+1);i++)
				this->symbol2code[i]=0;
				

			for (uint i=0;i<uint_len(this->minBitsCode,sigma+2);i++)
				this->code2symbol[i]=0;
				
			
			for (uint i=0;i<=sigma;i++){
				set_field(this->symbol2code, maxLevel, i, symb2code[i]);//[i]=symb2code[i];
				set_field(this->code2symbol, minBitsCode, i+1, symbSortByCode[i]);
			}
			set_field(this->code2symbol, minBitsCode, sigma+1, symbSortByCode[sigma]);
			
			this->codes = new uint*[this->maxLevel+1];
			this->nCodesAtLevel = new uint[this->maxLevel+1];
			this->posFirstCode = new uint[this->maxLevel+2];
			for (uint i=0;i<=this->maxLevel;i++) this->nCodesAtLevel[i]=nCodesAtLevel[i];
			this->posFirstCode[this->minLevel]=1;
			uint p=0;
			for (uint i=this->minLevel;i<=this->maxLevel;i++){
				this->codes[i] = new uint[uint_len(this->bitsLevel[i],this->nCodesAtLevel[i])];
				for (uint j=0;j<uint_len(this->bitsLevel[i],this->nCodesAtLevel[i]);j++) this->codes[i][j]=0;
				for (uint j=0;j<this->nCodesAtLevel[i];j++)
					set_field(this->codes[i], this->bitsLevel[i], j, codes[p++]);//[j]=codes[p++];
				for (uint j=1;j<this->nCodesAtLevel[i];j++)
					assert(get_field(this->codes[i], this->bitsLevel[i], j) > get_field(this->codes[i], this->bitsLevel[i], j-1));
				this->posFirstCode[i+1]=p+1;
			}
			
		}

	MappingWMBSC::MappingWMBSC(){}
	
	MappingWMBSC::~MappingWMBSC() {
		delete [] code2symbol;
		delete [] symbol2code;
		delete [] lastCode;
		for (uint i=0;i<maxLevel;i++){
			delete [] codes[i];
		}
		delete [] codes;
		delete [] nCodesAtLevel;
		delete [] posFirstCode;
		delete [] bitsLevel;
	}

	 uint MappingWMBSC::getSymbol(uint code, uint len) {
		 uint index = this->posFirstCode[len]+binarySearch(this->codes[len],len,this->nCodesAtLevel[len],code);
		 return get_field(this->code2symbol,minBitsCode,index);
		 //~ return this->code2symbol[this->posFirstCode[len]+binarySearch(this->codes[len],this->nCodesAtLevel[len],code)];
	 }
	
	 void MappingWMBSC::getCode(uint symbol, uint &code, uint &len) {
		code = get_field(this->symbol2code, maxLevel, symbol);
		for (uint i=minLevel;i<=maxLevel;i++){
			if (code <= lastCode[i]){
				len=i;
				return;
			}
		}
		len = maxLevel;
	 } 
	  
	bool MappingWMBSC::finishCode(uint code, uint len) {
		return (len>=minLevel) && (code <= lastCode[len]);
	}
	 
	size_t MappingWMBSC::getSize() {
		uint total=0;
		for (uint i=minLevel;i<=maxLevel;i++)
			total+=uint_len(bitsLevel[i],nCodesAtLevel[i])+1;
		total=total*sizeof(uint);
		total+=uint_len(maxLevel,sigma+1)*sizeof(uint);//symb2code
		total+=uint_len(minBitsCode,sigma+2)*sizeof(uint);//codes2symb
		total+=sizeof(uint)*(maxLevel+1);//bitsLevel
		total+=sizeof(uint)*(maxLevel*3+4);
		return total+sizeof(MappingWMBSC);
	}

	 void MappingWMBSC::save(ofstream & fp){
		uint r = MAPPING_WM_BSC; 
		saveValue(fp,r); 
		saveValue(fp, sigma); 
		saveValue(fp, minLevel);
		saveValue(fp, maxLevel);
		 
		saveValue(fp, symbol2code,uint_len(maxLevel,sigma+1));
		saveValue(fp, code2symbol,uint_len(minBitsCode,sigma+2));
		
		saveValue(fp, lastCode,maxLevel+1);
		saveValue(fp, nCodesAtLevel,maxLevel+1);
		saveValue(fp, posFirstCode,maxLevel+2);
		saveValue(fp, bitsLevel,maxLevel+1);
		
		for (uint i=minLevel;i<=maxLevel;i++)
			saveValue(fp, codes[i], uint_len(bitsLevel[i],nCodesAtLevel[i]));

	 }

	 MappingWMBSC * MappingWMBSC::load(ifstream & fp){
		uint type = loadValue<uint>(fp);
		if (type!=MAPPING_WM_BSC) return NULL; 
		
		MappingWMBSC *mp = new MappingWMBSC();
		mp->sigma    = loadValue<uint>(fp); 
		mp->minLevel = loadValue<uint>(fp); 
		mp->maxLevel = loadValue<uint>(fp); 
		mp->minBitsCode = bits(mp->sigma+1);
		
		mp->symbol2code  = loadValue<uint>(fp,uint_len(mp->maxLevel,mp->sigma+1));
		mp->code2symbol  = loadValue<uint>(fp,uint_len(mp->minBitsCode,mp->sigma+2));
		
		mp->lastCode     = loadValue<uint>(fp,mp->maxLevel+1);
		mp->nCodesAtLevel= loadValue<uint>(fp,mp->maxLevel+1);
		mp->posFirstCode = loadValue<uint>(fp,mp->maxLevel+2);
		mp->bitsLevel    = loadValue<uint>(fp,mp->maxLevel+1);
		
		mp->codes = new uint*[mp->maxLevel+1];
		for (uint i=mp->minLevel;i<=mp->maxLevel;i++)
			mp->codes[i]=loadValue<uint>(fp,uint_len(mp->bitsLevel[i],mp->nCodesAtLevel[i]));
			
		return mp;
	 }
	
	uint MappingWMBSC::binarySearch(uint *values, uint level, uint len, uint code) const{
		uint l=0,r=len,p=(l+r)/2;
		while(l<r){
			if (get_field(values,bitsLevel[level],p)<code){
				l=p+1;
			}else{
				r=p;
			}
			p=(l+r)/2;
		}
		assert(get_field(values,bitsLevel[level],p)==code);
		return p;
	}
};


