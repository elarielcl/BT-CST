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

#include <sequence/MappingWMBS.h>

namespace cds_static
{
	
		MappingWMBS::MappingWMBS(uint sigma, uint minLevel, uint maxLevel, uint *symb2code, uint *symbSortByCode, uint *lastCode, uint *codes, uint *nCodesAtLevel):MappingWM(sigma,minLevel,maxLevel){
			this->sigma = sigma;
			this->minLevel = minLevel; 
			this->maxLevel = maxLevel; 
			this->lastCode = new uint[maxLevel+1];
			for (uint i=0;i<=this->maxLevel;i++){
				this->lastCode[i]=lastCode[i];
			}	
			
			
			this->symbol2code = new uint[sigma+1];
			//rank on bitmap returns values in [1..sigma] --> to avoid substract 1 to 
			//the result of rank to access code2symbol we shift by one positions all its values. 
			this->code2symbol = new uint[sigma+2];
			for (uint i=0;i<=sigma;i++){
				this->symbol2code[i]=symb2code[i];
				this->code2symbol[i+1]=symbSortByCode[i];
			}
			this->code2symbol[sigma+1]=symbSortByCode[sigma];
			this->codes = new uint*[this->maxLevel+1];
			this->nCodesAtLevel = new uint[this->maxLevel+1];
			this->posFirstCode = new uint[this->maxLevel+2];
			for (uint i=0;i<=this->maxLevel;i++) this->nCodesAtLevel[i]=nCodesAtLevel[i];
			this->posFirstCode[this->minLevel]=1;
			uint p=0;
			for (uint i=this->minLevel;i<=this->maxLevel;i++){
				this->codes[i] = new uint[this->nCodesAtLevel[i]];
				for (uint j=0;j<this->nCodesAtLevel[i];j++)
					this->codes[i][j]=codes[p++];
				for (uint j=1;j<this->nCodesAtLevel[i];j++)
					assert(this->codes[i][j]>this->codes[i][j-1]);
				this->posFirstCode[i+1]=p+1;
			}
			
		}

	MappingWMBS::MappingWMBS(){}
	
	MappingWMBS::~MappingWMBS() {
		delete [] code2symbol;
		delete [] symbol2code;
		delete [] lastCode;
		for (uint i=0;i<maxLevel;i++){
			delete [] codes[i];
		}
		delete [] codes;
		delete [] nCodesAtLevel;
		delete [] posFirstCode;
	}

	 uint MappingWMBS::getSymbol(uint code, uint len) {
		 uint index = this->posFirstCode[len]+binarySearch(this->codes[len],this->nCodesAtLevel[len],code);
		 return this->code2symbol[index];
	 }
	
	 void MappingWMBS::getCode(uint symbol, uint &code, uint &len) {
		code = this->symbol2code[symbol];
		for (uint i=minLevel;i<=maxLevel;i++){
			if (code <= lastCode[i]){
				len=i;
				return;
			}
		}
		len = maxLevel;
	 } 
	  
	bool MappingWMBS::finishCode(uint code, uint len) {
		return (len>=minLevel) && (code <= lastCode[len]);
	}
	 
	size_t MappingWMBS::getSize() {
		return sizeof(uint)*(maxLevel+1+2*sigma+2+1)+(maxLevel*2)*sizeof(uint)+sizeof(uint)*sigma+sizeof(MappingWMBS);
	}

	 void MappingWMBS::save(ofstream & fp){
		uint r = MAPPING_WM_BS; 
		saveValue(fp,r); 
		saveValue(fp, sigma); 
		saveValue(fp, minLevel);
		saveValue(fp, maxLevel); 
		saveValue(fp, symbol2code,sigma+1);
		saveValue(fp, code2symbol,sigma+2);
		saveValue(fp, lastCode,maxLevel+1);
		saveValue(fp, nCodesAtLevel,maxLevel+1);
		saveValue(fp, posFirstCode,maxLevel+2);
		for (uint i=minLevel;i<=maxLevel;i++)
			saveValue(fp, codes[i], nCodesAtLevel[i]);

	 }

	 MappingWMBS * MappingWMBS::load(ifstream & fp){
		uint type = loadValue<uint>(fp);
		if (type!=MAPPING_WM_BS) return NULL; 
		
		MappingWMBS *mp = new MappingWMBS();
		mp->sigma    = loadValue<uint>(fp); 
		mp->minLevel = loadValue<uint>(fp); 
		mp->maxLevel = loadValue<uint>(fp); 
		mp->symbol2code  = loadValue<uint>(fp,mp->sigma+1);
		mp->code2symbol  = loadValue<uint>(fp,mp->sigma+2);
		mp->lastCode     = loadValue<uint>(fp,mp->maxLevel+1);
		mp->nCodesAtLevel= loadValue<uint>(fp,mp->maxLevel+1);
		mp->posFirstCode = loadValue<uint>(fp,mp->maxLevel+2);
		mp->codes = new uint*[mp->maxLevel+1];
		for (uint i=mp->minLevel;i<=mp->maxLevel;i++)
			mp->codes[i]=loadValue<uint>(fp,mp->nCodesAtLevel[i]);
			
		return mp;
	 }
	
	uint MappingWMBS::binarySearch(uint *values, uint len, uint code) const{
		uint l=0,r=len,p=(l+r)/2;
		while(l<r){
			if (values[p]<code){
				l=p+1;
			}else{
				r=p;
			}
			p=(l+r)/2;
		}
		assert(values[p]==code);
		return p;
	}
};


