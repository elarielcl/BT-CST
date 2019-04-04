/* CanonicalWTNoptrs.cpp
 * Copyright (C) 2012, Alberto Ordóñez, all rights reserved.
 *
 *  Implementation of a pointerless huffman (canonical) shaped wavelet tree
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

#include <sequence/CanonicalWTNoptrs.h>


namespace cds_static{


void build_wt(uint *symbols, int n, uint level, uint maxLevel, uint *sizeBitmaps, uint *bitmaps, tuple *table_symbols, uint *finishAtLevel){
	
	
	if (n>0 && (table_symbols[symbols[0]].len) == level){
		 finishAtLevel[level] +=n;
		 delete [] symbols;
		 return;
	 }
	uint left=0;
	for (uint i=0;i<n;i++){
		if (!(bitget(&(table_symbols[symbols[i]].code), table_symbols[symbols[i]].len-level-1)) && (table_symbols[symbols[i]].len > level)){
			left++;
		}else{
			bitset(bitmaps, sizeBitmaps[level]);
		}
		sizeBitmaps[level]++;
	} 
	uint *symbLeft, *symbRight, pl=0, pr=0;

	symbLeft = new uint[left];
	symbRight = new uint[n-left];
	
	for (uint i=0;i<n;i++){
		if (bitget(&(table_symbols[symbols[i]].code), table_symbols[symbols[i]].len-level-1) && table_symbols[symbols[i]].len > level){
			symbRight[pr++] = symbols[i];
		}else{
			symbLeft[pl++] = symbols[i];			
		}
	}
	
	assert(pl==left);
	assert(pr==(n-left));
	
	delete [] symbols;
	build_wt(symbLeft,  left, level+1, maxLevel, sizeBitmaps, bitmaps, table_symbols, finishAtLevel);
	build_wt(symbRight, n-left, level+1, maxLevel, sizeBitmaps, bitmaps, table_symbols, finishAtLevel);
	
}

void build_wt(uint *symbols, int n, uint level, uint maxLevel, uint *sizeBitmaps, uint **bitmaps, tuple *table_symbols, uint *finishAtLevel){
	
	if (n==0) return;
	if (n>0 && (table_symbols[symbols[0]].len) == level){
		 finishAtLevel[level] +=n;
		 delete [] symbols;
		 return;
	 }
	//~ if (level==maxLevel)
		//~ cout << endl;
	uint left=0;
	for (uint i=0;i<n;i++){
		if (!(bitget(&(table_symbols[symbols[i]].code), table_symbols[symbols[i]].len-level-1)) && (table_symbols[symbols[i]].len > level)){
			left++;
		}else{
			bitset(bitmaps[level], sizeBitmaps[level]);
		}
		sizeBitmaps[level]++;
	} 
	uint *symbLeft, *symbRight, pl=0, pr=0;

	symbLeft = new uint[left];
	symbRight = new uint[n-left];
	
	for (uint i=0;i<n;i++){
		if (bitget(&(table_symbols[symbols[i]].code), table_symbols[symbols[i]].len-level-1) && table_symbols[symbols[i]].len > level){
			symbRight[pr++] = symbols[i];
		}else{
			symbLeft[pl++] = symbols[i];			
		}
	}
	assert(pl==left);
	assert(pr==(n-left));
	
	delete [] symbols;
	build_wt(symbLeft,  left, level+1, maxLevel, sizeBitmaps, bitmaps, table_symbols, finishAtLevel);
	build_wt(symbRight, n-left, level+1, maxLevel, sizeBitmaps, bitmaps, table_symbols, finishAtLevel);
}

/**
 * First computes the Huffman set of codes of _symbols (and then the canonical one) and then builds a WT with thta set of codes 
 * */
CanonicalWTNoptrs::CanonicalWTNoptrs(uint * _symbols, uint _n, BitSequenceBuilder * bmb, Mapper * am, 
	MappingBuilder *msb, bool deleteSymbols) : Sequence(n){
	n=_n;
	uint *symbols = new uint[n];
	uint max_v=0;
    for (uint i=0;i<n;i++){
		   symbols[i]=_symbols[i];
			max_v=max(max_v,symbols[i]);
	   }
	 sigma=max_v;
	wt_coder_huff *wc = new wt_coder_huff(symbols, n, am); 
	wt_coder_huff_can *wcc = new wt_coder_huff_can(wc,max_v);
	build(symbols,max_v,wcc, bmb, am, msb);
	//~ delete [] symbols;
}


/**
 * Computes the canonical set of codes from a Huffman set of codes and then builds the WT with that shape
 * */
CanonicalWTNoptrs::CanonicalWTNoptrs(uint * _symbols, uint _n, wt_coder_huff *wc, BitSequenceBuilder * bmb, Mapper * am, MappingBuilder *msb,  bool deleteSymbols) : Sequence(n){
	n=_n;
	uint *symbols = new uint[n];
	uint max_v=0;
    for (uint i=0;i<n;i++){
		   symbols[i]=_symbols[i];
			max_v=max(max_v,symbols[i]);
	   }
	sigma=max_v;
	  
    if (wc==NULL){
		wc = new wt_coder_huff(symbols, n, am); 
	}
	wt_coder_huff_can *wcc = new wt_coder_huff_can(wc,max_v);

	build(symbols,max_v,wcc, bmb, am, msb);
	//~ delete [] symbols;
}

/**
 * Given a canonical set of codes, builds the WT with that shape
 * */
CanonicalWTNoptrs::CanonicalWTNoptrs(uint * _symbols, uint _n, wt_coder_huff_can *wc, BitSequenceBuilder * bmb, Mapper * am, MappingBuilder *msb,  bool deleteSymbols) : Sequence(n){
	n=_n;
	uint *symbols = new uint[n];
	uint max_v=0;
    for (uint i=0;i<n;i++){
		   symbols[i]=_symbols[i];
			max_v=max(max_v,symbols[i]);
	   }
	sigma=max_v;
	build(symbols,max_v,wc, bmb, am, msb);
	//~ delete [] symbols;
}

void CanonicalWTNoptrs::build(uint *symbols, uint max_v, wt_coder_huff_can *wcc, BitSequenceBuilder * bmb, Mapper * am,  MappingBuilder *msb){
	
  uint *occurrences=new uint[max_v+2];
    
  for(uint i=0;i<=max_v+1;i++) occurrences[i]=0;
 
  for(uint i=0;i<n;i++) occurrences[symbols[i]]++;

		this->firstLevelWithTerminals = wcc->minCodeLength;
		this->maxCodeLength=wcc->maxCodeLength;
		tuple *table_symbols_codes = wcc->table_symbols_codes;
		this->nCodesLevel=wcc->nCodesAtLevel;
		//build the HUFFMAN Model
		this->ms = msb->build(table_symbols_codes, max_v+1); 
		//sort table by symbol to access it using symbol
			tuple *auxTable = new tuple[max_v+1];  
			for (uint i=0;i<max_v+1;i++){
				auxTable[table_symbols_codes[i].symbol]= table_symbols_codes[i]; 
			}

		uint *sizeBitmaps = (uint *) malloc(sizeof(uint)*this->maxCodeLength);
		for (uint i=0;i<this->maxCodeLength; i++){
			sizeBitmaps[i]=0;
		}		
		for (uint i=0;i<n; i++){
			for (uint j=0;j<auxTable[symbols[i]].len; j++){
				sizeBitmaps[j]++;
			}
		}


		finishAtLevel = new uint[maxCodeLength+1];
		for (uint i=0;i<maxCodeLength;i++) finishAtLevel[i]=0;		
		
		uint **bitmaps = (uint **)malloc(sizeof(uint)*this->maxCodeLength);
		for (uint i=0;i<this->maxCodeLength;i++){
				bitmaps[i] = (uint *) calloc(sizeof(uint),((sizeBitmaps[i]+W-1)/W));
		}
		
		for (uint i=0;i<this->maxCodeLength;i++) sizeBitmaps[i]=0;
		for (uint i=0;i<maxCodeLength;i++) finishAtLevel[i]=0;

		build_wt(symbols, (int)this->n, 0, this->maxCodeLength, sizeBitmaps, 
					bitmaps, auxTable, finishAtLevel);

		assert(sizeBitmaps[0]==n);
		bitstring = new BitSequence*[this->maxCodeLength];
		for (uint i=0;i<this->maxCodeLength;i++){
			bitstring[i]=bmb->build(bitmaps[i],sizeBitmaps[i]);
		}
		free(sizeBitmaps);
		for (uint i=0;i<maxCodeLength;i++)  free(bitmaps[i]);
		free(bitmaps);
		
		//the vector C does not point any more to each virtual leaf of the WT. Instead, 
		//it points to each left child of each node. It is useful to improve the 
		//performance of select queries (it is only used in that case)
		C 			= new uint[max_v+1];
		iniC 		= new uint[maxCodeLength+1];
		fstCode 	= new uint[maxCodeLength+1];
	
		iniC[maxCodeLength]=0;
		C[iniC[maxCodeLength]]=-1;
		uint pointerFstCode = max_v-nCodesLevel[maxCodeLength]+1;
		uint pTable=pointerFstCode;
		
		uint nCodesLastLevel=nCodesLevel[maxCodeLength]/2;
		uint pointerC=1;
		fstCode[maxCodeLength] = table_symbols_codes[pointerFstCode].code;
		//the number of nodes in the last level is always even
		for (uint i=2;i<nCodesLevel[maxCodeLength];i+=2){
			C[pointerC]=C[pointerC-1]+(occurrences[table_symbols_codes[pTable].symbol]+occurrences[table_symbols_codes[pTable+1].symbol]);
			pTable+=2;
			pointerC++;
		}
		for (int i=maxCodeLength-1;i>=0;i--){
			pointerFstCode = pointerFstCode-nCodesLevel[i];
			fstCode[i]=table_symbols_codes[pointerFstCode].code;
			pTable=pointerFstCode;
			iniC[i]=pointerC;
			C[iniC[i]]=-1;//Changed
			pointerC++;
			uint limitj = nCodesLevel[i]/2*2;
			for (uint j=2;j<=limitj;j+=2){
				C[pointerC]=C[pointerC-1]+(occurrences[table_symbols_codes[pTable].symbol]+occurrences[table_symbols_codes[pTable+1].symbol]);
				pointerC++;	
				pTable+=2;
			}

			uint toAdd=0;
			uint even=1;
			//if there is an odd number of symbols that finishes at level i, 
			//the number of occurrences of the last code should be considered for 
			//the next value of C[pointerC]
			if (nCodesLevel[i]%2){
				toAdd=occurrences[table_symbols_codes[pTable].symbol];
				even=0;
			}
			uint indexPrev=0;
			
			for (uint j=1;(j+even)<nCodesLastLevel;j+=2){
				C[pointerC]=C[pointerC-1]+(C[iniC[i+1]+j+even]-C[iniC[i+1]+indexPrev])+toAdd;
				//~ assert(C[pointerC]>C[pointerC-1]);
				indexPrev=j+even;
				toAdd=0;
				//~ assert(C[pointerC]<starts[i]);
				//~ assert(C[pointerC]<bitmapwt->getLength());
				
				pointerC++;
			}
			nCodesLastLevel = pointerC - iniC[i];
		}
		

		this->maxValuePerLevel = new uint[maxCodeLength+1];
		for (uint i=0;i<=this->maxCodeLength;i++){
			 this->maxValuePerLevel[i]=0;
		 }
		//instead of store the number of codes at each level, we can store the greatest code at each level
		for (uint i=0;i<max_v+1;i++){
			if (this->maxValuePerLevel[auxTable[i].len] < auxTable[i].code){
				this->maxValuePerLevel[auxTable[i].len] = auxTable[i].code;
			}
		}
		
	//build the bitmap occ (replaces the vector C)

	  for(uint i=1;i<=max_v;i++)
		occurrences[i] += occurrences[i-1];
	  uint *oc = new uint[(n+1)/W+1];
	  
	  for(uint i=0;i<(n+1)/W+1;i++)
		oc[i] = 0;
		bitset(oc,0);
	  for(uint i=0;i<=max_v;i++)
		bitset(oc,occurrences[i]);
	  bitset(oc,n);
	  occ = bmb->build(oc,n+1);
	  delete [] oc;

  delete [] auxTable;
  delete wcc;
  delete [] occurrences;

}


CanonicalWTNoptrs::CanonicalWTNoptrs(): Sequence(0) {	
	length = 0; 
}


	CanonicalWTNoptrs::~CanonicalWTNoptrs() {
	  for (uint i=0;i<maxCodeLength;i++) delete bitstring[i];
	  delete [] bitstring;
	  delete occ;
	  delete [] maxValuePerLevel;
	  delete [] finishAtLevel;
	  delete [] nCodesLevel;
	  delete ms; 
	}


	void CanonicalWTNoptrs::save(ofstream & fp) const{
	  uint wr = CANONICALWT_NOPTRS_HDR;
	  saveValue(fp,wr); 
	  saveValue(fp,n); 
	  saveValue(fp,sigma);
	  saveValue(fp,maxCodeLength); 
	  saveValue(fp,maxValuePerLevel,this->maxCodeLength+1); 
	  saveValue(fp,finishAtLevel,this->maxCodeLength+1);  
	  saveValue(fp,firstLevelWithTerminals);  
	  saveValue(fp,C,sigma+1);
	  saveValue(fp,iniC,this->maxCodeLength+1);
	  saveValue(fp,fstCode,this->maxCodeLength+1);
	  saveValue(fp,nCodesLevel,this->maxCodeLength+1);
	  for (uint i=0;i<this->maxCodeLength;i++)
		bitstring[i]->save(fp);
	  occ->save(fp);
	  ms->save(fp); 
	}


	CanonicalWTNoptrs * CanonicalWTNoptrs::load(ifstream & fp) {
	  uint rd;
	  rd = loadValue<uint>(fp); 
	  if(rd!=CANONICALWT_NOPTRS_HDR) return NULL;
	  CanonicalWTNoptrs * ret = new CanonicalWTNoptrs();
	  ret->n = loadValue<uint>(fp); ; 
	  ret->length = ret->n; 
	  ret->sigma=loadValue<uint>(fp);
	  ret->maxCodeLength = loadValue<uint>(fp); 
	  ret->maxValuePerLevel = loadValue<uint>(fp, ret->maxCodeLength+1); 
	  ret->finishAtLevel = loadValue<uint>(fp, ret->maxCodeLength+1); 
	  ret->firstLevelWithTerminals = loadValue<uint>(fp); 
	  ret->C = loadValue<uint>(fp,ret->sigma+1);
	  ret->iniC = loadValue<uint>(fp,ret->maxCodeLength+1);
	  ret->fstCode = loadValue<uint>(fp,ret->maxCodeLength+1);
	  ret->nCodesLevel = loadValue<uint>(fp,ret->maxCodeLength+1);
	  ret->bitstring = new BitSequence*[ret->maxCodeLength];
	  for (uint i=0;i<ret->maxCodeLength;i++){
		ret->bitstring[i]=BitSequence::load(fp);
		if (ret->bitstring[i]==NULL){
			delete ret;
			return NULL;
		}
	  }
	  ret->occ = BitSequence::load(fp);
	  if(ret->occ==NULL) {
		delete ret;
		return NULL;
	  }
	  ret->ms = MappingSymbols::load(fp); 
	  return ret;
	}


	uint CanonicalWTNoptrs::access(size_t pos) const{
		uint level=0, start=0,end=n-1,code = 0;

	   do{
		pos+=start;
		uint r0 = bitstring[level]->rank0(start-1);
		uint n0 = bitstring[level]->rank0(end)-r0;
		uint n0_pos = bitstring[level]->rank0(pos-1)-r0;
		code = code << 1;
		if(bitstring[level]->access(pos)) {
			level++;
		  code|= 0x1;
		  pos = (pos-start)-n0_pos;//rank1 from rank0
		  start = start - finishAtLevel[level] + n0;
		  end   = end   - finishAtLevel[level];
		}else {
			level++;
		  pos = n0_pos;
		  start = start - finishAtLevel[level];
		  end   = start + n0-1;  
		}
	  }while(level<firstLevelWithTerminals || code>maxValuePerLevel[level]);
		//cambiar condicion while
		return ms->getSymbol(code,level);
	}

	size_t CanonicalWTNoptrs::rank(uint symbol, size_t pos) const{
	  if(pos+1==0) return -1;
	  uint level=0;
	  uint start=0;
	  uint end=n-1;
	  uint code = 0;
	  uint l;
	  uint leaf;

	   ms->getCode(symbol, code, l, leaf); 
	   l--;
	   //starts[level] empiezan en la posición anterior a la primera de cada bitmap
		do{
			pos+=start;
			uint r0 = bitstring[level]->rank0(start-1);
			uint n0 = bitstring[level]->rank0(end)-r0;
			uint n0_pos = bitstring[level]->rank0(pos-1)-r0;

			if(is_set(code,l-level)) {
			  level++;
			  pos = (pos-start)-n0_pos;//rank1 from rank0
			  start = start - finishAtLevel[level] + n0;
			  end   = end   - finishAtLevel[level]; 
			}else {
				level++;
				pos = n0_pos;
				start = start - finishAtLevel[level];
				end   = start + n0-1;  
			}
		}while(level<=l);
		
		return pos+1;

	}
	
	size_t CanonicalWTNoptrs::select(uint symbol, size_t j) const {
		
		 uint code=0;uint len; uint leaf;
		  ms->getCode(symbol,code,len,leaf);
		  //len++;
		  uint pos = (code-fstCode[len])>>1;
		  uint bit=0;
		  uint level = len;
		  while(level>0){
			  uint p = C[iniC[level]+pos];
			  level--;
			  if (is_set(code,bit++)){
				  uint r=bitstring[level]->rank1(p);
				  j = bitstring[level]->select1(j+r)-p;
			  }else{
				  uint r=bitstring[level]->rank0(p);
				  j = bitstring[level]->select0(j+r)-p;
			  }
			pos = (nCodesLevel[level]+pos )>>1;
			
		  }
		  return j-1;
	}


	//pos in [1..n]
	uint CanonicalWTNoptrs::forward(uint pos, uint & next) {

		uint value = occ->rank1(pos); 
		uint rank_tmp = pos - occ->select1(value); 
		next = select(value, rank_tmp+1)-1; 
		return value-1; //the bwt adds a symbol that is at the begining of voc
	}

	uint CanonicalWTNoptrs::recursiveForward(uint pos, uint code, uint start, uint end, uint &codePos, uint level) {
		return -1; 
	}

	uint CanonicalWTNoptrs::rankLessThan(uint i, uint j) {
	  if(i==0) return 0;
	  return occ->select1(i+1);
	}


	uint CanonicalWTNoptrs::access(size_t pos, size_t & r) const{
	  cerr << "Not implemented yet" << endl;
	  return -1;
	}


	size_t CanonicalWTNoptrs::selectNext(uint c, size_t i) const{
		
		return 0;
	}

	size_t CanonicalWTNoptrs::getSize() const {

		size_t bmp_size=0;
		for (uint i=0;i<this->maxCodeLength;i++){
		 bmp_size+=bitstring[i]->getSize();
		}
		bmp_size+=sizeof(BitSequence *)*maxCodeLength;
		return (size_t)bmp_size+sizeof(uint)*(maxCodeLength+1)*5+ms->getSize()+sizeof(CanonicalWTNoptrs) + 3*sizeof(uint)*(maxCodeLength+1)+(sigma+1)*sizeof(uint);
	}



	uint CanonicalWTNoptrs::max_value(uint *symbols, uint n) {
	  uint max_v = 0;
	  for(uint i=0;i<n;i++)
		max_v = max(symbols[i],max_v);
	  return max_v;
	}



bool CanonicalWTNoptrs::is_set(uint val, uint ind)  const{
  return (val & (1<<ind))!=0;
}


};
