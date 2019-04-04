/* wt_coder_Suboptimal.h
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

#include <sequence/wt_coder_Suboptimal.h>
#include <climits>

namespace cds_static
{
    

    uint wt_coder_Suboptimal::countingSort(uint *_freqs, uint max_v, uint *&new_order){

    uint *freqs = new uint[max_v+1];
    for (uint i=0;i<=max_v;i++)
      freqs[i]=_freqs[i];

    uint *origFreqs = new uint[max_v+1];
    for (uint i=0;i<=max_v;i++)
      origFreqs[i]=freqs[i];

    uint maxFreq = 0;
    for (uint i=0;i<=max_v;i++)
      maxFreq = max(maxFreq,freqs[i]);

    uint *bitmap;
    createEmptyBitmap(&bitmap, maxFreq+1);
    for (uint i=0;i<=max_v;i++){
      bitset(bitmap,freqs[i]);
    }
    BitSequenceRG *rg = new BitSequenceRG(bitmap, maxFreq+1, 32);
    for (uint i=0;i<=max_v;i++){
      freqs[i] = rg->rank1(freqs[i])-1;
      assert(rg->select1(freqs[i]+1)==origFreqs[i]);
      assert(freqs[i]<=max_v);
    }
    uint ndistinct = rg->rank1(rg->getLength()-1);
    uint *bucket = new uint[ndistinct];
    for (uint i=0;i<ndistinct;i++)
      bucket[i]=0;
    for (uint i=0;i<=max_v;i++){
      bucket[freqs[i]]++;
    }

    int *testBucket = new int[ndistinct];
    for (uint i=0;i<ndistinct-1;i++){
      testBucket[i]=bucket[i];
      assert(i==0 || testBucket[i]>0);
    }

    uint tmp = bucket[0];
    bucket[0]=0;
    for (uint i=1;i<ndistinct-1;i++){
      uint tmp2 = bucket[i];
      bucket[i]=tmp;
      tmp +=tmp2;
    }
    bucket[ndistinct-1] = tmp;
    for (uint i=1;i<ndistinct-1;i++){
      assert(bucket[i]>=bucket[i-1]);
    }

    new_order = new uint[max_v+1];
    for (uint i=0;i<=max_v;i++){
        // if (bucket[freqs[i]]==1){
        //   cerr << endl;
        // }
        new_order[bucket[freqs[i]]]=i;
        bucket[freqs[i]]++;    
    }

    uint acum=0;  
    for (uint i=0;i<ndistinct-1;i++){
      acum+=testBucket[i];
      assert(acum==bucket[i]);
    }

    for (uint i=1;i<ndistinct-1;i++){
      assert(bucket[i]>=bucket[i-1]);
    }
    for (uint i=0;i<max_v;i++){
      assert(origFreqs[new_order[i]]<=origFreqs[new_order[i+1]]);
    }
   // delete [] origFreqs;
    delete rg;
    delete [] bitmap;
    delete [] bucket;
    delete [] testBucket;
    delete [] freqs;
    return 0;
  }

    void wt_coder_Suboptimal::takePackage(uint j, uint n) {
        /** @type {number} */
        uint x = type[j][curpos[j]];
        
        if (x == n) {
          takePackage(j+1,n);
          takePackage(j+1,n);
        } else {
          --c[x];
        }

        ++curpos[j];
  }
    
  /**
 * Reverse Package Merge Algorithm.
 * @param {Array.<number>} P sorted probability.
 * @param {number} n number of symbols.
 * @param {number} L code length limit.
 * @return {Array.<number>} code lengths.
 */
    uint * wt_coder_Suboptimal::reversePackageMerge(uint *P, long n, int L) {
      /** @type {Array.<number>} */
      uint *l = new uint[L];
      /** @type {Array.<number>} */
      uint *b = new uint[L];
      /** @type {Array.<number>} */
      uint  **value = new uint*[L];

      type = new uint*[L];

      c = new uint[n];

      curpos = new uint[L];

      uint excess = (1 << L) - n;
      /** @type {number} */
      uint half = (1 << (L - 1));
      /** @type {number} */
      uint i;
      /** @type {number} */
      int j;
      /** @type {number} */
      uint t;
      /** @type {number} */
      uint weight;
      /** @type {number} */
      uint next;


      l[L-1] = n;

      for (j = 0; j < L; ++j) {
        if (excess < half) {
          b[j] = 0;
        } else {
          b[j] = 1;
          excess -= half;
        }
        excess <<= 1;
        if (L-2-j<0) continue; 
        l[L-2-j] = (l[L-1-j] / 2 | 0) + n;
        //cerr << "ind: " << L-2-j << ", value: " << l[L-2-j] << endl;
      }
      l[0] = b[0];

      value[0] = new uint[l[0]];
      type[0]  = new uint[l[0]];
      for (j = 1; j < L; ++j) {
        if (l[j] > 2 * l[j-1] + b[j]) {
          l[j] = 2 * l[j-1] + b[j];
        }
        //cerr << "index: " << j << ", li: " << l[j] << endl;
        value[j] = new uint[l[j]];
        type[j]  = new uint[l[j]];
      }

      for (i = 0; i < n; ++i) {
        c[i] = L;
        //cerr << c[i] << endl;
      }
    //cerr << "n: " << n << endl;

      for (t = 0; t < l[L-1]; ++t) {
        value[L-1][t] = P[t];
        type[L-1][t]  = t;
      }

      for (i = 0; i < L; ++i) {
        curpos[i] = 0;
      }
      if (b[L-1] == 1) {
        --c[0];
        ++curpos[L-1];
      }

      for (j = L-2; j >= 0; --j) {
        i = 0;
        weight = 0;
        next = curpos[j+1];
        //cerr << "next: " << next << endl;
        for (t = 0; t < l[j]; t++) {
          weight = value[j+1][next] + value[j+1][next+1];
          //cerr << weight << endl;
          if (weight > P[i]) {
            value[j][t] = weight;
            type[j][t] = n;
            next += 2;
          } else {
            value[j][t] = P[i];
            type[j][t] = i;
            ++i;
          }
        }

        curpos[j] = 0;
        if (b[j] == 1) {
         // cerr << "j: " << j << endl;
          takePackage(j,n);
        }
      }
      for (uint i=0;i<n;i++){
       // cerr << c[i] << ", ";
      }
      return c;
    }

    wt_coder_Suboptimal::wt_coder_Suboptimal():wt_coder_huff_can() {}

    wt_coder_Suboptimal::wt_coder_Suboptimal(uint *input, uint len, uint L, MappingBuilder *mb):wt_coder_huff_can(){
     
        uint max_v=0;
      for (uint i=0;i<len;i++){
        max_v = max(max_v,input[i]);
      }
      sigma = max_v+1;

      uint *occs = new uint[max_v+1];
      for (uint i=0;i<len;i++){
        occs[input[i]]++;
      }

      uint lmax = L;
      
      if(lmax<(bits(max_v))){
        cerr << "lmax: " << lmax << "< log(sigma): " << bits(max_v) << endl;
        abort();
        
      }

      uint totalFreq = 0;
      for (uint i=0;i<=max_v;i++)
        totalFreq+=occs[i];
      assert(totalFreq==len);
      uint *copyOccs = new uint[max_v+1];
      for (uint i=0;i<=max_v;i++)
          copyOccs[i]=occs[i];

      uint *newOrder;
      countingSort(occs, max_v, newOrder);
      //cerr << "Totalfreq: " << totalFreq << endl;

      uint *freqsSorted = new uint[max_v+1];
      uint testTotalFreq=0;
      for (uint i=0;i<=max_v;i++){
        freqsSorted[i] =copyOccs[newOrder[i]]; 
        testTotalFreq+=freqsSorted[i] ;
      }
      for (uint i=0;i<(max_v+1)/2;i++){
        uint swp = freqsSorted[i];
        uint swp2 = newOrder[i];
        freqsSorted[i] = freqsSorted[max_v-i];
        newOrder[i] = newOrder[max_v-i];
        freqsSorted[max_v-i] = swp;
        newOrder[max_v-i] = swp2;
      }

      for (uint i=0;i<max_v;i++){
        assert(freqsSorted[i]>=freqsSorted[i+1]);
      }
      assert(totalFreq==testTotalFreq);
      
      for (uint i=0;i<=max_v;i++){
        assert(copyOccs[newOrder[i]]==freqsSorted[i]);
      }
      uint *newCodeLen = reversePackageMerge(freqsSorted, max_v+1, lmax);

      double optimality=0.0;
      assert(totalFreq>0);
      uint zeros=0;
      for (uint i=0;i<=max_v;i++){
        if (newCodeLen[i]==0){
          zeros++;
        }
      }
      assert(zeros==0);
      if (zeros!=0){
        cerr << "There are codes with codeLen=0"<<endl;
      }

      for (uint i=0;i<max_v;i++){
        if (!(newCodeLen[i]<=newCodeLen[i+1])){
          cerr << endl;
        }
      }
      table_symbols_codes = new tuple[sigma];
      minCodeLength=INT_MAX;
      maxCodeLength=0;
      for (uint i=0;i<sigma;i++){
        table_symbols_codes[i].len = newCodeLen[i];
        table_symbols_codes[i].symbol = newOrder[i];
        minCodeLength = min(minCodeLength,newCodeLen[i]);
        maxCodeLength = max(maxCodeLength,newCodeLen[i]);
      }

      
      //cerr << "zeros: " << zeros << " out of " << max_v+1 << endl;
      uint tf=0;
      for (uint i=0;i<=max_v;i++){
        assert(newCodeLen[i]>0);
        assert(newCodeLen[i]<=lmax);
        tf+=freqsSorted[i];
        optimality+=(((double)freqsSorted[i]*1.0)/(double)totalFreq)*(newCodeLen[i]);
      }
      
      // cerr << "tf: " << tf << ", n: " << len << endl;
      //cerr << "lmax: " << lmax << ";optimality: " << optimality << endl;
      buildCanonical(sigma-1, minCodeLength, maxCodeLength);

      model = mb->build(table_symbols_codes, sigma);
    }
    
    //returns the length of the code and stores in code the code of symbol
    uint wt_coder_Suboptimal::getCode(uint symbol, uint &code) const{
      uint len,leaf;
        model->getCode(symbol, code, len, leaf);
     return len;
    }

    uint wt_coder_Suboptimal::getSymbols(uint code, uint &n_symbol, vector<uint> &symbols) const{
        return model->getSymbols(code,n_symbol,symbols);
    }

    uint wt_coder_Suboptimal::getSymbol(uint code) const{
        return model->getSymbol(code);
    }

    uint wt_coder_Suboptimal::getSymbol(uint code, uint len) const{
        return model->getSymbol(code, len);
    }

    uint * wt_coder_Suboptimal::get_symbol(uint symbol) const {
        cerr << "get_symbol not implemented yet (suboptimal add)" << endl;
        return nullptr;
    }

    bool wt_coder_Suboptimal::is_set(uint symbol, uint l) const
    {
        uint code;
        code = get_field(symbol2code,maxCodeLength,symbol);         
        return (code>>l)&1;
    }

    bool wt_coder_Suboptimal::is_set(uint *symbol, uint l) const {
        
        return ((*symbol)>>l)&1;
    }

    bool wt_coder_Suboptimal::done(uint symbol, uint l) const
    {
        cerr << "not implemented yet"<<endl;
        return false;
    }

    size_t wt_coder_Suboptimal::getSize() const
    {
        return sizeof(wt_coder_Suboptimal)+model->getSize();
    }

    void wt_coder_Suboptimal::save(ofstream & fp) const
    {
        
        uint wr = WT_CODER_HUFF_CAN_HDR;
        saveValue(fp,wr);
        saveValue(fp,table_len);
        saveValue(fp,maxCodeLength);
        saveValue(fp,minCodeLength);
        saveValue(fp,nCodesAtLevel,maxCodeLength+1);
        saveValue(fp,symbol2code,table_len);
    }

    wt_coder_Suboptimal * wt_coder_Suboptimal::load(ifstream & fp) {
        uint rd = loadValue<uint>(fp);
        if(rd!=WT_CODER_HUFF_CAN_HDR) return NULL;
        wt_coder_Suboptimal * ret = new wt_coder_Suboptimal();
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

};
