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

#include <sequence/MappingWMBlocks.h>

namespace cds_static
{
	
		MappingWMBlocks::MappingWMBlocks(uint sigma, uint minLevel, uint maxLevel, uint *symb2code, uint *symbSortByCode, uint *lastCode, uint *codes, uint *nCodesAtLevel):MappingWM(sigma,minLevel,maxLevel){
			this->sigma = sigma;
			this->minLevel = minLevel; 
			this->maxLevel = maxLevel; 
			this->lastCode = new uint[maxLevel+1];
			for (uint i=0;i<=this->maxLevel;i++){
				this->lastCode[i]=lastCode[i];
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
			
			this->nCodesAtLevel = new uint[this->maxLevel+1];
			this->posFirstCode = new uint[this->maxLevel+2];
			for (uint i=0;i<=this->maxLevel;i++) this->nCodesAtLevel[i]=nCodesAtLevel[i];
			this->posFirstCode[this->minLevel]=1;
			
			this->cb = new unsigned char*[this->maxLevel+1];
			this->cs = new unsigned short int*[this->maxLevel+1];
			this->ci = new uint*[this->maxLevel+1];
			
			uint p=0;
			for (uint i=minLevel;i<=maxLevel;i++){
				if (i<=8){
					this->cb[i]=new unsigned char[this->nCodesAtLevel[i]];
					for (uint j=0;j<this->nCodesAtLevel[i];j++)
						this->cb[i][j]=codes[p++];
					this->posFirstCode[i+1]=p+1;
				}else if (i<=16){
					this->cs[i]=new unsigned short int[this->nCodesAtLevel[i]];
					for (uint j=0;j<this->nCodesAtLevel[i];j++)
						this->cs[i][j]=codes[p++];
					this->posFirstCode[i+1]=p+1;
				}else if (i<=24){
					this->cb[i]=new unsigned char[this->nCodesAtLevel[i]];
					this->cs[i]=new unsigned short int[this->nCodesAtLevel[i]];
					for (uint j=0;j<this->nCodesAtLevel[i];j++){
						this->cb[i][j]=(codes[p]>>16)&0xff;
						this->cs[i][j]=codes[p]&0xffff;
						p++;
					}
					this->posFirstCode[i+1]=p+1;
				}else if (i<=32){
					this->ci[i]=new uint[this->nCodesAtLevel[i]];
					for (uint j=0;j<this->nCodesAtLevel[i];j++)
						this->ci[i][j]=codes[p++];
					this->posFirstCode[i+1]=p+1;
				}
			}
			
		}

	MappingWMBlocks::MappingWMBlocks(){}
	
	MappingWMBlocks::~MappingWMBlocks() {
		delete [] code2symbol;
		delete [] symbol2code;
		delete [] lastCode;
		for (uint i=0;i<maxLevel;i++){
			delete [] cb[i];
			delete [] cs[i];
			delete [] ci[i];
		}
		delete [] cb;
		delete [] cs;
		delete [] ci;
		delete [] nCodesAtLevel;
		delete [] posFirstCode;
	}

	 uint MappingWMBlocks::getSymbol(uint code, uint len) {
		 uint index = this->posFirstCode[len]+binarySearch(len,this->nCodesAtLevel[len],code);
		 return get_field(this->code2symbol,minBitsCode,index);
	 }
	
	 void MappingWMBlocks::getCode(uint symbol, uint &code, uint &len) {
		code = get_field(this->symbol2code, maxLevel, symbol);
		for (uint i=minLevel;i<=maxLevel;i++){
			if (code <= lastCode[i]){
				len=i;
				return;
			}
		}
		len = maxLevel;
	 } 
	  
	bool MappingWMBlocks::finishCode(uint code, uint len) {
		return (len>=minLevel) && (code <= lastCode[len]);
	}
	 
	size_t MappingWMBlocks::getSize() {
		uint total=0;
		for (uint i=minLevel;i<=maxLevel;i++){
			if (i<=8){
				total+=sizeof(unsigned char)*nCodesAtLevel[i];
			}else if (i>24){
				total+=sizeof(uint)*nCodesAtLevel[i];
			}else if (i<=16){
				total+=sizeof(short int)*nCodesAtLevel[i];
			}else{
				total+=sizeof(unsigned char)*nCodesAtLevel[i];
				total+=sizeof(short int)*nCodesAtLevel[i];
			}
		}
		
		total+=uint_len(maxLevel,sigma+1)*sizeof(uint);//symb2code
		total+=uint_len(minBitsCode,sigma+2)*sizeof(uint);//codes2symb
		total+=(maxLevel*3+4)*sizeof(uint);
		return total+sizeof(MappingWMBlocks);
	}

	 void MappingWMBlocks::save(ofstream & fp){
		uint r = MAPPING_WM_BLOCKS; 
		saveValue(fp,r); 
		saveValue(fp, sigma); 
		saveValue(fp, minLevel);
		saveValue(fp, maxLevel);
		 
		saveValue(fp, symbol2code,uint_len(maxLevel,sigma+1));
		saveValue(fp, code2symbol,uint_len(minBitsCode,sigma+2));
		
		saveValue(fp, lastCode,maxLevel+1);
		saveValue(fp, nCodesAtLevel,maxLevel+1);
		saveValue(fp, posFirstCode,maxLevel+2);
		
		for (uint i=minLevel;i<=maxLevel;i++){
			if (i<=8){
				saveValue(fp,cb[i],nCodesAtLevel[i]);
			}else if (i>24){
				saveValue(fp,ci[i],nCodesAtLevel[i]);
			}else if (i<=16){
				saveValue(fp,cs[i],nCodesAtLevel[i]);
			}else{
				saveValue(fp,cb[i],nCodesAtLevel[i]);
				saveValue(fp,cs[i],nCodesAtLevel[i]);
			}
		}
	 }

	 MappingWMBlocks * MappingWMBlocks::load(ifstream & fp){
		uint type = loadValue<uint>(fp);
		if (type!=MAPPING_WM_BLOCKS) return NULL; 
		
		MappingWMBlocks *mp = new MappingWMBlocks();
		mp->sigma    = loadValue<uint>(fp); 
		mp->minLevel = loadValue<uint>(fp); 
		mp->maxLevel = loadValue<uint>(fp); 
		mp->minBitsCode = bits(mp->sigma+1);
		
		mp->symbol2code  = loadValue<uint>(fp,uint_len(mp->maxLevel,mp->sigma+1));
		mp->code2symbol  = loadValue<uint>(fp,uint_len(mp->minBitsCode,mp->sigma+2));
		
		mp->lastCode     = loadValue<uint>(fp,mp->maxLevel+1);
		mp->nCodesAtLevel= loadValue<uint>(fp,mp->maxLevel+1);
		mp->posFirstCode = loadValue<uint>(fp,mp->maxLevel+2);
		
		//~ mp->codes = new uint*[mp->maxLevel+1];
		mp->cb = new unsigned char*[mp->maxLevel+1];
		mp->cs = new unsigned short int*[mp->maxLevel+1];
		mp->ci = new uint*[mp->maxLevel+1];
		
		for (uint i=mp->minLevel;i<=mp->maxLevel;i++){
			if (i<=8){
				mp->cb[i]=loadValue<unsigned char>(fp,mp->nCodesAtLevel[i]);
			}else if (i<=16){
				mp->cs[i]=loadValue<unsigned short int>(fp,mp->nCodesAtLevel[i]);
			}else if (i<=24){
				mp->cb[i]=loadValue<unsigned char>(fp,mp->nCodesAtLevel[i]);
				mp->cs[i]=loadValue<unsigned short int>(fp,mp->nCodesAtLevel[i]);
			}else{
				mp->ci[i]=loadValue<uint>(fp,mp->nCodesAtLevel[i]);
			}
		}
		
		return mp;
	 }
	
	uint MappingWMBlocks::binarySearch(uint level, uint len, uint code) const{
		uint l=0,r=len,p=(l+r)/2;
		while(l<r){
			if (getValue(level,p)<code){
				l=p+1;
			}else{
				r=p;
			}
			p=(l+r)/2;
		}
		assert(getValue(level,p)==code);
		return p;
	}
	
	uint MappingWMBlocks::getValue(uint level, uint pos) const{

		if (level<=8){
			return (uint)this->cb[level][pos];
		}else if (level<=16){
			return (uint) this->cs[level][pos];
		}else if (level<=24){
			return (((uint)this->cb[level][pos])<<16)|(uint)this->cs[level][pos]; 
		}else{
			return this->ci[level][pos];
		}
		//~ if (level<=8){
			//~ return (uint)this->cb[level][pos];
		//~ }else if (level>24){
			//~ return this->ci[level][pos];
		//~ }else if (level<=16){
			//~ return (uint) this->cs[level][pos];
		//~ }else{
			//~ return (((uint)this->cb[level][pos])<<16)|(uint)this->cs[level][pos]; 
		//~ }
	}
};


