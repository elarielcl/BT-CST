/* wt_coder_huff_can.cpp
 * Copyright (C) 2014-current_year, Alberto Ordóñez, all rights reserved.
 *
 * Builds a cannonincal Huffman set of codes
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

#include <sequence/wt_coder_huff_can.h>
#include <limits.h>

namespace cds_static
{


	void wt_coder_huff_can::buildCanonical(uint max_v, uint minCode, uint maxCode){
		
		sigma = max_v+1;
		minCodeLength = minCode;
		maxCodeLength = maxCode;
		
		//~ buildCanonical(table_symbols_codes,max_v,minCodeLength,maxCodeLength);
		uint firstLevelWithTerminals = minCodeLength;
		uint *limitSection = new uint[maxCodeLength+1];
		//Computes the number of codes per level, that is, the number of terminal nodes at each level
		
		nCodesAtLevel= new uint[maxCodeLength+1];
		for (uint i=0;i<=maxCodeLength;i++)
				nCodesAtLevel[i]=limitSection[i]=0;
				
		for (uint i=0;i<max_v+1;i++){
			nCodesAtLevel[table_symbols_codes[i].len]++;
			firstLevelWithTerminals = min(firstLevelWithTerminals,table_symbols_codes[i].len);
		}

		limitSection[firstLevelWithTerminals]=nCodesAtLevel[firstLevelWithTerminals];
		for (uint i=firstLevelWithTerminals+1;i<maxCodeLength+1;i++){
			limitSection[i] = limitSection[i-1]+nCodesAtLevel[i];
		}
		uint *pointerSection = new uint[maxCodeLength+1];
		pointerSection[firstLevelWithTerminals]=0;
		for (uint i=firstLevelWithTerminals+1;i<maxCodeLength+1;i++) 
			pointerSection[i]=limitSection[i-1]; 
		
		 tuple *auxTable = new tuple[max_v+1];
		 for (uint i=0;i<max_v+1;i++){
			 auxTable[pointerSection[table_symbols_codes[i].len]++] = table_symbols_codes[i]; 
		 }
		 for (uint i=0;i<max_v+1;i++){
			 table_symbols_codes[i]=auxTable[i];
		 }

		delete [] auxTable;
		delete [] pointerSection;
		delete [] limitSection;

		firstCodeAtLevel = new uint[maxCodeLength+1];
		
		//asign consecutive codes to objects at the same level (it seems that the huffman 
		//encoder is not doing it correctly).
		uint code = 0, p = 0, maxSymbolsPerLevel = 0;
		for (uint i=1;i<=maxCodeLength;i++){
			if (nCodesAtLevel[i]==0){
				//a level with no leaves
				code*=2;
				continue;
			}else{
				firstCodeAtLevel[i]=code;
				for (uint j = 0; j<nCodesAtLevel[i]; j++){
					table_symbols_codes[p++].code = code++;					
				}
				
				maxSymbolsPerLevel=max(maxSymbolsPerLevel,nCodesAtLevel[i]);

				code *=2;//code is already (code+1) since it is always incremented after the asignment
			}
		}
		for (uint i=minCodeLength;i<=maxCodeLength;i++){
			//cerr << "first: " << firstCodeAtLevel[i] << ", nCodes: " << nCodesAtLevel[i] << ", bits(): " << bits(firstCodeAtLevel[i]+nCodesAtLevel[i]-1) << endl;
			if (!(bits(firstCodeAtLevel[i]+nCodesAtLevel[i]-1)<=i)){
				cerr << endl;;
			}
			assert(bits(firstCodeAtLevel[i]+nCodesAtLevel[i]-1)<=i);
		}
		
		//sort the symbols at each level
		tuple *sortSymbols = new tuple[maxSymbolsPerLevel];
		sortSymbols[0] = table_symbols_codes[0];
		p=1;
		uint begin=0;
		for (uint i=1;i<max_v+1;i++){
			if (table_symbols_codes[i].len == table_symbols_codes[i-1].len){
				sortSymbols[p++]=table_symbols_codes[i];
			}else{
				assert(bits(table_symbols_codes[i-1].code)<=table_symbols_codes[i-1].len);
				qsort(sortSymbols,p,sizeof(tuple),cmpTupleBySymbol);
				uint k=0;
				uint code = firstCodeAtLevel[sortSymbols[0].len];
				for (uint j=begin;j<(begin+p); j++){
					 table_symbols_codes[j]=sortSymbols[k++];
					 table_symbols_codes[j].code = code++;
				 }
				if (p>1){
					for (uint w=begin;w<(begin+p-1);w++){
						assert(table_symbols_codes[w].code+1==table_symbols_codes[w+1].code);
					}
				}
				begin = i;
				p=1;
				sortSymbols[0] = table_symbols_codes[i];
			}
		}
			//sort the last level of the tree
			qsort(sortSymbols,p,sizeof(tuple),cmpTupleBySymbol); 
			uint k = 0;
			code = firstCodeAtLevel[sortSymbols[0].len];
			for (uint j=begin;j<(begin+p); j++){
				table_symbols_codes[j]=sortSymbols[k++];
				table_symbols_codes[j].code=code++;
			}
		delete [] sortSymbols;
		for (uint i=1;i<=max_v;i++){
			if (table_symbols_codes[i].len==table_symbols_codes[i-1].len){
				assert(table_symbols_codes[i].code==(table_symbols_codes[i-1].code+1));
			}
		}
		table_len=uint_len(max_v+2,maxCodeLength);
		symbol2code = new uint [table_len];
		for (uint i=0;i<=max_v; i++){
			uint invCode=0;
			for (uint j=0;j<table_symbols_codes[i].len;j++){
				if (bitget(&(table_symbols_codes[i].code),j))
					bitset(&invCode,table_symbols_codes[i].len-j-1);
			}
			set_field(symbol2code, maxCodeLength, i, invCode);
		}

		assert(testCanonicalSorted());
		
	}
	
	wt_coder_huff_can::wt_coder_huff_can(wt_coder_huff *wc, uint max_v){
		sigma = max_v+1;
		table_symbols_codes = new tuple[max_v+1];
		maxCodeLength = 0;
		minCodeLength = INT_MAX;
		float avgLenHuff=0.0;
		for (uint i=0;i<=max_v; i++){
			table_symbols_codes[i].symbol = i;//symbols[i];
			table_symbols_codes[i].code = 0;
			table_symbols_codes[i].occs = -1;// occurrences[symbols[i]];
			table_symbols_codes[i].len = wc->getCode(i, table_symbols_codes[i].code);
			avgLenHuff+=table_symbols_codes[i].len;
			maxCodeLength = max(maxCodeLength,table_symbols_codes[i].len);
			minCodeLength = min(minCodeLength,table_symbols_codes[i].len);
		}
		buildCanonical(max_v,minCodeLength,maxCodeLength);
	}
	
	
    wt_coder_huff_can::wt_coder_huff_can(const Array & a, Mapper * am) {
		cerr << "Not implemented yet" << endl;
    }
	//returns the length of the code and stores in code the code of symbol
	uint wt_coder_huff_can::getCode(uint symbol, uint &code) const{
	  // for (uint i=0;i<sigma;i++){
	  // 	if (table_symbols_codes[i].symbol==symbol){
	  // 		code = table_symbols_codes[i].code;
	  // 		return table_symbols_codes[i].len;
	  // 	}
	  // }
//	  assert(table_symbols_codes[symbol].symbol==symbol);
	  code = table_symbols_codes[symbol].code;
	  return table_symbols_codes[symbol].len;
	}

    wt_coder_huff_can::wt_coder_huff_can(uint * symbs, size_t n, Mapper * am) {
		cerr << "Not implemented yet" << endl;
    }

    wt_coder_huff_can::wt_coder_huff_can(uchar * symbs, size_t n, Mapper * am) {
		cerr << "Not implemented yet" << endl;
    }

    wt_coder_huff_can::wt_coder_huff_can() {}

    wt_coder_huff_can::~wt_coder_huff_can() {
        delete [] symbol2code;
    }

	uint * wt_coder_huff_can::get_symbol(uint symbol) const {
		uint code=get_field(symbol2code,maxCodeLength,symbol); 		
		uint *c = new uint[1];
		c[0]=code;
		return c;
	}

    bool wt_coder_huff_can::is_set(uint symbol, uint l) const
    {
		uint code;
		code = get_field(symbol2code,maxCodeLength,symbol); 		
		return (code>>l)&1;
    }

    bool wt_coder_huff_can::is_set(uint *symbol, uint l) const {
		
        return ((*symbol)>>l)&1;
    }

    bool wt_coder_huff_can::done(uint symbol, uint l) const
    {
		cerr << "not implemented yet"<<endl;
		return false;
    }

    size_t wt_coder_huff_can::getSize() const
    {
		return sizeof(wt_coder_huff_can)+sizeof(uint)*table_len;
    }

    tuple *wt_coder_huff_can::getCopyTableSymbols(){
    	tuple *copy = new tuple[sigma];
    	for (uint i=0;i<sigma;i++){
    		copy[i]=table_symbols_codes[i];
    	}
    	return copy;
    }
    void wt_coder_huff_can::save(ofstream & fp) const
    {
		
        uint wr = WT_CODER_HUFF_CAN_HDR;
        saveValue(fp,wr);
        saveValue(fp,table_len);
        saveValue(fp,maxCodeLength);
        saveValue(fp,minCodeLength);
        saveValue(fp,nCodesAtLevel,maxCodeLength+1);
        saveValue(fp,symbol2code,table_len);
    }

    wt_coder_huff_can * wt_coder_huff_can::load(ifstream & fp) {
        uint rd = loadValue<uint>(fp);
        if(rd!=WT_CODER_HUFF_CAN_HDR) return NULL;
        wt_coder_huff_can * ret = new wt_coder_huff_can();
        ret->table_len=loadValue<uint>(fp);
        ret->maxCodeLength=loadValue<uint>(fp);
        ret->minCodeLength=loadValue<uint>(fp);
        ret->nCodesAtLevel = new uint[ret->maxCodeLength+1];
        ret->nCodesAtLevel=loadValue<uint>(fp,ret->maxCodeLength+1);
        ret->symbol2code = new uint[ret->table_len];
        ret->symbol2code = loadValue<uint>(fp,ret->table_len);
        cerr << ret->table_len << endl;
        cerr << ret->maxCodeLength << endl;
        return ret;
    }

    bool wt_coder_huff_can::testCanonicalSorted(){
    	for (uint i=0;i<sigma-1;i++){
    		assert(bits(table_symbols_codes[i].code)<=table_symbols_codes[i].len);
    		assert((table_symbols_codes[i].len==table_symbols_codes[i+1].len && table_symbols_codes[i].code+1==table_symbols_codes[i+1].code && 
    				table_symbols_codes[i].symbol<table_symbols_codes[i+1].symbol) || (table_symbols_codes[i].len!=table_symbols_codes[i+1].len));
    	}
    	return true;
    }
};
