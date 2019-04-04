/* wt_coder_SubOptimalAdd.h
 * Copyright (C) 2014-current_year, Alberto Ordóñez, all rights reserved.
 *
 * Builds a suboptimal code limiting the height of longest code to L. To do 
 * so we follow the idea of Milidiú & Laber [Algorithmica(2001)31: 513-529]. 
 * Besides, the returned set of codes is not optimal but it is cannonical, and 
 * all the symbols at each level of the tree of codes are sorted in increasing order. 
 * Thus, if codeLen(symbol_i)=codeLen(symbol_j), then if (symbol_i<symbol_j) then 
 * code(symbol_i)<code(symbol_j), the opposite otherwise. 
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

#include <sequence/wt_coder_SubOptimalAdd.h>
#include <climits>

namespace cds_static
{
	
	
	void wt_coder_SubOptimalAdd::build(wt_coder_huff *wc, uint max_v, uint L){
		
		table_symbols_codes = new tuple[max_v+1];
		maxCodeLength = 0;
		minCodeLength = ~0;
		sigma = max_v+1;
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
			cerr << "L(" << L << ") == maxCodeLength("<<maxCodeLength << ")."<<endl;
			abort();
			return;
		}
		if (L>maxCodeLength){
			cerr << "L(" << L << ") > maxCodeLength("<<maxCodeLength << ")."<<endl;
			abort();
			return;
		}
		if (L<=minCodeLength){
			cerr << "L(" << L << ") <= minCodeLength("<<minCodeLength << ")."<<endl;
			abort();
			return;
		}
		{
			uint countNCodes = 0;
			for (uint i=minCodeLength;i<=maxCodeLength;i++)
				countNCodes+=nCodesAtLevel[i];
			assert(countNCodes==max_v+1);
		}
		
		//WHY do i increment L? Originally, the algorithm was programed in such a way that 
		//all the codes underneath L (inclusive) were removed to create a balanced binary 
		//tree (and to fill up the gaps that appeared at level L-1). However, what we really want 
		//is to limit the maximum height to L, thus, we need to remove everything that is
		//strictly underneath L. To force this without changing the algorithms, we 
		//just increment L. 
		//L++;

		//removing the symbols at level L yields holesAtminus1 empty leaves
		uint holesAtLminus1 = (((1<<(L+1))-firstCodeAtLevel[L+1])>>1) + (((1<<(L+1))-firstCodeAtLevel[L+1])&1);
		//totalUnderL is the number of codes we have to rebalance
		uint totalUnderL=0;
		for (uint i=L+1;i<=maxCodeLength;i++)
			totalUnderL+=nCodesAtLevel[i];
		uint tmp =0;
		for (uint i=0;i<=max_v;i++){
			if (table_symbols_codes[i].len>L)tmp++;
		}
		assert(tmp==totalUnderL);
		assert(totalUnderL>0);

		// uint nCodesStartWith0 = 0;
		// uint maxLevelStartingWith0=0;
		// for (uint i=0;i<=max_v;i++){
		// 	if (table_symbols_codes[i].len<L && (!bitget(&(table_symbols_codes[i].code),table_symbols_codes[i].len-1))) { 
		// 		nCodesStartWith0++;	
		// 		maxLevelStartingWith0=max(maxLevelStartingWith0,table_symbols_codes[i].len);			
		// 	}
		// }
		// cerr << "nCodesStartWith0: " << nCodesStartWith0 << endl;
		// cerr << "maxLevelStartingWith0: " << maxLevelStartingWith0-1 << endl;

		//L--;
		//put the symbols of that codes in a vector. Note we have two vectors: 
		//symbolsForHoles stores the symbols we will place at the leaves left at L-1
		//rebalanceSymbols stores the symbols to convert in a balanced tree
		uint *posSymbolsForHoles = new uint[holesAtLminus1];
		uint *posRebalancedSymbols = new uint[totalUnderL];
		uint forHoles,forRebalance;
		forHoles=forRebalance=0;
		//first try to fill those holes at L-1 with symbols from the deepest levels
		uint acumForHoles=0;
		uint minLenForHoles=maxCodeLength;
		for (int i=maxCodeLength;i>=0;i--){
			acumForHoles+=nCodesAtLevel[i];
			minLenForHoles=i;
			if (acumForHoles>=holesAtLminus1)
				break;
		}

		for (uint i=0;i<=max_v;i++){
			if (table_symbols_codes[i].len>=minLenForHoles){
				if (forHoles<holesAtLminus1){
					posSymbolsForHoles[forHoles++]=i;
					table_symbols_codes[i].len=L;
				}else{
					break;
				}
			}
		}


		for (uint i=0;i<=max_v;i++){
			if (table_symbols_codes[i].len>L){
				if (forHoles<holesAtLminus1){
					posSymbolsForHoles[forHoles++]=i;
					table_symbols_codes[i].len=L;
				}else{
					posRebalancedSymbols[forRebalance++]=i;
					//table_symbols_codes[i].len = 1;
				}
			}
		}

		assert(forHoles==holesAtLminus1);
		
		assert((forHoles+forRebalance)==(totalUnderL));
		
		if (2+bits(forRebalance)>L && bits(sigma)<=L){
			uint *newSymb = new uint[sigma];
			for (uint i=0;i<sigma;i++){
				newSymb[i]=i;
				table_symbols_codes[i].len=0;
			}
			recAsignCodes(newSymb, sigma);
			delete [] newSymb;
			minCodeLength=INT_MAX;
			maxCodeLength=0;
			for (uint i=0;i<=max_v;i++){
				minCodeLength=min(minCodeLength,table_symbols_codes[i].len);
				maxCodeLength=max(maxCodeLength,table_symbols_codes[i].len);
			}
		}else{
			maxCodeLength=0;
			for (uint i=0;i<=max_v;i++){
				minCodeLength=min(minCodeLength,table_symbols_codes[i].len);
				maxCodeLength=max(maxCodeLength,table_symbols_codes[i].len);
			}
			//increase the code length of those codes that:
			//(1)starts with a 0 (left branch of the root);
			//(2) have length < L. 
			for (uint i=0;i<=max_v;i++){
				if (table_symbols_codes[i].len<L && (!bitget(&(table_symbols_codes[i].code),table_symbols_codes[i].len-1))) { //(!(table_symbols_codes[i].code>>(table_symbols_codes[i].len-1)))){
					table_symbols_codes[i].len++;				
				}
			}
			
			//cerr << "bits for rebalanced =(2+bits(nsymb)) = "<<2+bits(totalUnderL-holesAtLminus1-1) << endl;
			rebuildBalanced(posSymbolsForHoles,posRebalancedSymbols,forRebalance,holesAtLminus1,L);

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
		}
		//assert(maxCodeLength==L);
		//build again the canonical code but with the new set of lengths
		buildCanonical(max_v,minCodeLength,maxCodeLength);
		

		// for (uint i=0;i<=maxCodeLength;i++){
  //     		cout << "nCodes[" << i << "]: " << nCodesAtLevel[i] << endl;
  //     	}
		//Done.
	}
		
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
	wt_coder_SubOptimalAdd::wt_coder_SubOptimalAdd(wt_coder_huff *wc, uint max_v, uint L, MappingBuilder *ms):wt_coder_huff_can(){
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

	wt_coder_SubOptimalAdd::wt_coder_SubOptimalAdd(wt_coder_huff *wc, uint max_v, uint L, BitSequenceBuilder *bsb):wt_coder_huff_can(){
		build(wc,max_v,L);
		sigma = max_v+1;
		assert(testCanonicalSorted());
		model = new MappingPermutations(table_symbols_codes, sigma, bsb);
		assert(testCodeDecode());
	}
	/**
	 * L = (maxCodeLength-minCodeLength+1)*Lfactor.
	 * */
	wt_coder_SubOptimalAdd::wt_coder_SubOptimalAdd(wt_coder_huff *wc, uint max_v, float Lfactor):wt_coder_huff_can(){
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
	
	void wt_coder_SubOptimalAdd::rebuildBalanced2(uint *posSymbolsHoles, uint *posSymbolsRebalance, uint n,uint nholes, uint L){
		
		uint lenCode = bits(n)-1;
		uint nsymbPrevL = (1<<lenCode);
		uint nsymb_nbits,nsymb_nbits_plus1;
		
		nsymb_nbits = nsymbPrevL - (n-nsymbPrevL);
		nsymb_nbits_plus1 = (n-nsymbPrevL)*2;
		assert(nsymb_nbits+nsymb_nbits_plus1==n);
		//+2 because we have to insert the balance tree below the right child of root's left branch
		//This adds 2 heights
		lenCode+=2;
		uint i;
		for (i=0;i<nsymb_nbits;i++){
			table_symbols_codes[posSymbolsRebalance[i]].len = lenCode+2;
		}
		uint j;
		lenCode++;
		for (j=0;j<nsymb_nbits_plus1;j++,i++){
			table_symbols_codes[posSymbolsRebalance[i]].len = lenCode+2;
		}
		L--;
		for (i=0;i<nholes;i++){
			table_symbols_codes[posSymbolsHoles[i]].len= L;
		}
	}
	
	void wt_coder_SubOptimalAdd::rebuildBalanced(uint *posSymbolsHoles, uint *posSymbolsRebalance, uint n,uint nholes, uint L){
		
		//fill those holes at L with symbols selected for that purpose
		for (uint i=0;i<nholes;i++){
			table_symbols_codes[posSymbolsHoles[i]].len= L;
		}
		//+2 because we have to insert the balance tree below the right child of root's left branch
		//This adds 2 heights
		uint depth=0;
		for (uint i=0;i<n;i++){
			table_symbols_codes[posSymbolsRebalance[i]].len=2;
		}
		uint total = recAsignCodes(posSymbolsRebalance,n);
		assert(total==n);
	}

	uint wt_coder_SubOptimalAdd::recAsignCodes(uint *posSymbolsRebalance, uint n){
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

    wt_coder_SubOptimalAdd::wt_coder_SubOptimalAdd(const Array & a, Mapper * am) {
		cerr << "Not implemented yet" << endl;
    }
	//returns the length of the code and stores in code the code of symbol
	uint wt_coder_SubOptimalAdd::getCode(uint symbol, uint &code) const{
	  uint len,leaf;
	  	model->getCode(symbol, code, len, leaf);
	 return len;
	}

    wt_coder_SubOptimalAdd::wt_coder_SubOptimalAdd(uint * symbs, size_t n, Mapper * am) {
		cerr << "Not implemented yet" << endl;
    }

    wt_coder_SubOptimalAdd::wt_coder_SubOptimalAdd(uchar * symbs, size_t n, Mapper * am) {
		cerr << "Not implemented yet" << endl;
    }

    wt_coder_SubOptimalAdd::wt_coder_SubOptimalAdd() {}

    wt_coder_SubOptimalAdd::~wt_coder_SubOptimalAdd() {
    }

	uint wt_coder_SubOptimalAdd::getSymbols(uint code, uint &n_symbol, vector<uint> &symbols) const{
		return model->getSymbols(code,n_symbol,symbols);
	}

    uint wt_coder_SubOptimalAdd::getSymbol(uint code) const{
        return model->getSymbol(code);
    }

    uint wt_coder_SubOptimalAdd::getSymbol(uint code, uint len) const{
        return model->getSymbol(code, len);
    }

	uint * wt_coder_SubOptimalAdd::get_symbol(uint symbol) const {
		cerr << "get_symbol not implemented yet (suboptimal add)" << endl;
		return nullptr;
	}

    bool wt_coder_SubOptimalAdd::is_set(uint symbol, uint l) const
    {
		uint code;
		code = get_field(symbol2code,maxCodeLength,symbol); 		
		return (code>>l)&1;
    }

    bool wt_coder_SubOptimalAdd::is_set(uint *symbol, uint l) const {
		
        return ((*symbol)>>l)&1;
    }

    bool wt_coder_SubOptimalAdd::done(uint symbol, uint l) const
    {
		cerr << "not implemented yet"<<endl;
		return false;
    }

    size_t wt_coder_SubOptimalAdd::getSize() const
    {
		return sizeof(wt_coder_SubOptimalAdd)+model->getSize();
    }

    void wt_coder_SubOptimalAdd::save(ofstream & fp) const
    {
		
        uint wr = WT_CODER_HUFF_CAN_HDR;
        saveValue(fp,wr);
        saveValue(fp,table_len);
        saveValue(fp,maxCodeLength);
        saveValue(fp,minCodeLength);
        saveValue(fp,nCodesAtLevel,maxCodeLength+1);
        saveValue(fp,symbol2code,table_len);
    }

    wt_coder_SubOptimalAdd * wt_coder_SubOptimalAdd::load(ifstream & fp) {
        uint rd = loadValue<uint>(fp);
        if(rd!=WT_CODER_HUFF_CAN_HDR) return NULL;
        wt_coder_SubOptimalAdd * ret = new wt_coder_SubOptimalAdd();
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

    bool wt_coder_SubOptimalAdd::testCodeDecode() const{

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
