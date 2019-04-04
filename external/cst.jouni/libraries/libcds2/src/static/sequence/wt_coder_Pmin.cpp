/* wt_coder_Pmin.h
 * Copyright (C) 2014-current_year, Alberto Ordóñez, all rights reserved.
 *
 * Increments the frequency of the least frequent symbols in order to obtain a flatter
   encoding (limit the length of a code to a given bound).
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

#include <sequence/wt_coder_Pmin.h>
#include <climits>

namespace cds_static
{
	
	/**
	 * It takes as input a huffman set of codes (we first convert it to canonical...it is 
	 * easier to handle it, but it is not actually necessary). The algorithm 
	 * cuts the nodes that have hieght >=L. It simulates the ideas 
	 * of Milidiú & Laber[Algorithmica(2001)31: 513-529]. However, we do not handle trees but code lengths
	 * instead (you can get the tree from the set of codes lengths and the first code of each length since it 
	 * is balanced). We first cut what is above level L-1 
	 * and we simulate building a balanced tree with those nodes. Then, we the first left branch of the root and
	 * increases the height of each tree node that is dangling from it (codes that start by 0 and have length <L). 
	 * Finally we simulate pasting the rebalanced subtree down the right branch of the left child of the root. Note 
	 * this left child of the root is a new node we have created to make room to paste the rebalanced tree. 
	 * 
	 * */
	wt_coder_Pmin::wt_coder_Pmin(wt_coder_huff *wc, uint max_v, uint L, MappingBuilder *ms):wt_coder_huff_can(){
		// build(wc,max_v,L);
		// sigma = max_v+1;
		// assert(testCanonicalSorted());
		// model = ms->build(table_symbols_codes, sigma);// new MappingPermutations( bsb);
		// 	minCodeLength = INT_MAX;
		// maxCodeLength = 0;
		// for (uint i=0;i<=max_v;i++){
		// 	minCodeLength=min(minCodeLength,table_symbols_codes[i].len);
		// 	maxCodeLength=max(maxCodeLength,table_symbols_codes[i].len);
		// }
		// //cerr << "min: " << minCodeLength << ", max: " << maxCodeLength << endl;
		// assert(testCodeDecode());
	}

	wt_coder_Pmin::wt_coder_Pmin(wt_coder_huff *wc, uint max_v, uint L, BitSequenceBuilder *bsb):wt_coder_huff_can(){
		// build(wc,max_v,L);
		// sigma = max_v+1;
		// assert(testCanonicalSorted());
		// model = new MappingPermutations(table_symbols_codes, sigma, bsb);
		// assert(testCodeDecode());
	}
	/**
	 * L = (maxCodeLength-minCodeLength+1)*Lfactor.
	 * */
	wt_coder_Pmin::wt_coder_Pmin(wt_coder_huff *wc, uint max_v, float Lfactor):wt_coder_huff_can(){
		// assert(Lfactor>0 && Lfactor<1);
		// uint minLen,maxLen;
		// minLen=INT_MAX;
		// maxLen=0;
		// for (uint i=0;i<max_v+1; i++){
		// 	uint dummy;
		// 	uint len = wc->getCode(i, dummy);
		// 	minLen=min(minLen,len);
		// 	maxLen=max(maxLen,len);
		// }
		// uint Lprime = (uint)minLen+(maxLen-minLen+1)*Lfactor;
		// build(wc,max_v,Lprime);
		// sigma = max_v+1;	
	}
	
    wt_coder_Pmin::wt_coder_Pmin(const Array & a, Mapper * am) {
		cerr << "Not implemented yet" << endl;
    }
	//returns the length of the code and stores in code the code of symbol
	uint wt_coder_Pmin::getCode(uint symbol, uint &code) const{
	  uint len,leaf;
	  	model->getCode(symbol, code, len, leaf);
	 return len;
	}

    wt_coder_Pmin::wt_coder_Pmin(uint * symbs, size_t n, uint L, float minFreq, MappingBuilder *ms) {
    	uint max_v=0;
    	for (uint i=0;i<n;i++){
    		max_v=max(symbs[i],max_v);
    	}
    	uint *noccs = new uint[max_v+1];
    	for (uint i=0;i<n;i++){
    		noccs[symbs[i]]++;
    	}
    	double avgFreq =0.0;
    	for (uint i=0;i<=max_v;i++){
    		avgFreq+=noccs[i];
    	}
    	uint *origNoccs = new uint[max_v+1];
    	for (uint i=0;i<=max_v;i++){
	    	origNoccs[i] = noccs[i]; 
	    }
	    
	    uint limitCounter=500;
    	while(minFreq<limitCounter){
    		
	    	for (uint i=0;i<=max_v;i++){
	    		noccs[i] = origNoccs[i];
	    	}

    	// cerr << "avgFreq: " << avgFreq/(max_v+1) << endl;

		// double minFreq = (1.0+sqrt(5.0))/2.0;
		
		// minFreq = 1.0/pow(minFreq,L);
		// cerr << "n: "<<n << endl;
		// cerr << "minFreq: " << minFreq << endl;
		// minFreq*=n;
		// cerr << "minFreq: " << minFreq << endl;
		// cerr << "sigma: " << max_v+1 << endl;
		// cerr << "bits(sigma): " << bits(max_v+1) << endl;

		// minFreq = 8*avgFreq/(max_v+1);
		// cerr << "minFreq: " << minFreq << endl;
		// uint newTotal = 0;
		// uint fails=0;
		// minFreq/=8;
    	for (uint i=0;i<=max_v;i++){
    		if (noccs[i]<(uint)minFreq){
//    			noccs[i]+=(uint)minFreq;
				noccs[i]=(uint)minFreq;
    			// fails++;
    		}
    		// newTotal+=noccs[i];
    	}
    	// cerr << "fails: " << fails << endl;
    	for (uint i=0;i<=max_v;i++){
    		assert(noccs[i]>=(uint)minFreq);
    	}
    	// cerr << "n: " << n << ", newtotal: " << newTotal << endl;	
    	wt_coder_huff *wc = new wt_coder_huff(noccs,max_v+1);
    	table_symbols_codes = new tuple[max_v+1];
    	maxCodeLength = 0;
		minCodeLength = ~0;
		float avgLenHuff=0.0;
		for (uint i=0;i<=max_v; i++){
			table_symbols_codes[i].symbol = i;//symbols[i];
			table_symbols_codes[i].code = 0;
			table_symbols_codes[i].occs = -1;// occurrences[symbols[i]];
			table_symbols_codes[i].len = wc->getCode(i, table_symbols_codes[i].code);
			avgLenHuff+=table_symbols_codes[i].len;
			if (table_symbols_codes[i].len > maxCodeLength) maxCodeLength = table_symbols_codes[i].len;
			if (table_symbols_codes[i].len < minCodeLength) minCodeLength = table_symbols_codes[i].len;
		}
		// cerr << "maxCodeLength: " << maxCodeLength << endl;
			if (maxCodeLength!=L){
				minFreq++;
				delete wc;
				delete [] table_symbols_codes;
			}else{
				cerr << "minFreq: " << minFreq << ", lmax: " << L << endl;
				break;
			}
		}
		if (minFreq==limitCounter){
			cerr << "lmax: " << L << ", minFreq: " << minFreq << " result not found" << endl;
			abort();
		}
		delete [] origNoccs;
		assert(maxCodeLength<=L);
		buildCanonical(max_v,minCodeLength,maxCodeLength);
		// for (uint i=0;i<maxCodeLength;i++)
		// 	cerr << "nCodes["<<i<<"]: " << nCodesAtLevel[i] << endl;
		assert(testCanonicalSorted());
		model = ms->build(table_symbols_codes, max_v+1);// new MappingPermutations( bsb);
		assert(testCodeDecode());
		
    }

     wt_coder_Pmin::wt_coder_Pmin(uint * symbs, size_t n, uint L, MappingBuilder *ms, ofstream &of) {
    	uint max_v=0;
    	for (uint i=0;i<n;i++){
    		max_v=max(symbs[i],max_v);
    	}
    	uint *noccs = new uint[max_v+1];
    	for (uint i=0;i<n;i++){
    		noccs[symbs[i]]++;
    	}

		double minFreq = (1+sqrt(5))/2;
		minFreq = ceil(pow(minFreq,-1.0*L)*n);

    	for (uint i=0;i<=max_v;i++){
    		noccs[i]=max(noccs[i],(uint)minFreq);
    	}
    	for (uint i=0;i<=max_v;i++){
    		assert(noccs[i]>=minFreq);
    	}	
    	wt_coder_huff *wc = new wt_coder_huff(noccs,max_v+1);
    	
    	wc->save(of);

    	table_symbols_codes = new tuple[max_v+1];
    	maxCodeLength = 0;
		minCodeLength = ~0;
		float avgLenHuff=0.0;
		for (uint i=0;i<=max_v; i++){
			table_symbols_codes[i].symbol = i;//symbols[i];
			table_symbols_codes[i].code = 0;
			table_symbols_codes[i].occs = -1;// occurrences[symbols[i]];
			table_symbols_codes[i].len = wc->getCode(i, table_symbols_codes[i].code);
			avgLenHuff+=table_symbols_codes[i].len;
			if (table_symbols_codes[i].len > maxCodeLength) maxCodeLength = table_symbols_codes[i].len;
			if (table_symbols_codes[i].len < minCodeLength) minCodeLength = table_symbols_codes[i].len;
		}
		assert(maxCodeLength<=L);
		buildCanonical(max_v,minCodeLength,maxCodeLength);
		
		assert(testCanonicalSorted());
		
		model = ms->build(table_symbols_codes, max_v+1);// new MappingPermutations( bsb);
		assert(testCodeDecode());
		
    }



    wt_coder_Pmin::wt_coder_Pmin(uchar * symbs, size_t n, Mapper * am) {
		cerr << "Not implemented yet" << endl;
    }

    wt_coder_Pmin::wt_coder_Pmin() {}

    wt_coder_Pmin::~wt_coder_Pmin() {
    }

    uint wt_coder_Pmin::getSymbol(uint code) const{
        return model->getSymbol(code);
    }

    uint wt_coder_Pmin::getSymbol(uint code, uint len) const{
        return model->getSymbol(code, len);
    }

	uint * wt_coder_Pmin::get_symbol(uint symbol) const {
		cerr << "get_symbol not implemented yet (suboptimal add)" << endl;
		return nullptr;
	}

    bool wt_coder_Pmin::is_set(uint symbol, uint l) const
    {
		uint code;
		code = get_field(symbol2code,maxCodeLength,symbol); 		
		return (code>>l)&1;
    }

    bool wt_coder_Pmin::is_set(uint *symbol, uint l) const {
		
        return ((*symbol)>>l)&1;
    }

    bool wt_coder_Pmin::done(uint symbol, uint l) const
    {
		cerr << "not implemented yet"<<endl;
		return false;
    }

    size_t wt_coder_Pmin::getSize() const
    {
		return sizeof(wt_coder_Pmin)+model->getSize();
    }

    void wt_coder_Pmin::save(ofstream & fp) const
    {
		
        uint wr = WT_CODER_HUFF_CAN_HDR;
        saveValue(fp,wr);
        saveValue(fp,table_len);
        saveValue(fp,maxCodeLength);
        saveValue(fp,minCodeLength);
        saveValue(fp,nCodesAtLevel,maxCodeLength+1);
        saveValue(fp,symbol2code,table_len);
    }

    wt_coder_Pmin * wt_coder_Pmin::load(ifstream & fp) {
        uint rd = loadValue<uint>(fp);
        if(rd!=WT_CODER_HUFF_CAN_HDR) return NULL;
        wt_coder_Pmin * ret = new wt_coder_Pmin();
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

    bool wt_coder_Pmin::testCodeDecode() const{

    	for (uint i=0;i<sigma;i++){
    		uint code,len,leaf;
    		model->getCode(table_symbols_codes[i].symbol, code, len, leaf);
    		assert(code==table_symbols_codes[i].code);
    		if (!(len==table_symbols_codes[i].len)){
    			model->getCode(table_symbols_codes[i].symbol, code, len, leaf);
    		}
    		assert(len==table_symbols_codes[i].len);
    		code=0;
    		len = getCode(table_symbols_codes[i].symbol,code);
    		assert(code==table_symbols_codes[i].code);
    		assert(len==table_symbols_codes[i].len);
    		uint symbol = model->getSymbol(code, len);
    		assert(symbol==table_symbols_codes[i].symbol);
    		assert(table_symbols_codes[i].symbol==getSymbol(code, len));
    	}
    	return true;
    }
};
