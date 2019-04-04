/* WaveletMatrixHuff.cpp
 * Copyright (C) 2012, Alberto Ordóñez, all rights reserved.
 *
 * Huffman shaped Wavelet Matrix
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

#include <sequence/WaveletMatrixHuff.h>

namespace cds_static
{
	
	typedef struct p{
		uint code;
		uint symbol;
		uint len;
	}pairsc;
	
	int cmpPairs(const void *s1, const void *s2){
		pairsc *sc1,*sc2;
		sc1=((pairsc *)(s1));
		sc2=((pairsc *)(s2));
		
		if (sc1->len == sc2->len){
			if (bitget(&(sc1->code),0)==bitget(&(sc2->code),0)){
			uint min_l=min(sc1->len,sc2->len)-1;
			uint i=0;
			while(i<min_l && bitget(&(sc1->code),i)==bitget(&(sc2->code),i))i++;
				return bitget(&(sc1->code),i);
			}else{
				return bitget(&(sc1->code),0);
			}
		}
			return sc1->len - sc2->len;
	}

	void updateTableCodigos(uint **tableCodigos1, uint **tableCodigos2, uint *maxCodes, uint *nCodesAtLevel, uint maxLength, uint minLength, uint level, uint group){
		int i;
		for (i=(int)group;i<=(int)maxLength;i++){
			if (nCodesAtLevel[i]==0){ 
				tableCodigos2[level][i]--;
				tableCodigos2[level][i+1]++;
				continue;
			}
			if (tableCodigos2[level][i]==maxCodes[i]){
				tableCodigos2[level][i+1]++;
			}else{
				break;
			}
		}
		int change = --i;
		//~ if (change>=minLength){debug++; cout << change << ", #: " << cuantos << endl;cuantos=0;}
		for (int j=i;j>=(int)minLength;j--){
			if (nCodesAtLevel[j]==0) continue;
			tableCodigos2[level][j]=tableCodigos1[change][j];
		}
	}


	uint buildCodigos(uint minLength, uint maxLength, uint *nCodesAtLevel, uint **codes, uint **invNegCodes, uint **bmpCodes, uint **lastCode){
		uint ncodes = 0;
		(*bmpCodes) = new uint[maxLength+1];
		
		(*bmpCodes) = new uint[((1<<maxLength)+W-1)/W];
		for (uint i=0;i<((1<<maxLength)+W-1)/W;i++) (*bmpCodes)[i]=0;
		
		
		uint **tableCodes = new uint*[maxLength+2];
		uint **tableCodes2 = new uint*[maxLength+2];
		for (uint i=0;i<=maxLength+1;i++){
			tableCodes[i]=new uint[maxLength+2];
			tableCodes2[i]=new uint[maxLength+2];
		}
		for (uint i=0;i<=maxLength+1;i++){
			for (uint j=0;j<=maxLength;j++){
				tableCodes[i][j]=0;
				tableCodes2[i][j]=0;
			}
		}
		
		(*lastCode) = new uint[maxLength+1];
		uint *maxCode;
		maxCode = new uint[maxLength+1];
		uint from=0;
		for (uint i=0;i<=maxLength;i++){
			if (nCodesAtLevel[i]!=0){
				maxCode[i] = (1<<(i-from));
				from=i;
			}
		}

		for (uint i=minLength;i<=maxLength;i++){ncodes+=nCodesAtLevel[i];}

		(*codes) = new uint[ncodes]; 
		(*invNegCodes) = new uint[ncodes]; 
		uint code;
		uint pcodes=0;
		
		for (uint len = minLength; len <= maxLength; len++){	
			//~ cout << "len: " << len << ", nCodes: " << nCodesAtLevel[len]<< endl;
			for (uint i=0;i<nCodesAtLevel[len];i++){
				uint j=0;
				code=0;
				for (uint k=minLength;k<=len;k++){
					if (nCodesAtLevel[k]==0) continue;
					assert(tableCodes2[len][k]>=tableCodes[k][k]);
					code = code | (tableCodes2[len][k]<<j);
					j=k;
				}
				//cuantos++;
				
				bitset((*bmpCodes),code);
				
				(*codes)[pcodes] = ~(invertWord(code) >> (32 -len)) ;
				(*invNegCodes)[pcodes] = code;
				if (pcodes>1){
					assert((*invNegCodes)[pcodes]>(*invNegCodes)[pcodes-1]);
				}
				pcodes++;
				tableCodes2[len][minLength]++;
				updateTableCodigos(tableCodes,tableCodes2,maxCode,nCodesAtLevel,maxLength,minLength,len,minLength);
			}
				
				(*lastCode)[len]=(*invNegCodes)[pcodes-1];
				
			for (uint i=minLength;i<=len;i++){
				tableCodes[len][i] = tableCodes2[len][i];
				tableCodes2[len+1][i] = tableCodes2[len][i];
			}
		}
		for (uint i=0;i<=maxLength+1;i++){
			delete [] tableCodes[i];
			delete [] tableCodes2[i];
		}
		delete [] tableCodes;
		delete [] tableCodes2;
		delete [] maxCode;
		assert(pcodes==ncodes);
		return pcodes; 
	}
	uint buildCodigos2(uint minLength, uint maxLength, uint *nCodesAtLevel, uint **codes, uint **invNegCodes, uint **bmpCodes, uint **lastCode){
		uint ncodes = 0;
		(*bmpCodes) = new uint[maxLength+1];
		
		(*bmpCodes) = new uint[((1<<maxLength)+W-1)/W];
		for (uint i=0;i<((1<<maxLength)+W-1)/W;i++) (*bmpCodes)[i]=0;
		
		
		uint **tableCodes = new uint*[maxLength+2];
		uint **tableCodes2 = new uint*[maxLength+2];
		for (uint i=0;i<=maxLength+1;i++){
			tableCodes[i]=new uint[maxLength+2];
			tableCodes2[i]=new uint[maxLength+2];
		}
		for (uint i=0;i<=maxLength+1;i++){
			for (uint j=0;j<=maxLength;j++){
				tableCodes[i][j]=0;
				tableCodes2[i][j]=0;
			}
		}
		
		(*lastCode) = new uint[maxLength+1];
		uint *maxCode;
		maxCode = new uint[maxLength+1];
		uint from=0;
		for (uint i=0;i<=maxLength;i++){
			if (nCodesAtLevel[i]!=0){
				maxCode[i] = (1<<(i-from));
				from=i;
			}
		}

		for (uint i=minLength;i<=maxLength;i++){ncodes+=nCodesAtLevel[i];}

		(*codes) = new uint[ncodes]; 
		(*invNegCodes) = new uint[ncodes]; 
		uint code;
		uint pcodes=0;
		
		for (uint len = minLength; len <= maxLength; len++){	
			//~ cout << "len: " << len << ", nCodes: " << nCodesAtLevel[len]<< endl;
			for (uint i=0;i<nCodesAtLevel[len];i++){
				uint j=0;
				code=0;
				for (uint k=minLength;k<=len;k++){
					if (nCodesAtLevel[k]==0) continue;
					assert(tableCodes2[len][k]>=tableCodes[k][k]);
					code = code | (tableCodes2[len][k]<<j);
					j=k;
				}
				//cuantos++;
				
				bitset((*bmpCodes),code);
				
				(*codes)[pcodes] = ~(invertWord(code) >> (32 -len)) ;
				(*invNegCodes)[pcodes] = code;
				if (pcodes>1){
					assert((*invNegCodes)[pcodes]>(*invNegCodes)[pcodes-1]);
				}
				pcodes++;
				tableCodes2[len][minLength]++;
				updateTableCodigos(tableCodes,tableCodes2,maxCode,nCodesAtLevel,maxLength,minLength,len,minLength);
			}
				
				(*lastCode)[len]=(*invNegCodes)[pcodes-1];
				
			for (uint i=minLength;i<=len;i++){
				tableCodes[len][i] = tableCodes2[len][i];
				tableCodes2[len+1][i] = tableCodes2[len][i];
			}
		}
		for (uint i=0;i<=maxLength+1;i++){
			delete [] tableCodes[i];
			delete [] tableCodes2[i];
		}
		delete [] tableCodes;
		delete [] tableCodes2;
		delete [] maxCode;
		assert(pcodes==ncodes);
		return pcodes; 
	}
	
	WaveletMatrixHuff::WaveletMatrixHuff(const Array &symbols2,  wt_coder_huff * coder, BitSequenceBuilder * bmb, Mapper * am) : Sequence(n) {
		cout << "Not implemented yet!" << endl;
		
		//~ uint *symbols = new uint[symbols2->getLength()];
		//~ for (long int i=0;i<symbols2->getLength();i++)
			//~ symbols[i]=symbols2->getField(i);
		//~ 
		bmb->use();
		n = symbols2.getLength();
		uint *symbols = new uint[n];
		this->am = am;
		am->use();
		for(uint i = 0; i < n; i++)
			symbols[i] = am->map(symbols2.getField(i));
		max_v = max_value(symbols, n);
		height = bits(max_v);

		OCC = new uint[max_v + 2];
		for (uint i = 0; i <= max_v + 1; i++)
			OCC[i] = 0;
		for (uint i = 0; i < n; i++)
			OCC[symbols[i] + 1]++;

		uint to_add = 0;
		for (uint i = 1; i <= max_v + 1; i++)
			if (OCC[i] == 0) to_add++;

		uint * new_symb = new uint[n + to_add];
		for (uint i = 0; i < n; i++)
			new_symb[i] = symbols[i];
		delete [] symbols;

		to_add = 0;
		for (uint i = 1; i <= max_v + 1; i++)
		if (OCC[i] == 0) {
			OCC[i]++;
			new_symb[n + to_add] = i - 1;
			to_add++;
		}

		uint new_n = n + to_add;
		for(uint i = 1;i <= max_v + 1; i++)
			OCC[i] += OCC[i - 1];
		this->n = new_n;

		uint **_bm=new uint*[height];
		for(uint i = 0; i < height; i++) {
			_bm[i] = new uint[new_n / W + 1];
			for(uint j = 0;j < new_n / W + 1; j++)
				_bm[i][j] = 0;
		}

		build_level(_bm, new_symb, new_n, 0, NULL, NULL,NULL,NULL,NULL);
		bitstring = new BitSequence*[height];
		C = new uint[height];
		for(uint i = 0; i < height; i++) {
			bitstring[i] = bmb->build(_bm[i], new_n);
			C[i] = bitstring[i]->rank0(new_n - 1);
			delete [] _bm[i];
		}
		delete [] _bm;
		// delete [] oc;
		bmb->unuse();

		this->length = n;
	}
	WaveletMatrixHuff::WaveletMatrixHuff(uint * symbols, size_t n, wt_coder_huff * coder, BitSequenceBuilder ** bmb, uint nbmb, Mapper * am, MappingWMBuilder *wmb, bool deleteSymbols) : Sequence(n) {
		for (uint i=0;i<nbmb;i++)
			bmb[i]->use();
		
		this->length=n;
		this->n=n;
		this->am=am;
		am->use();
		for(uint i=0;i<n;i++)
			symbols[i] = am->map(symbols[i]);
		max_v=max_value(symbols,n);		
		cout << "n: " << n << endl;
		cout << "sigma: " << max_v << endl; 
		
		OCC = new uint[max_v + 1];

		uint *new_symb = new uint[this->n];
		//Apply a cont mapper to symbols to access values and so on. If we know that symbols 
		//are consecutives, we do not need this. 
		BitSequenceBuilder *bsb = new BitSequenceBuilderRRR(4);
		MapperCont *mc = new MapperCont(symbols, n, *bsb);
		for (uint i=0;i<this->n;i++) new_symb[i]=mc->map(symbols[i])-1;
		delete mc; 
		uint max_v_cont = max_value(new_symb,this->n);
		
		if (deleteSymbols){
			delete [] symbols; 
			symbols=0;
		}
		//stores the length of each code
		uint *codeLen = new uint[max_v_cont + 1];
		uint *codes;
		minLength=~0;
		
		//we fill codeLen and obtain min and max code lengths. MaxCode length is height
		height=0;
		for (uint i=0;i<=max_v_cont;i++){
			uint c; 
			codeLen[i]=coder->getCode(i,c);
			if (codeLen[i]>height) height=codeLen[i];
			if (codeLen[i]<minLength) minLength=codeLen[i];
		}
		cout << "Maximum length of a code: " << height << endl;
		cout << "Minimum length of a code: " << minLength << endl; 
		
		/** number of distinct symbols that finish at each level **/
		uint *diffSymbolsAt = new uint[height+1];
		uint *nCodesAtLevel = new uint[height+1];
		for (uint i=0;i<=height;i++){
			diffSymbolsAt[i]=0;
			nCodesAtLevel[i]=0;
		}
		for (uint i=0;i<=max_v_cont;i++){
			diffSymbolsAt[codeLen[i]]++;
		}
		for (uint i=minLength;i<=height;i++){
			nCodesAtLevel[i]=diffSymbolsAt[i];
		}

		//computes the number of elements that finishes each level (the number of bits per level for each bitmap)
		uint *lengths = new uint[height+1];
		uint *bmpLengths = new uint[height];
		for (uint i=0;i<height;i++){
			bmpLengths[i]=lengths[i]=0;
		}
		lengths[height]=0;
		for (uint i=0;i<this->n;i++){
			for (uint j=0;j<codeLen[new_symb[i]];j++){
				bmpLengths[j]++;
				lengths[j]++;
			}	
			lengths[codeLen[new_symb[i]]]++;
		}
		
		/** build codes. codes are sorted according to its length (shortest are locate previous than largest) **/
		uint *bmpCodes=NULL,*lastCode;uint *codesInv;
		
		buildCodigos(minLength, height, diffSymbolsAt,&codes,&codesInv,&bmpCodes,&lastCode);
		uint initial = 0; 
		for (uint i=minLength;i<=height;i++){
			 uint tmp=diffSymbolsAt[i];
			 diffSymbolsAt[i] = initial;
			 initial+=tmp;
		}
		
		//newCodes sort the codes by symbol. Code of symbol[i] is now in 
		//newCodes[symbol[i]] (It is the same as newCodes[i] since symbols are mapped using a continuous mapper).
		uint *newCodes = new uint[max_v_cont+1];
		uint *newCodesInv = new uint[max_v_cont+1];
		uint *symbolMap = new uint[max_v_cont+1];
		
		// unsigned long long orden=0;
		// pairsc *pares = new pairsc[max_v_cont];
		for (uint i=0;i<=max_v_cont;i++){		
			//symbolMap, for each code stores the symbol that is associated with the i-th code. Useful to obtain a symbol from a code.
			symbolMap[diffSymbolsAt[codeLen[i]]]=i;
			newCodes[i]=codes[diffSymbolsAt[codeLen[i]]];
			newCodesInv[i]=codesInv[diffSymbolsAt[codeLen[i]]++];
		}
		//build the mapping that returns a code from a symbol and vice-versa.
		huffModel = wmb->build(max_v_cont,minLength,height,newCodesInv,symbolMap,lastCode,codesInv,nCodesAtLevel);
	
		
		delete [] newCodesInv; 
		delete [] symbolMap; 
		delete [] lastCode;
		delete [] codes;
		delete [] diffSymbolsAt;
		
		//allocate memory for bitmaps and set all to 0.
		uint ** _bm=new uint*[height];
		for(uint i=0;i<height;i++) {
			_bm[i] = new uint[lengths[i]/W+1];
			for(uint j=0;j<lengths[i]/W+1;j++)
				_bm[i][j]=0;
		}
		for (uint i=0;i<=max_v_cont;i++) OCC[i]=~0;

		build_level(_bm, new_symb, this->n, max_v_cont, OCC, lengths, newCodes, codeLen,lastCode);
		
		delete [] lengths;
		buildBitmaps(bmb, nbmb, _bm, bmpLengths);
		// bitstring = new BitSequence*[height];
		// C = new uint[height];
		// for(uint i=0;i<height;i++) {
		// 	bitstring[i] = bmb->build(_bm[i],bmpLengths[i]);
		// 	C[i] = bitstring[i]->rank0(bmpLengths[i] - 1);
		// 	delete [] _bm[i];
		// }
		delete [] _bm;

		for (uint i=0;i<nbmb;i++){
			bmb[i]->use();
		}
	}
	//TODO refactor the code inside both constructors
	WaveletMatrixHuff::WaveletMatrixHuff(uint * symbols, size_t n, wt_coder_huff * coder, 
		BitSequenceBuilder * bmb, Mapper * am, MappingWMBuilder *wmb, bool deleteSymbols) : Sequence(n){
		bmb->use();
		
		this->length=n;
		this->n=n;
		this->am=am;
		am->use();
		for(uint i=0;i<n;i++)
			symbols[i] = am->map(symbols[i]);
		max_v=max_value(symbols,n);		
		cout << "n: " << n << endl;
		cout << "sigma: " << max_v << endl; 
		
		OCC = new uint[max_v + 1];

		uint *new_symb = new uint[this->n];
		//Apply a cont mapper to symbols to access values and so on. If we know that symbols 
		//are consecutives, we do not need this. 
		BitSequenceBuilder *bsb = new BitSequenceBuilderRRR(4);
		MapperCont *mc = new MapperCont(symbols, n, *bsb);
		for (uint i=0;i<this->n;i++) new_symb[i]=mc->map(symbols[i])-1;
		delete mc; 
		uint max_v_cont = max_value(new_symb,this->n);
		
		if (deleteSymbols){
			delete [] symbols; 
			symbols=0;
		}
		//stores the length of each code
		uint *codeLen = new uint[max_v_cont + 1];
		uint *codes;
		minLength=~0;
		
		//we fill codeLen and obtain min and max code lengths. MaxCode length is height
		height=0;
		for (uint i=0;i<=max_v_cont;i++){
			uint c; 
			codeLen[i]=coder->getCode(i,c);
			if (codeLen[i]>height) height=codeLen[i];
			if (codeLen[i]<minLength) minLength=codeLen[i];
		}
		cout << "Maximum length of a code: " << height << endl;
		cout << "Minimum length of a code: " << minLength << endl; 
		
		/** number of distinct symbols that finish at each level **/
		uint *diffSymbolsAt = new uint[height+1];
		uint *nCodesAtLevel = new uint[height+1];
		for (uint i=0;i<=height;i++){
			diffSymbolsAt[i]=0;
			nCodesAtLevel[i]=0;
		}
		for (uint i=0;i<=max_v_cont;i++){
			diffSymbolsAt[codeLen[i]]++;
		}
		for (uint i=minLength;i<=height;i++){
			nCodesAtLevel[i]=diffSymbolsAt[i];
		}

		//computes the number of elements that finishes each level (the number of bits per level for each bitmap)
		uint *lengths = new uint[height+1];
		uint *bmpLengths = new uint[height];
		for (uint i=0;i<height;i++){
			bmpLengths[i]=lengths[i]=0;
		}
		lengths[height]=0;
		for (uint i=0;i<this->n;i++){
			for (uint j=0;j<codeLen[new_symb[i]];j++){
				bmpLengths[j]++;
				lengths[j]++;
			}	
			lengths[codeLen[new_symb[i]]]++;
		}
		
		/** build codes. codes are sorted according to its length (shortest are locate previous than largest) **/
		uint *bmpCodes=NULL,*lastCode;uint *codesInv;
		
		buildCodigos(minLength, height, diffSymbolsAt,&codes,&codesInv,&bmpCodes,&lastCode);
		uint initial = 0; 
		for (uint i=minLength;i<=height;i++){
			 uint tmp=diffSymbolsAt[i];
			 diffSymbolsAt[i] = initial;
			 initial+=tmp;
		}
		
		//newCodes sort the codes by symbol. Code of symbol[i] is now in 
		//newCodes[symbol[i]] (It is the same as newCodes[i] since symbols are mapped using a continuous mapper).
		uint *newCodes = new uint[max_v_cont+1];
		uint *newCodesInv = new uint[max_v_cont+1];
		uint *symbolMap = new uint[max_v_cont+1];
		
		// unsigned long long orden=0;
		// pairsc *pares = new pairsc[max_v_cont];
		for (uint i=0;i<=max_v_cont;i++){		
			//symbolMap, for each code stores the symbol that is associated with the i-th code. Useful to obtain a symbol from a code.
			symbolMap[diffSymbolsAt[codeLen[i]]]=i;
			newCodes[i]=codes[diffSymbolsAt[codeLen[i]]];
			newCodesInv[i]=codesInv[diffSymbolsAt[codeLen[i]]++];
		}
		//build the mapping that returns a code from a symbol and vice-versa.
		huffModel = wmb->build(max_v_cont,minLength,height,newCodesInv,symbolMap,lastCode,codesInv,nCodesAtLevel);
	
		
		delete [] newCodesInv; 
		delete [] symbolMap; 
		delete [] lastCode;
		delete [] codes;
		delete [] diffSymbolsAt;
		
		//allocate memory for bitmaps and set all to 0.
		uint ** _bm=new uint*[height];
		for(uint i=0;i<height;i++) {
			_bm[i] = new uint[lengths[i]/W+1];
			for(uint j=0;j<lengths[i]/W+1;j++)
				_bm[i][j]=0;
		}
		for (uint i=0;i<=max_v_cont;i++) OCC[i]=~0;

		build_level(_bm, new_symb, this->n, max_v_cont, OCC, lengths, newCodes, codeLen,lastCode);
		
		delete [] lengths;
		buildBitmaps(&bmb, 1, _bm, bmpLengths);
		// bitstring = new BitSequence*[height];
		// C = new uint[height];
		// for(uint i=0;i<height;i++) {
		// 	bitstring[i] = bmb->build(_bm[i],bmpLengths[i]);
		// 	C[i] = bitstring[i]->rank0(bmpLengths[i] - 1);
		// 	delete [] _bm[i];
		// }
		delete [] _bm;
		bmb->unuse();
	}
	

	WaveletMatrixHuff::WaveletMatrixHuff():Sequence(0) {
		bitstring = NULL;
		// occ = NULL;
		am = NULL;
	}

	WaveletMatrixHuff::~WaveletMatrixHuff() {
		if(bitstring) {
			for(uint i=0;i<height;i++)
				if(bitstring[i])
					delete bitstring[i];
			delete [] bitstring;
		}

		if(am)
			am->unuse();
		delete [] C;
	}

	void WaveletMatrixHuff::save(ofstream & fp) const
	{
		uint wr = WVMATRIX_HUFF_HDR;
		saveValue(fp,wr);
		saveValue<size_t>(fp,n);
		saveValue(fp,max_v);
		saveValue(fp,height);
		saveValue(fp, C, height);
		am->save(fp);
		for(uint i=0;i<height;i++)
			bitstring[i]->save(fp);
		saveValue<uint>(fp, OCC, max_v + 1);
		huffModel->save(fp);
	}

	WaveletMatrixHuff * WaveletMatrixHuff::load(ifstream & fp) {
		uint rd = loadValue<uint>(fp);
		if(rd!=WVMATRIX_HUFF_HDR) return NULL;
		WaveletMatrixHuff * ret = new WaveletMatrixHuff();
		ret->n = loadValue<size_t>(fp);
		ret->length = ret->n;
		ret->max_v = loadValue<uint>(fp);
		ret->height = loadValue<uint>(fp);
		ret->C = loadValue<uint>(fp, ret->height);
		ret->am = Mapper::load(fp);
		if(ret->am==NULL) {
			delete ret;
			return NULL;
		}
		ret->am->use();
		ret->bitstring = new BitSequence*[ret->height];
		for(uint i=0;i<ret->height;i++)
			ret->bitstring[i] = NULL;
		for(uint i=0;i<ret->height;i++) {
			ret->bitstring[i] = BitSequence::load(fp);
			if(ret->bitstring[i]==NULL) {
				cout << "damn" << i << " " << ret->height << endl;
				delete ret;
				return NULL;
			}
		}
		ret->OCC = loadValue<uint>(fp, ret->max_v + 1);
		ret->huffModel = MappingWM::load(fp);

		return ret;
	}

	inline uint get_start(uint symbol, uint mask) {
		return symbol&mask;
	}

	inline uint get_end(uint symbol, uint mask) {
		return get_start(symbol,mask)+!mask+1;
	}

	bool WaveletMatrixHuff::is_set(uint val, uint ind) const
	{
		assert(ind<height);
		return (val & (1<<ind))!=0;
	}

	uint WaveletMatrixHuff::set(uint val, uint ind) const
	{
		assert(ind<=height);
		return val | (1<<(height-ind-1));
	}

	uint WaveletMatrixHuff::access(size_t pos) const
	{
		uint ret=0;
		uint level = 0, levelplus1;
		while(1){
			size_t optR = 0;
			levelplus1=level+1;
			if (bitstring[level]->access(pos, optR)) {
				//~ cout << "1";
				//~ ret = (ret<<1);
				if (huffModel->finishCode(ret,levelplus1)) break;
				pos = C[level] + optR - 1;
			}
			else {
				//~ cout << "0";
				ret = ret| (1<<level);
				if (huffModel->finishCode(ret,levelplus1)) break;
				pos = optR - 1;
			}
			level=levelplus1;
		}
		return huffModel->getSymbol(ret,levelplus1);
	}

	size_t WaveletMatrixHuff::rank(uint symbol, size_t pos) const
	{
		symbol = am->map(symbol);
		uint code,len;code=len=0;
		huffModel->getCode(symbol,code,len);
		//~ size_t start = 0;
		for (uint level = 0; level < len; level++) {
			if(is_set(code,level)) {
				//~ if (start > 0)
					//~ start = start - bitstring[level]->rank1(start - 1);
				pos = pos - bitstring[level]->rank1(pos);
			}
			else {
				//~ if (start > 0)
					//~ start = bitstring[level]->rank1(start - 1);
				//~ start += C[level];
				pos = bitstring[level]->rank1(pos) + C[level] - 1;
				
			}
			//~ if (pos + 1 - start == 0) return 0;
		}
		//~ return pos + 1 - start;;
		return pos + 1 - OCC[symbol];;
	}

	size_t WaveletMatrixHuff::select(uint symbol, size_t j) const
	{
		symbol = am->map(symbol);
		uint len,code;
		//~ m->getLength(symbol,len);
		huffModel->getCode(symbol,code,len);
		size_t pos = OCC[symbol] + j - 1; //(symbol == 0? -1 : occ->select1(symbol)) + j;
		
		for (int level = len - 1; level >= 0; level--) {
			if (pos < C[level]) {
				pos = bitstring[level]->select0(pos + 1);
			}					 // right
			else {
				pos = bitstring[level]->select1(pos - C[level] + 1);
			}
		}
		//~ cout << endl;
		return pos;
	}

	size_t WaveletMatrixHuff::getSize() const
	{
		size_t ptrs = sizeof(WaveletMatrixHuff)+height*sizeof(Sequence*);
		size_t bytesBitstrings = 0;
		
		for(uint i=0;i<height;i++){
			bytesBitstrings += bitstring[i]->getSize();
		}
		return bytesBitstrings /* + occ->getSize() */ + ptrs 
			+ height * sizeof(uint) + sizeof(uint) * (max_v + 2) + huffModel->getSize();
	}

	//check if a code has to continue in the next level. if not remove from new_symbols and continue. 

	void pintSymbols(uint *symb, uint len, uint *codeLen, uint sigma){
		
		uint *contadores = new uint[sigma+1];
		for (uint i=0;i<=sigma;i++) contadores[i]=0;
		for (uint i=0;i<len;i++) contadores[codeLen[symb[i]]]++;
		for (uint i=0;i<=sigma;i++) cout << "ncodes[" << i << "]: " << contadores[i] << endl;
		delete [] contadores;
	}
	
	void WaveletMatrixHuff::build_level(uint **bm, uint *symbols, uint length, uint sigma, uint *occs, uint *lengths, uint *codes, uint *codeLen, uint *lastCode) {
		
		bool *new_order = new bool[sigma + 1];	 
		for (uint level = 0; level <= height; level++) {
			
			for (uint i = 0; i < sigma + 1; i++)
				new_order[i] = ((codeLen[i]>level) && is_set(codes[i], codeLen[i] - level-1)) || (codeLen[i]==level);
				
			uint zeroes = 0;
			for (uint i = 0; i < lengths[level]; i++)
				if (!new_order[symbols[i]])
					zeroes++;

			uint cnt=0;
			uint fstEnd=0;
			uint EMPTY = ~0;	
			uint *new_symbols = new uint[lengths[level]];
			uint new_pos0 = 0, new_pos1 = zeroes;
			uint codeAux=0;
			for (uint i=0; i < lengths[level]; i++) {
				
				
				if (new_order[symbols[i]]) {
					if (codeLen[symbols[i]] > level){ 
						assert(fstEnd==0);
						if (fstEnd>0)  cout << "**Error: codes wrong formed --> level: " << level << "1-symbol: " << symbols[i] << ", code: " << codes[symbols[i]] << ", codeLen: " << codeLen[symbols[i]] << ", code_finished: " << codeAux <<endl;
						new_symbols[new_pos1++] = symbols[i];
						bitset(bm[level],i);
					}else{
						cnt++;
						if (fstEnd==0){
							fstEnd=i+1;
							codeAux = codes[symbols[i]];
							// assert(lastCode[level]==codes[symbols[i]]);
							//cout << "symbol: " << symbols[i] << ", code: " << codes[symbols[i]] << ", codeLen: " << codeLen[symbols[i]] << ", len: " << level << endl;
						 }
						if (occs[symbols[i]]==EMPTY){
							occs[symbols[i]] = i;
						}
					}
				}
				else {
					if (codeLen[symbols[i]] > level){ 
						assert(fstEnd==0);
						if (fstEnd>0)  cout << "**Error: codes wrong formed --> level: " << level << "**1-symbol: " << symbols[i] << ", code: " << codes[symbols[i]] << ", codeLen: " << codeLen[symbols[i]] << ", code_finished: " << codeAux <<endl;
						new_symbols[new_pos0++] = symbols[i];
						bitclean(bm[level],i);
					}else{
						cnt++;
						if (fstEnd==0){
							 fstEnd=i+1;
							 codeAux = codes[symbols[i]];
							 // assert(lastCode[level]==codes[symbols[i]]);
							 //cout << "symbol: " << symbols[i] << ", code: " << codes[symbols[i]] << ", codeLen: " << codeLen[symbols[i]] << ", len: " << level << endl;
						 }
						if (occs[symbols[i]]==EMPTY){
							occs[symbols[i]] = i;
						}
					}	
				}
				
			}
			assert(new_pos0==zeroes);
			// cout << "newPos1: " << new_pos1 << ", new_pos0: " << new_pos0 << endl;
			//~ cout << "cnt: " << cnt << endl;
			//~ cout <<"fstEnd: " << fstEnd <<  endl;
			delete [] symbols;
			symbols = new_symbols;
		}
		delete [] symbols;
		delete [] new_order;
	}
	
	uint WaveletMatrixHuff::max_value(uint *symbols, size_t n) {
		uint max_v = 0;
		for(size_t i=0;i<n;i++)
			max_v = max(symbols[i],max_v);
		return max_v;
	}

	uint WaveletMatrixHuff::bits(uint val) {
		uint ret = 0;
		while(val!=0) {
			ret++;
			val >>= 1;
		}
		return ret;
	}
	void WaveletMatrixHuff::buildBitmaps(BitSequenceBuilder **bmb, uint nbsb, uint **_bm, uint *bmpLengths){
		bitstring = new BitSequence*[height];
		C = new uint[height];
		for(uint i=0;i<height;i++) {
			bitstring[i] = bmb[0]->build(_bm[i],bmpLengths[i]);
			C[i] = bitstring[i]->rank0(bmpLengths[i] - 1);
			for (uint j=1;j<nbsb;j++){
				BitSequence *bs = bmb[j]->build(_bm[i],bmpLengths[i]);
				if (bs->getSize()<bitstring[i]->getSize()){
					BitSequence *tmp = bitstring[i];
					bitstring[i] = bs;
					delete tmp;
				}
			}
			delete [] _bm[i];
		}
		
	}
};
