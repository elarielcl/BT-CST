/* wt_coder_SubOptimalMult.h
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

#include <sequence/wt_coder_SubOptimalMult.h>
#include <climits>
#include <stdexcept>

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
	wt_coder_SubOptimalMult::wt_coder_SubOptimalMult(wt_coder_huff_can *wc, uint max_v, float c, uint _lmax):wt_coder(){
		lmax = _lmax;
		nsymbols = max_v+1;
		sigma = max_v+1;
		uint maxLen = 0;
		firstLevelWithCodes = INT_MAX;
		for (int i=0;i<max_v;i++){
			//canonical and symbols with codes of the same level sorted in increasing order
			assert(wc->table_symbols_codes[i].len!=wc->table_symbols_codes[i+1].len || 
				(wc->table_symbols_codes[i].code+1==wc->table_symbols_codes[i+1].code && 
					wc->table_symbols_codes[i].symbol<wc->table_symbols_codes[i+1].symbol));
			firstLevelWithCodes = min(firstLevelWithCodes,(int)wc->table_symbols_codes[i].len);
			maxLen = max(maxLen,wc->table_symbols_codes[i].len);
		}
		maxLen = max(maxLen,wc->table_symbols_codes[max_v].len);
		levelFreq = static_cast<uint>(maxLen/c+2);
		//Fills and builds the hash table with the info relative to frequent symbols
		fillHash(wc, max_v);
		buildUnfreq(wc, max_v);
		assert(testEncode(wc));
		assert(testDecode(wc));

	}
	
	uint wt_coder_SubOptimalMult::fillHash(wt_coder_huff_can *wc, uint max_v){
		//count how many short symbols we have
		uint nShort=0;
		for (int i=0;i<=max_v;i++){
			if (wc->table_symbols_codes[i].len<=levelFreq){
				nShort++;
			}
		}
		sR  = new uint[levelFreq+1];
		fst = new uint[levelFreq+1];
		nCodes = new uint[levelFreq+1];
		for (int i=0;i<=levelFreq;i++){
			nCodes[i]=sR[i]=fst[i]=0;
		}

		uint *symbolsShort = new uint[nShort];
		//we add those short symbols to the hashTable. In such a table we store the symbol id plus the code associated
		//with it as satellite information.
		hashFreq = new HashTable<int>(5,7,nShort*2);
		nShort=0;
		for (int i=0;i<=max_v;i++){
			if (wc->table_symbols_codes[i].len<=levelFreq){
				nCodes[wc->table_symbols_codes[i].len]++;

				if (sR[wc->table_symbols_codes[i].len]==0){
					//we store the first code of each length
					fst[wc->table_symbols_codes[i].len]=wc->table_symbols_codes[i].code;
				}	
				//we count how many codes of each length exist
				sR[wc->table_symbols_codes[i].len]++;

				uint ret = hashFreq->insert(wc->table_symbols_codes[i].symbol,wc->table_symbols_codes[i].code);
				assert(ret!=-1);
				symbolsShort[nShort++]=wc->table_symbols_codes[i].symbol;

			}
		}

		uint acum = sR[0];
		for (int i=1;i<=levelFreq;i++){
			uint tmp = sR[i];
			sR[i]=acum;
			acum+=tmp;
		}

		//builds inverseHash (ihash)
		ihash = new Array(symbolsShort,nShort);
		
		delete [] symbolsShort;
		//assert frequent symbols ok
		assert(testHashFreqSymbols(wc, max_v, nShort));

		return nShort;

	}

	uint wt_coder_SubOptimalMult::buildUnfreq(wt_coder_huff_can *wc, uint max_v){
		for (uint i = 0;i<=max_v;i++){
			if (wc->table_symbols_codes[i].len>levelFreq){
				baseCodeUnFreq = wc->table_symbols_codes[i-1].code;
				break;
			}
		}
		baseCodeUnFreq++;
		assert(bits(baseCodeUnFreq)<=levelFreq+1);
		baseCodeUnFreq = baseCodeUnFreq << (lmax+1-levelFreq);
		assert(bits(baseCodeUnFreq)<=(lmax+1));
		for (uint i=0;i<=max_v;i++){
			if (wc->table_symbols_codes[i].len<=levelFreq) continue;
			wc->table_symbols_codes[i].len = lmax+1;
			wc->table_symbols_codes[i].code = baseCodeUnFreq+wc->table_symbols_codes[i].symbol;
			assert(bits(wc->table_symbols_codes[i].code)<=lmax+1);
		}

		return 0;
	}

	bool wt_coder_SubOptimalMult::testHashFreqSymbols(wt_coder_huff_can *wc, uint max_v, uint nShort){
		//direct access to the hash (from symbol to code)
		for (int i=0;i<=max_v;i++){
			if (wc->table_symbols_codes[i].len<=levelFreq){
				uint v = hashFreq->search(wc->table_symbols_codes[i].symbol);
				assert(v!=-1);
				assert((*hashFreq)[v]==wc->table_symbols_codes[i].code);
			}
		}
		//check sR vector is OK.
		for (int i=0;i<max_v;i++){
			if (wc->table_symbols_codes[i+1].len>levelFreq)
				break;
			if (wc->table_symbols_codes[i].len != wc->table_symbols_codes[i+1].len){
				assert(sR[wc->table_symbols_codes[i+1].len]==i+1);
			}
		}
		//inverse access (from code to symbol)
		for (int i=0;i<=max_v;i++){
			if (wc->table_symbols_codes[i].len<=levelFreq){
				uint code = (*hashFreq)[hashFreq->search(wc->table_symbols_codes[i].symbol)];
				uint len=-1;
				for (int j=0;j<=levelFreq;j++){
					if (fst[j]>code){
						len = j-1;
						break;
					}
				}
				if (len==-1)
					len = levelFreq;
				//we algo to obtain the length of a code is ok
				for(int j=0;j<=max_v;j++){
					if (wc->table_symbols_codes[i].code==code){
						assert(len==wc->table_symbols_codes[i].len);
						break;
					}
				}
				//check wether the retrieved symbol from ihash is ok
				assert(code>=fst[len]);
				uint symbol = (*ihash)[sR[len]+(code-fst[len])];
				for (int j=0;j<nShort;j++){
					if (wc->table_symbols_codes[j].code == code){
						assert(symbol == wc->table_symbols_codes[j].symbol);
						break;
					}
				}
			}
		}

		return true;
	}


	bool wt_coder_SubOptimalMult::testEncode(wt_coder_huff_can *wc){
		uint maxLen=0;
		for (uint i=0;i<nsymbols;i++){
			uint code; 
			uint len; 
			len = getCode(wc->table_symbols_codes[i].symbol, code);
			assert(bits(code)<=lmax+1);
			maxLen=max(maxLen,len);
			if (!(len==wc->table_symbols_codes[i].len)){
				len = getCode(wc->table_symbols_codes[i].symbol, code);
			}
			assert(len==wc->table_symbols_codes[i].len);
			assert(code==wc->table_symbols_codes[i].code);
		}
		assert(maxLen==lmax+1);
		return true;
	}

	bool wt_coder_SubOptimalMult::testDecode(wt_coder_huff_can *wc){
		for (uint i=0;i<nsymbols;i++){
			uint symbol; 
			symbol = getSymbol(wc->table_symbols_codes[i].code, wc->table_symbols_codes[i].len);
			assert(symbol==wc->table_symbols_codes[i].symbol);
		}
		return true;
	}
    wt_coder_SubOptimalMult::wt_coder_SubOptimalMult() {}

    wt_coder_SubOptimalMult::~wt_coder_SubOptimalMult() {
    }

	uint wt_coder_SubOptimalMult::getCode(uint symbol, uint &code) const{
		uint v;
		uint len;
		if ((v=hashFreq->search(symbol))!=-1){
			code = (*hashFreq)[v];
			len=-1;
				for (int j=firstLevelWithCodes;j<=levelFreq;j++){
					if (code<(fst[j]+nCodes[j])){
						len = j;
						break;
					}
				}
				if (len==-1) len = levelFreq;
				
		}else{
			len = lmax+1;
			code = symbol + baseCodeUnFreq;
			assert(bits(code)<=len);
		}
		return len; 
	}

	uint wt_coder_SubOptimalMult::getSymbol(uint code, uint len) const{
		
		if (len==lmax+1){
			return code-baseCodeUnFreq;
		}else{
			uint len=firstLevelWithCodes;
			for (int j=firstLevelWithCodes;j<=levelFreq;j++){
				if (code<(fst[j]+nCodes[j])){
						len = j;
					assert(code>=fst[len]);
					return (*ihash)[sR[len]+(code-fst[len])];
				}
			}
			throw runtime_error("symbol not found at wt_coder_SubOptimalMult::getSymbol(code,len)");
		}
	}

	uint wt_coder_SubOptimalMult::getSymbols(uint code_rev, uint &n_symbol, vector<uint> &symbols) const{
		uint len=0;
		uint code= invertWord(code_rev)>>(W-firstLevelWithCodes);
		for (int j=firstLevelWithCodes;j<=levelFreq;j++){
			if (code<(fst[j]+nCodes[j])){
				len = j;
				assert(code>=fst[len]);
				symbols[0] = (*ihash)[sR[len]+(code-fst[len])];
				n_symbol = 1;
				return len;
			}
			code = (code<<1)|((code_rev>>j)&1);
		}
		symbols[0] = code-baseCodeUnFreq;
		n_symbol = 1;
		return lmax+1;
	}

	uint wt_coder_SubOptimalMult::getSymbol(uint code) const{
		uint len=0;
			for (int j=firstLevelWithCodes;j<=levelFreq;j++){
				if (code<(fst[j]+nCodes[j])){
						len = j;
						assert(code>=fst[len]);
					return (*ihash)[sR[len]+(code-fst[len])];
				}
			}
			return code-baseCodeUnFreq;
	}

	uint * wt_coder_SubOptimalMult::get_symbol(uint symbol) const {
		throw runtime_error("wt_coder_SubOptimalMult::get_symbol Not implemented yet");
	}

    bool wt_coder_SubOptimalMult::is_set(uint symbol, uint l) const
    {
	throw runtime_error("wt_coder_SubOptimalMult::is_set Not implemented yet");
	}

    bool wt_coder_SubOptimalMult::is_set(uint *symbol, uint l) const {
		
        throw runtime_error("wt_coder_SubOptimalMult::is_set Not implemented yet");
    }

    bool wt_coder_SubOptimalMult::done(uint symbol, uint l) const
    {
		throw runtime_error("wt_coder_SubOptimalMult::done Not implemented yet");
    }

    size_t wt_coder_SubOptimalMult::getSize() const
    {
		return hashFreq->getSize()+ihash->getSize()+sizeof(wt_coder_SubOptimalMult);
    }

    void wt_coder_SubOptimalMult::save(ofstream & fp) const
    {
		throw runtime_error("wt_coder_SubOptimalMult::save Not implemented yet");
    }

    wt_coder_SubOptimalMult * wt_coder_SubOptimalMult::load(ifstream & fp) {
		throw runtime_error("wt_coder_SubOptimalMult::load Not implemented yet");
	}

};
