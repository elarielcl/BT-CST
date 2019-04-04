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

#include <sequence/MappingWMBMP.h>

namespace cds_static
{
	
		MappingWMBMP::MappingWMBMP(uint sigma, uint minLevel, uint maxLevel, uint *symb2code, uint *symbSortByCode, uint *lastCode, uint *bmpCodes, BitSequenceBuilder *bsb):MappingWM(sigma,minLevel,maxLevel){
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
			this->code2symbol_bmp = bsb->build(bmpCodes, (1<<maxLevel)); 
		}

	MappingWMBMP::MappingWMBMP(){}
	
	MappingWMBMP::~MappingWMBMP() {
		delete [] code2symbol;
		delete [] symbol2code;
		delete [] lastCode;
		delete code2symbol_bmp;
	}

	 uint MappingWMBMP::getSymbol(uint code, uint len) {
		return this->code2symbol[this->code2symbol_bmp->rank1(code)]; 
	 }
	
	 void MappingWMBMP::getCode(uint symbol, uint &code, uint &len) {
		code = this->symbol2code[symbol];
		for (uint i=minLevel;i<=maxLevel;i++){
			if (code <= lastCode[i]){
				len=i;
				return;
			}
		}
		len = maxLevel;
	 } 
	  
	bool MappingWMBMP::finishCode(uint code, uint len) {
		return (len>=minLevel) && (code <= lastCode[len]);
	}
	 
	size_t MappingWMBMP::getSize() {
		return sizeof(uint)*(maxLevel+1+2*sigma+2+1)+code2symbol_bmp->getSize()+sizeof(MappingWMBMP);
	}

	 void MappingWMBMP::save(ofstream & fp){
		uint r = MAPPING_WM_BMP; 
		saveValue(fp,r); 
		saveValue(fp, sigma); 
		saveValue(fp, minLevel);
		saveValue(fp, maxLevel); 
		saveValue(fp, symbol2code,sigma+1);
		saveValue(fp, code2symbol,sigma+2);
		saveValue(fp, lastCode,maxLevel+1);
		code2symbol_bmp->save(fp);
	 }

	 MappingWMBMP * MappingWMBMP::load(ifstream & fp){
		uint type = loadValue<uint>(fp);
		if (type!=MAPPING_WM_BMP) return NULL; 
		
		MappingWMBMP *mp = new MappingWMBMP();
		mp->sigma    = loadValue<uint>(fp); 
		mp->minLevel = loadValue<uint>(fp); 
		mp->maxLevel = loadValue<uint>(fp); 
		mp->symbol2code  = loadValue<uint>(fp,mp->sigma+1);
		mp->code2symbol  = loadValue<uint>(fp,mp->sigma+2);
		mp->lastCode     = loadValue<uint>(fp,mp->maxLevel+1);
		mp->code2symbol_bmp  = BitSequence::load(fp);
		return mp;
	 }
	
};


