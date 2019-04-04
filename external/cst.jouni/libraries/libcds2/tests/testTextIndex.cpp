/**
 *    build_index.cpp
 *    Copyright (C) 2011  Francisco Claude F.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <fmi.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <Mapper.h>
#include "timeMeasure.h"
using namespace std;
using namespace cds_static;

/*Knuth-Morris-Patt algorithm obtained from http://www-igm.univ-mlv.fr/~lecroq/string/node8.html (09/09/2014)*/
void preKmp(uint *x, int m, int kmpNext[]) {
   int i, j;

   i = 0;
   j = kmpNext[0] = -1;
   while (i < m) {
      while (j > -1 && x[i] != x[j])
         j = kmpNext[j];
      i++;
      j++;
      if (x[i] == x[j])
         kmpNext[i] = kmpNext[j];
      else
         kmpNext[i] = j;
   }
}


uint KMP(uint *x, int m, uint *y, int n) {
   int i, j, kmpNext[m*m];
   uint occs = 0;
   /* Preprocessing */
   preKmp(x, m, kmpNext);

   /* Searching */
   i = j = 0;
   while (j < n) {
      while (i > -1 && x[i] != y[j])
         i = kmpNext[i];
      i++;
      j++;
      if (i >= m) {
         occs++;
         i = kmpNext[i];
      }
   }
   return occs;
}

int main(int argc, char ** argv) {

  // uint proba[12] = {1,2,5,1,3,1,4,1,2,5,1,0};
  // int n = 12;
  // uint pat[3] = {1,2,5};
  // int m = 1;
  // cerr << "found: " << KMP(pat,m,proba,n) << endl;
  
  if(argc<4) {
    cerr << "Given a text, it builds and fmi and runs count queries with patterns of size <max_pat_length> " << endl;
    cerr << "chosen at random along the sequence. The result of the count queries" << endl;
    cerr << " is compared with the output of a Knuth-Morris-Patt algorithm." << endl;
    cout << "usage: " << argv[0] << " <input-file> <max_pat_length> <nqueries>" << endl;
    return 0;
  }

  fstream input(argv[1],ios::in | ios::binary);
  if(!input.is_open()) {
    cout << "Error opening file: " << argv[1] << endl;
    return -1;
  }
  uint max_pattern_length = atoi(argv[2]);
  uint nqueries = atoi(argv[3]);
  input.seekg(0,ios::end);
  uint n=input.tellg();

  //n = min(100000u,n);

  uchar * text = new uchar[n+1];// = new uchar[n+sizeof(uint)/sizeof(uchar)];
  uint *data = new uint[n+1];
  input.seekg(0,ios::beg);
 
  input.read((char*)text,n);
  input.close();
  text[n]=0;

  
  for (uint i=0;i<n;i++){
    data[i] = (uint)text[i]; 
  }
  Mapper *mc = new MapperCont(data,n,BitSequenceBuilderRG(32));
  uint max_v = 0;
  for (uint i=0;i<n;i++){
    data[i] = mc->map(data[i]);
    max_v = max(max_v,data[i]);
    assert(data[i]>0);
    assert(data[i]<256);
  }

  uint *occs = new uint[max_v+1];
  for (uint i=0;i<=max_v;i++)
    occs[i]=0;
  for (uint i=0;i<n;i++){
    occs[data[i]]++;
  }
  double h0 = 0.0;
  for (uint i=1;i<=max_v;i++){
    double pi = ((double)occs[i])/n;
    assert(occs[i]>0);
    h0 += pi *log(pi)/log(2);
  }
  cerr << "H0: " << h0 << endl;
  data[n]=0;
  for (uint i=0;i<n;i++){
    assert(data[i]<256);
    text[i] = (uchar)data[i];
  }
  SequenceBuilder *seq_buidler = new SequenceBuilderWaveletMatrix(new BitSequenceBuilderRG(32),new MapperNone());
  fmi * _fmi = new fmi(data,n,seq_buidler);
  _fmi->set_samplepos(32);
  _fmi->set_samplesuff(32);
  _fmi->build_index();
  _fmi->print_stats();

  uint *A,*invA;
  A = _fmi->getSA();
  invA = (uint*)_fmi->getInvSA();

  for (uint i=0;i<n;i++){
    assert(invA[A[i]]==i);
  }

  cout << "Index size: " << _fmi->size() << endl;
  cout << "Size (bps): " << _fmi->getBWTSize()*8.0 / n << endl;
  
  if (argc==6){
    uint nqueries = atoi(argv[4]);
    uint m = atoi(argv[5]);
    ifstream in(argv[3]);
    struct stat sdata;
    stat(argv[3],&sdata);
    uchar *queries = new uchar[sdata.st_size];
    in.read((char*)queries,sdata.st_size);
    in.close();
    double t0,t1,total=0.0;
      uchar *pq = queries;
      for (uint i=0;i<nqueries;i++){
        for (uint j=0;j<m;j++)
          pq[j] = mc->map(pq[j]);
        t0 = getTime();
          _fmi->count(pq,m);
        t1 = getTime();
        total = t1-t0;
        pq+=m;
        while(pq[0]!='\0')pq++;
        pq++;
      }
  cerr << "Total time: " << total/nqueries << endl;
  }

   srand(time(0));
  for (uint i=0;i<nqueries;i++){
    uint pat_len = rand()%max_pattern_length+1;
    
    uint iniPos = rand()%(n-pat_len-1);
    assert(iniPos>=0 && iniPos<=(n-pat_len-1));
    uint found_index = 0;
    found_index = _fmi->count(data+iniPos,pat_len);
    uint KMP_found = KMP(data+iniPos, pat_len, data, n);
    assert(KMP_found==found_index);
  } 

  cerr << "Test Ok!" << endl;


  //delete sbb;
  delete _fmi;
  delete [] text;

  return 0;
}

