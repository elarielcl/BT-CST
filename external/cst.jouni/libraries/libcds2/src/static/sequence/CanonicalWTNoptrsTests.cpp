/* static_sequence_canonical_wt.cpp
 * Copyright (C) 2012, Alberto Ordóñez, all rights reserved.
 *
 * static_sequence_canonical_wt definition
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


void build_wt(uint *symbols, uint n, uint level, uint maxLevel, uint *sizeBitmaps, uint *bitmaps, tuple *table_symbols, uint *finishAtLevel){
	
	
	if (n>=0 && (table_symbols[symbols[0]].len) == level){
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


CanonicalWTNoptrs::CanonicalWTNoptrs(uint * symbols, uint n, BitSequenceBuilder * bmb, Mapper * am, MappingBuilder *msb, bool deleteSymbols) : Sequence(n){

	build(symbols, n, bmb, am, msb);
}


void CanonicalWTNoptrs::build(uint * symbols, uint n,  BitSequenceBuilder * bmb, Mapper * am,  MappingBuilder *msb){
	
this->n=n;
	for(size_t i=0;i<n;i++) {
            symbols[i] = am->map(symbols[i])-1;
    }
	cout << "Building canonical wt without pointers" << endl; 
  uint max_v=max_value(symbols,n);
  uint *occurrences=new uint[max_v+2];
          
  
  for(uint i=0;i<=max_v+1;i++) occurrences[i]=0;
 
  for(uint i=0;i<n;i++) occurrences[symbols[i]+1]++;

	//obtain huffman codes and then continue building the bitmap occ (based on C)
	cout << "Obtaining Huffman encoding" << endl; 
		//~ wt_coder_huff *wc = new wt_coder_huff(symbols, n, am); 
		//~ tuple *table_symbols_codes = new tuple[max_v+1];
		//~ this->maxCodeLength = 0;
		//~ uint minCodeLength = ~0;
		//~ for (uint i=0;i<max_v+1; i++){
			//~ table_symbols_codes[i].symbol = i;
			//~ table_symbols_codes[i].code = 0;
			//~ table_symbols_codes[i].occs = occurrences[i];
			//~ table_symbols_codes[i].len = wc->getCode(i, table_symbols_codes[i].code);
			//~ if (table_symbols_codes[i].len > this->maxCodeLength) this->maxCodeLength = table_symbols_codes[i].len;
			//~ if (table_symbols_codes[i].len < minCodeLength) minCodeLength = table_symbols_codes[i].len;
//~ 
		//~ }
		//~ delete wc;
		//~ FILE *salida; 
		//~ salida = fopen("modeloHuffmanIndo","wb"); 
		//~ max_v++;
		//~ fwrite(&max_v,sizeof(uint),1,salida);
		//~ max_v--;
		//~ fwrite(table_symbols_codes,sizeof(tuple)*(max_v+1),1,salida);
		//~ fclose(salida);
		
		
		FILE *input; 
		input = fopen("modeloHuffmanIndo","rb"); 
		
		fread(&max_v,sizeof(uint),1,input);
		tuple *table_symbols_codes = new tuple[max_v+1];
		max_v--;
		fread(table_symbols_codes,sizeof(tuple)*(max_v+1),1,input);
		fclose(input);
		cout << "After loading huffman" << endl << "Maxv: " << max_v << endl; 
		this->maxCodeLength = 0;
		uint minCodeLength = ~0;
		for (uint i=0;i<max_v+1; i++){
			if (table_symbols_codes[i].code == 0) cout << table_symbols_codes[i].len << endl; 
			if (table_symbols_codes[i].len > this->maxCodeLength) this->maxCodeLength = table_symbols_codes[i].len;
			if (table_symbols_codes[i].len < minCodeLength) minCodeLength = table_symbols_codes[i].len;

		}
		
		cout << "MaxCodeLen: " << this->maxCodeLength << endl; 
		cout << "MinCodeLen: " << minCodeLength << endl; 
		this->firstLevelWithTerminals = this->maxCodeLength; 
		uint *limitSection = new uint[this->maxCodeLength+1];
		//Calculate the number of codes per level, that is, the number of terminal nodes in each level
		uint *numberCodesPerLevel = new uint[this->maxCodeLength+1];
		for (uint i=0;i<=this->maxCodeLength;i++){ numberCodesPerLevel[i]=0; limitSection[i]=0;}
		for (uint i=0;i<max_v+1;i++){
			numberCodesPerLevel[table_symbols_codes[i].len]++;
			if (table_symbols_codes[i].len < this->firstLevelWithTerminals) this->firstLevelWithTerminals = table_symbols_codes[i].len;
		}
		cout << "Replaces the sort by length" << endl; // << this->MaxCodeLength << endl;
			limitSection[firstLevelWithTerminals]=numberCodesPerLevel[firstLevelWithTerminals];
			for (uint i=firstLevelWithTerminals+1;i<this->maxCodeLength+1;i++){
				limitSection[i] = limitSection[i-1]+numberCodesPerLevel[i];
			}
			uint *pointerSection = new uint[this->maxCodeLength+1];
			for (uint i=firstLevelWithTerminals+1;i<maxCodeLength+1;i++) pointerSection[i]=limitSection[i-1]; 
			
			 tuple *swapTable, *auxTable = new tuple[max_v+1]; 
			 for (uint i=0;i<max_v+1;i++){
				 auxTable[pointerSection[table_symbols_codes[i].len]] = table_symbols_codes[i]; 
				 pointerSection[table_symbols_codes[i].len]++;
			 }
			 swapTable = table_symbols_codes;
			 table_symbols_codes = auxTable; 
			 
			delete [] swapTable; 
			delete [] pointerSection; 
			delete [] limitSection; 
		//~ qsort(table_symbols_codes, max_v+1, sizeof(tuple), &cmpTupleByLength);
		

		
		//asign consecutive codes to objects at the same level (it seems that the huffman 
		//encoder is not doing it correctly).
		uint code = 0, p = 0, maxSymbolsPerLevel = 0;
		for (uint i=1;i<=this->maxCodeLength;i++){
			if (numberCodesPerLevel[i]==0){
				code*=2;
				continue;
			}else{
				uint j;
				for ( j = 0; j<numberCodesPerLevel[i]; j++){
					table_symbols_codes[p++].code = code;
					code++;
				}
				if (j > maxSymbolsPerLevel) maxSymbolsPerLevel = j;
				
				code = (code)*2;//code is already (code+1) since it is always incremented after the asignment
			}
		}
		
		cout << "Sorting symbols at each level" << endl; 
		//sort the symbols of each length (in increasing order)
		uint *sortSymbols = new uint[maxSymbolsPerLevel];
		sortSymbols[0] = table_symbols_codes[0].symbol;
		p=1;
		uint begin=0;
		for (uint i=1;i<max_v+1;i++){
			if (table_symbols_codes[i].len == table_symbols_codes[i-1].len){
				sortSymbols[p++]=table_symbols_codes[i].symbol;
				
			}else{
				qsort(sortSymbols,p,sizeof(uint),cmpSymbols); 
				uint k=0;
				for (uint j=begin;j<(begin+p); j++) table_symbols_codes[j].symbol=sortSymbols[k++];
		
				begin = i;
				p=1;
				sortSymbols[0] = table_symbols_codes[i].symbol;
			}
		}
			//sort the last level of the tree
			qsort(sortSymbols,p,sizeof(uint),cmpSymbols); 
			uint k = 0;
			for (uint j=begin;j<(begin+p); j++) table_symbols_codes[j].symbol=sortSymbols[k++];
			
		delete [] sortSymbols;
		
		cout << "Building the Huffman model" << endl; 
		//build the HUFFMAN Model
		this->ms = msb->build(table_symbols_codes, max_v+1); 
		
		//sort table by symbol to access it using symbol
			auxTable = new tuple[max_v+1];  
			for (uint i=0;i<max_v+1;i++){
				auxTable[table_symbols_codes[i].symbol]= table_symbols_codes[i]; 
			}
			swapTable = table_symbols_codes; 
			table_symbols_codes = auxTable; 
			delete [] swapTable; 
		//~ qsort(table_symbols_codes, max_v+1, sizeof(tuple), cmpTupleBySymbol);

		uint *sizeBitmaps = (uint *) malloc(sizeof(uint)*this->maxCodeLength);
		for (uint i=0;i<this->maxCodeLength; i++){
			sizeBitmaps[i]=0;
		}		
		for (uint i=0;i<n; i++){
			for (uint j=0;j<table_symbols_codes[symbols[i]].len; j++){
				sizeBitmaps[j]++;
			}
		}

		
		firstLevelWithTerminals = 0; 
		for (uint i=0;i<max_v+1;i++){
			 if (firstLevelWithTerminals>table_symbols_codes[i].len)firstLevelWithTerminals= table_symbols_codes[i].len; 
		}
			
		uint totalBits = 0;
		
			uint tmp1, tmp = sizeBitmaps[0];
			sizeBitmaps[0]=0;
			for (uint i=1;i<maxCodeLength;i++){
				tmp1 =sizeBitmaps[i];
				sizeBitmaps[i]=sizeBitmaps[i-1]+tmp;
				tmp = tmp1;
			}
		
		totalBits+=sizeBitmaps[maxCodeLength-1]+tmp;
		starts = new uint[maxCodeLength];
		starts[0]=-1;
		for (uint i=1;i<maxCodeLength;i++) starts[i] = sizeBitmaps[i]-1;
		cout << "Total bits: " << totalBits << endl;
		
		uint *bitmap = (uint *) calloc(sizeof(uint) , ((totalBits+W-1)/W));

		this->n = n;
		finishAtLevel = new uint[maxCodeLength+1];
		for (uint i=0;i<maxCodeLength;i++) finishAtLevel[i]=0;
		//n o n+1?
		//~ for (uint i=0;i<n;i++) cout << symbols[i] << endl;
		build_wt(symbols, this->n, 0, this->maxCodeLength, sizeBitmaps, 
						bitmap, table_symbols_codes, finishAtLevel);
		cout << "Building bitmap" << endl;
		bitmapwt = bmb->build(bitmap, totalBits); 
		
		cout << "After building bitmap" << endl;

		cout << "Tamaño WT: " << bitmapwt->getSize() << endl;
		free(bitmap);
		free(sizeBitmaps);
		

		this->maxValuePerLevel = new uint[maxCodeLength+1];
		for (uint i=0;i<=this->maxCodeLength;i++){
			 this->maxValuePerLevel[i]=0;
		 }
		//instead of store the number of codes in each level, we can store the greatest code at each level
		for (uint i=0;i<max_v+1;i++){
			if (this->maxValuePerLevel[table_symbols_codes[i].len] < table_symbols_codes[i].code){
				this->maxValuePerLevel[table_symbols_codes[i].len] = table_symbols_codes[i].code+1;
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
	  

  delete [] numberCodesPerLevel;
  delete [] table_symbols_codes;
  delete [] occurrences;

}

CanonicalWTNoptrs::CanonicalWTNoptrs(): Sequence(0) {
	
	length = 0; 
}


CanonicalWTNoptrs::~CanonicalWTNoptrs() {
  //~ for(uint i=0;i<maxCodeLength-1;i++)
    //~ delete bitstring[i];
  //~ delete [] bitstring;
  delete bitmapwt;
  delete occ;
  delete [] maxValuePerLevel;
  delete [] finishAtLevel;
  delete ms; 
  //~ delete permVoc;
}


void CanonicalWTNoptrs::save(ofstream & fp) const{
  uint wr = CANONICALWT_NOPTRS_HDR;
  saveValue(fp,wr); 
  //~ wr = fwrite(&wr,sizeof(uint),1,fp);
  saveValue(fp,n); 
  //~ wr += fwrite(&n,sizeof(uint),1,fp);
  //~ saveValue(fp,max_v); 
  //~ wr += fwrite(&max_v,sizeof(uint),1,fp);
  //~ saveValue(fp,minCodeLength); 
  //~ wr += fwrite(&minCodeLength,sizeof(uint),1,fp);
  saveValue(fp,maxCodeLength); 
  //~ wr += fwrite(&maxCodeLength,sizeof(uint),1,fp);
  //~ saveValue(fp,numberCodesPerLevel,this->maxCodeLength+1); 
  //~ wr += fwrite(numberCodesPerLevel,sizeof(uint),this->maxCodeLength+1,fp);
  saveValue(fp,maxValuePerLevel,this->maxCodeLength+1); 
  //~ wr += fwrite(maxValuePerLevel,sizeof(uint),this->maxCodeLength+1,fp);
  //~ saveValue(fp,minValuePerLevel,this->maxCodeLength+1);  
  //~ wr += fwrite(minValuePerLevel,sizeof(uint),this->maxCodeLength+1,fp);
  saveValue(fp,finishAtLevel,this->maxCodeLength+1);  
  //~ wr += fwrite(finishAtLevel,sizeof(uint),this->maxCodeLength+1,fp);	
  
  saveValue(fp,starts,this->maxCodeLength);   
  //~ wr += fwrite(starts,sizeof(uint),this->maxCodeLength,fp);	
  
  //~ saveValue(fp,ncodesUpToLevel,this->maxCodeLength+1);  
  //~ wr += fwrite(ncodesUpToLevel,sizeof(uint),this->maxCodeLength+1,fp);	
  
  saveValue(fp,firstLevelWithTerminals);  
  //~ wr += fwrite(&firstLevelWithTerminals,sizeof(uint),1,fp);
  
  bitmapwt->save(fp); 
  occ->save(fp);
  //~ permVoc->save(fp);
  ms->save(fp); 

}


CanonicalWTNoptrs * CanonicalWTNoptrs::load(ifstream & fp) {
	cout << "Loading CANONICAL WT"<<endl;
  uint rd;
  rd = loadValue<uint>(fp); 
  if(rd!=CANONICALWT_NOPTRS_HDR) return NULL;
  CanonicalWTNoptrs * ret = new CanonicalWTNoptrs();
  ret->n = loadValue<uint>(fp); ; 
  ret->length = ret->n; 
  //~ ret->max_v = loadValue<uint>(fp); 

  ret->maxCodeLength = loadValue<uint>(fp); 

  ret->maxValuePerLevel = loadValue<uint>(fp, ret->maxCodeLength+1); 

  ret->finishAtLevel = loadValue<uint>(fp, ret->maxCodeLength+1); 

  ret->starts = loadValue<uint>(fp, ret->maxCodeLength); 

	ret->firstLevelWithTerminals = loadValue<uint>(fp); 

    ret->bitmapwt = BitSequence::load(fp);
    if(ret->bitmapwt==NULL) {
      delete ret;
      return NULL;
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
	  uint level=0;
	  uint start=0;
	  uint end=n;
	  uint code = 0;
	  uint r0;
	  uint codeLen=1;
	   pos++;
	   while(1){
		r0=(bitmapwt->rank0(end+starts[level])-bitmapwt->rank0(start+starts[level]));
		pos+=start;
		if(bitmapwt->access(pos+starts[level])) {
		  code=(code << 1)| 0x1;
		  pos=bitmapwt->rank1(pos+starts[level])-bitmapwt->rank1(start+starts[level]);
		  level++;
		  if (code<maxValuePerLevel[level]) break ; // && level>=firstLevelWithTerminals) break; 
		  start = start + r0 - finishAtLevel[level];
		  end = end - finishAtLevel[level];
		}
		else {
		  pos=bitmapwt->rank0(pos+starts[level])-bitmapwt->rank0(start+starts[level]);
		  level++;
		  code <<=1;
		  if (code<maxValuePerLevel[level]) break; // && level>=firstLevelWithTerminals) break; 
		  start =start - finishAtLevel[level];
		  end =start + r0;  
		}
		codeLen++;
	  }
		return ms->getSymbol(code,codeLen);
	}


	size_t CanonicalWTNoptrs::rank(uint symbol, size_t pos) const{
	  if(pos+1==0) return -1;
	  uint level=0;
	  uint start=0;
	  uint end=n;
	  uint code = 0;
	  uint r0;
	  uint l;int len;//int
	  uint leaf;

		ms->getCode(symbol, code, l, leaf); 
		len = (int)l;

	   pos++;//starts[level] empiezan en la posición anterior a la primera de cada bitmap
	   while(len!=-1){
			pos+=start;
			r0 = bitmapwt->rank0(starts[level]+end)-bitmapwt->rank0(starts[level]+start) ;
			if(is_set(code,len)) {
			  pos=bitmapwt->rank1(pos+starts[level])-bitmapwt->rank1(start+starts[level]);
			  level++;
			  start = start + r0 - finishAtLevel[level];
			  end   = end        - finishAtLevel[level];
			}
			else {
			  pos=bitmapwt->rank0(pos+starts[level])-bitmapwt->rank0(start+starts[level]);
			  level++;
			  start =start - finishAtLevel[level];
			  end =start + r0; 
			}
			len--;
		}
		return pos;

	}


	//result is in [1..n]
	size_t CanonicalWTNoptrs::select(uint symbol, size_t j) const {
		
		if(j==0) return 0;
		  uint code=0;uint len; uint leaf;
		  ms->getCode(symbol,code,len,leaf);
		  return recursiveSelect(code,j,0,n,(int)len,0)-1;
	}

	uint CanonicalWTNoptrs::recursiveSelect(uint code, uint j, uint start, uint end, int len, uint level)  const{

		if (len==-1){
			 return j;
		}
			uint r = bitmapwt->rank0(start+starts[level]);
			uint r0 = bitmapwt->rank0(end+starts[level])-r;
			if(is_set(code,len)) {
				level++;
			  j = recursiveSelect(code, j, start + r0 - finishAtLevel[level], end - finishAtLevel[level], len-1, level);
			   level--;
			  r = bitmapwt->rank1(start+starts[level]);
			  return bitmapwt->select1(j+r)-(start+starts[level]);
			}
			else {
			  uint tmpStart = start - finishAtLevel[level+1];
			  j = recursiveSelect(code, j, tmpStart, tmpStart + r0, len-1, level+1);
			  return bitmapwt->select0(j+r)-(start+starts[level]);
			}
	}
	
	//pos in [1..n]
	uint CanonicalWTNoptrs::forward(uint pos, uint & next) {

		uint value = occ->rank1(pos); 
		uint rank_tmp = pos - occ->select1(value); 
		next = select(value, rank_tmp+1)-1; 
		return value-1; //the bwt adds a symbol that is at the begining of voc
	}

uint CanonicalWTNoptrs::recursiveForward(uint pos, uint code, uint start, uint end, uint &codePos, uint level) {
//~ 
	//~ if (code<=maxValuePerLevel[level] && numberCodesPerLevel[level]){
		   //~ codePos = ncodesUpToLevel[level-1] + code-(maxValuePerLevel[level] - numberCodesPerLevel[level]+1);
		   //~ return pos; //return posV+pos;
	//~ }
	//~ 
	//~ uint r0;
	//~ uint r;
   //~ r  = bitmapwt->rank0(start+starts[level]);
   //~ r0 =(bitmapwt->rank0(end+starts[level])-bitmapwt->rank0(start+starts[level]));
   //~ 
   //~ if (pos >= r0){ //move to right child
      //~ code=(code << 1)| 0x1;
      //~ pos=bitmapwt->rank1(pos+starts[level])-bitmapwt->rank1(start+starts[level]);
      //~ pos = recursiveForward(pos, code, start + r0 - finishAtLevel[level+1], end - finishAtLevel[level+1], codePos, level+1);
      //~ r = bitmapwt->rank1(start+starts[level]);
	  //~ return bitmapwt->select1(pos+r)-(start+starts[level]);
    //~ }
    //~ else {
	  //~ pos=bitmapwt->rank0(pos+starts[level])-bitmapwt->rank0(start+starts[level]);
	  //~ code <<=1;
      //~ pos = recursiveForward(pos, code, start - finishAtLevel[level+1], start - finishAtLevel[level+1] + r0, codePos, level+1);
      //~ return bitmapwt->select0(pos+r)-(start+starts[level]);
    //~ }
    return -1; 

}

uint CanonicalWTNoptrs::rankLessThan(uint i, uint j) {
  if(i==0) return 0;
  return occ->select1(i+1);
}


	uint CanonicalWTNoptrs::access(size_t pos, size_t & r) const{
	  uint level=0;
	  uint start=0;
	  uint end=n;
	  uint code = 0;
	  uint r0;
	  uint len=1;
	  
	   pos++;
	   while(1){
		r0=(bitmapwt->rank0(end+starts[level])-bitmapwt->rank0(start+starts[level]));
		pos+=start;
		if(bitmapwt->access(pos+starts[level])) {
		  code=(code << 1)| 0x1;
		  pos=bitmapwt->rank1(pos+starts[level])-bitmapwt->rank1(start+starts[level]);
		  level++;
		  if (code<maxValuePerLevel[level]) break; // && level>=firstLevelWithTerminals) break; 
		  start = start + r0 - finishAtLevel[level];
		  end = end - finishAtLevel[level];
		}
		else {
		  pos=bitmapwt->rank0(pos+starts[level])-bitmapwt->rank0(start+starts[level]);
		  level++;
		  code <<=1;
		 if (code<maxValuePerLevel[level]) break;// && level>=firstLevelWithTerminals) break; 
		  start =start - finishAtLevel[level];
		  end =start + r0;  
		}
		len++;
	  }
		r = pos-1;
		return ms->getSymbol(code,len);

	}


size_t CanonicalWTNoptrs::selectNext(uint c, size_t i) const{
	
	return 0;
}

size_t CanonicalWTNoptrs::getSize() const {
  uint ptrs = sizeof(CanonicalWTNoptrs)+maxCodeLength*sizeof(Sequence*);

    cout << "Bitmap size: " << bitmapwt->getSize() << endl;
    cout << "Model size: " << ms->getSize() << endl; 
    cout << "Only Wavelet Tree: " << bitmapwt->getSize()+ptrs+sizeof(uint)*(maxCodeLength+1)*5+sizeof(CanonicalWTNoptrs) << endl;
  return (size_t)bitmapwt->getSize()+occ->getSize()+ptrs+sizeof(uint)*(maxCodeLength+1)*5+ms->getSize()+sizeof(CanonicalWTNoptrs);

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
