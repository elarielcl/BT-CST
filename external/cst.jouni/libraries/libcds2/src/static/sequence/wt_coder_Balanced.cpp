/* wt_coder_Balanced.h
 * Copyright (C) 2014-current_year, Alberto Ordóñez, all rights reserved.
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

#include <sequence/wt_coder_Balanced.h>
#include <climits>

namespace cds_static
{
	
	uint wt_coder_Balanced::nCodesUnderLevel(uint level){
		uint nc = 0;
		for (uint i=level;i<=maxCodeLength;i++){
			nc+=nCodesAtLevel[i];
		}
		return nc;
	}
	/*
		Given a code (node) at level "level", it expands node with 0's until
		some of that expansions is a leaf. Finally returns the level in which
		that leaf is found. 
	*/
	uint wt_coder_Balanced::getLeftMostCodeLength(uint node, uint level){
	
		for (int i=level;i<=maxCodeLength;i++){
			uint c = node<<(i-level);
			if (c<firstCodeAtLevel[i]+nCodesAtLevel[i]){
				return i;
			}
		}
		return -1;	
	}

	void wt_coder_Balanced::build(wt_coder_huff *wc, uint max_v, uint L){
		
		table_symbols_codes = new tuple[max_v+1];
		maxCodeLength = 0;
		minCodeLength = ~0;
		for (uint i=0;i<=max_v; i++){
			table_symbols_codes[i].symbol = i;//symbols[i];
			table_symbols_codes[i].code = 0;
			table_symbols_codes[i].occs = -1;// occurrences[symbols[i]];
			table_symbols_codes[i].len = wc->getCode(i, table_symbols_codes[i].code);
			if (table_symbols_codes[i].len > maxCodeLength) maxCodeLength = table_symbols_codes[i].len;
			if (table_symbols_codes[i].len < minCodeLength) minCodeLength = table_symbols_codes[i].len;
		}
		
		buildCanonical(max_v,minCodeLength,maxCodeLength);		
      	
      	// for (uint i=0;i<=maxCodeLength;i++){
      	// 	cout << "nCodes_prev[" << i << "]: " << nCodesAtLevel[i] << endl;
      	// }
      	if (L==maxCodeLength){
      		cerr << "L(" << L << ") == maxCodeLength("<<maxCodeLength<< ")."<<endl;
      		abort();
      	}
      	if (L<bits(max_v+1)){
      		cerr << "L(" << L << ") < bits(maxCodeLength)("<<bits(max_v+1)<< ")."<<endl;
			abort();
      	}

		{
			uint countNCodes = 0;
			for (uint i=minCodeLength;i<=maxCodeLength;i++)
				countNCodes+=nCodesAtLevel[i];
			assert(countNCodes==max_v+1);
		}
		
		
		uint node; 
		int level = L;
		uint firstCode;
		uint ncodes = 0;
		while(level>=minCodeLength){
			if (nCodesAtLevel[level]>0){
				//node is the rightmost node (not code) at level "level"
				node = (1<<level)-1;
				//levelFirst is the first code we can obtain by extending node with 0's
				uint levelFirst = getLeftMostCodeLength(node,level);

				if (levelFirst<=L){
					//is the first code we obtain by extending node with 0's
					firstCode = node<<(levelFirst-level);
					assert(firstCode <firstCodeAtLevel[levelFirst]+nCodesAtLevel[levelFirst]);
					//ncodes of length levelFirst to the right of firstCode (inclusive) 
					ncodes = nCodesAtLevel[levelFirst]-(firstCode - firstCodeAtLevel[levelFirst]);
					ncodes+=nCodesUnderLevel(levelFirst+1);
					if (level+bits(ncodes)<=L){
						break;
					}
				}
			}
			level--;
		}
		//the next line implies practically all our solutions have length log(sigma)	
		if (level<minCodeLength){
			//we need to build a balanced tree with all the codes
			firstCode = 0;
			ncodes = sigma;
			level=0;
			//we need to rebalance the whole tree
		}
		// we set the code length of all those symbols to reblance to the depth of the 
		// first node that contains them all.  
		assert(ncodes);
		uint *symbolsToRebalance = new uint[ncodes];
		uint pointerRebalance=0;
		for (uint i=0;i<=max_v; i++){
			if (table_symbols_codes[i].code>=firstCode){
				symbolsToRebalance[pointerRebalance++]=i;
				table_symbols_codes[i].len = level;
			}	
		}
		// for (uint i=0;i<=maxCodeLength;i++){
		// 	cerr << "nCodes["<<i<<"]: " << nCodesAtLevel[i] << endl;
		// }
		assert(ncodes==pointerRebalance);

		recAsignCodes(symbolsToRebalance, ncodes);

		//recompute the min/max code length
		minCodeLength = INT_MAX;
		maxCodeLength = 0;
		for (uint i=0;i<=max_v;i++){
			minCodeLength=min(minCodeLength,table_symbols_codes[i].len);
			maxCodeLength=max(maxCodeLength,table_symbols_codes[i].len);
		}
		if (maxCodeLength>L){
			cerr << "Warning: maxCodeLength("<<maxCodeLength<<")>L("<<L<<")"<<endl;
			cerr << "Program will abort (wt_coder_SubOptimalAdd)" << endl;
			abort();
		}
		
		//assert(maxCodeLength==L);
		//build again the canonical code but with the new set of lengths
		buildCanonical(max_v,minCodeLength,maxCodeLength);

		// for (uint i=minCodeLength;i<=maxCodeLength;i++){
		// 	cerr << "nCodes["<<i<<"]: " << nCodesAtLevel[i] << endl;
		// }

		//Done.
	}
	
	uint wt_coder_Balanced::recAsignCodes(uint *posSymbolsRebalance, uint n){
		assert(n);
		if (n==1){
			return n;
		}
		uint nbits = bits(n-1);
		uint maxElem = 1<<nbits;
		int j;
		int limSup = (maxElem==n)?n:maxElem/2;
		uint len = nbits;
		for (j=0;j<limSup;j++){
			table_symbols_codes[posSymbolsRebalance[j]].len+=len;
			if (table_symbols_codes[posSymbolsRebalance[j]].len<10){
				cerr << endl;
			}
		}
		for (;j<n;j++){
			table_symbols_codes[posSymbolsRebalance[j]].len++;
		}
		if (limSup==n)
			return n;

		posSymbolsRebalance+=limSup;
		
		uint total = recAsignCodes(posSymbolsRebalance, n-limSup);
		assert(total+limSup==n);
		return n;
	}
	
	// uint wt_coder_Balanced::recAsignCodes(uint *posSymbolsRebalance, uint n){
	// 	assert(n);
		
	// 	uint nbits = bits(n-1);
	// 	for (uint i=0;i<n;i++){
	// 		table_symbols_codes[posSymbolsRebalance[i]].len+=nbits;

	// 	}
	// 	return n;
	// }
		
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
	wt_coder_Balanced::wt_coder_Balanced(wt_coder_huff *wc, uint max_v, uint L, MappingBuilder *ms):wt_coder_huff_can(){
		build(wc,max_v,L);
		sigma = max_v+1;
		assert(testCanonicalSorted());
		model = ms->build(table_symbols_codes, sigma);// new MappingPermutations( bsb);
			minCodeLength = INT_MAX;
		maxCodeLength = 0;
		for (uint i=0;i<=max_v;i++){
			minCodeLength=min(minCodeLength,table_symbols_codes[i].len);
			maxCodeLength=max(maxCodeLength,table_symbols_codes[i].len);
		}
		//cerr << "min: " << minCodeLength << ", max: " << maxCodeLength << endl;
		assert(testCodeDecode());
	}

	wt_coder_Balanced::wt_coder_Balanced(wt_coder_huff *wc, uint max_v, uint L, BitSequenceBuilder *bsb):wt_coder_huff_can(){
		build(wc,max_v,L);
		sigma = max_v+1;
		assert(testCanonicalSorted());
		model = new MappingPermutations(table_symbols_codes, sigma, bsb);
		assert(testCodeDecode());
	}
	/**
	 * L = (maxCodeLength-minCodeLength+1)*Lfactor.
	 * */
	wt_coder_Balanced::wt_coder_Balanced(wt_coder_huff *wc, uint max_v, float Lfactor):wt_coder_huff_can(){
		assert(Lfactor>0 && Lfactor<1);
		uint minLen,maxLen;
		minLen=INT_MAX;
		maxLen=0;
		for (uint i=0;i<max_v+1; i++){
			uint dummy;
			uint len = wc->getCode(i, dummy);
			minLen=min(minLen,len);
			maxLen=max(maxLen,len);
		}
		uint Lprime = (uint)minLen+(maxLen-minLen+1)*Lfactor;
		build(wc,max_v,Lprime);
		sigma = max_v+1;	
	}
	
    wt_coder_Balanced::wt_coder_Balanced(const Array & a, Mapper * am) {
		cerr << "Not implemented yet" << endl;
    }
	//returns the length of the code and stores in code the code of symbol
	uint wt_coder_Balanced::getCode(uint symbol, uint &code) const{
	  uint len,leaf;
	  	model->getCode(symbol, code, len, leaf);
	 return len;
	}

    wt_coder_Balanced::wt_coder_Balanced(uint * symbs, size_t n, Mapper * am) {
		cerr << "Not implemented yet" << endl;
    }

    wt_coder_Balanced::wt_coder_Balanced(uchar * symbs, size_t n, Mapper * am) {
		cerr << "Not implemented yet" << endl;
    }

    wt_coder_Balanced::wt_coder_Balanced() {}

    wt_coder_Balanced::~wt_coder_Balanced() {
    }

    uint wt_coder_Balanced::getSymbol(uint code) const{
        return model->getSymbol(code);
    }

    uint wt_coder_Balanced::getSymbol(uint code, uint len) const{
        return model->getSymbol(code, len);
    }

	uint * wt_coder_Balanced::get_symbol(uint symbol) const {
		cerr << "get_symbol not implemented yet (suboptimal add)" << endl;
		return nullptr;
	}

    bool wt_coder_Balanced::is_set(uint symbol, uint l) const
    {
		uint code;
		code = get_field(symbol2code,maxCodeLength,symbol); 		
		return (code>>l)&1;
    }

    bool wt_coder_Balanced::is_set(uint *symbol, uint l) const {
		
        return ((*symbol)>>l)&1;
    }

    bool wt_coder_Balanced::done(uint symbol, uint l) const
    {
		cerr << "not implemented yet"<<endl;
		return false;
    }

    size_t wt_coder_Balanced::getSize() const
    {
		return sizeof(wt_coder_Balanced)+model->getSize();
    }

    void wt_coder_Balanced::save(ofstream & fp) const
    {
		
        uint wr = WT_CODER_HUFF_CAN_HDR;
        saveValue(fp,wr);
        saveValue(fp,table_len);
        saveValue(fp,maxCodeLength);
        saveValue(fp,minCodeLength);
        saveValue(fp,nCodesAtLevel,maxCodeLength+1);
        saveValue(fp,symbol2code,table_len);
    }

    wt_coder_Balanced * wt_coder_Balanced::load(ifstream & fp) {
        uint rd = loadValue<uint>(fp);
        if(rd!=WT_CODER_HUFF_CAN_HDR) return NULL;
        wt_coder_Balanced * ret = new wt_coder_Balanced();
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

    bool wt_coder_Balanced::testCodeDecode() const{

    	for (uint i=0;i<sigma;i++){
    		uint code,len,leaf;
    		model->getCode(table_symbols_codes[i].symbol, code, len, leaf);
    		assert(code==table_symbols_codes[i].code);
    		if (!(len==table_symbols_codes[i].len)){
    			model->getCode(table_symbols_codes[i].symbol, code, len, leaf);
    		}
    		assert(len==table_symbols_codes[i].len);
    		code=0;len=0;
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
