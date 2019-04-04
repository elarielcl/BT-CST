
#include <cstdlib>

#include <libcdsBasics.h>
#include <BitString.h>
#include <BitSequence.h>
#include <BitSequenceRG.h>
#include <BitSequenceRRR.h>
#include <BitSequenceSDArray.h>
#include <BitSequenceBuilder.h>
#include <PermutationBuilder.h>
#include "timeMeasure.h"


using namespace std;
using namespace cds_utils;
using namespace cds_static;

BitSequence * saveLoad(BitSequence * bs) {
    ofstream ofs("bitsequence.tmp");
    bs->save(ofs);
    ofs.close();
    ifstream ifs("bitsequence.tmp");
    BitSequence * ret = BitSequence::load(ifs);
    ifs.close();
    return ret;
}
bool testTimeSelect(BitString & a, BitSequence * bs) {
  uint n0,n1;
  n0 = bs->rank0(bs->getLength()-1);
  n1 = bs->getLength()-n0;
  for (uint i=0;i<n0;i++){
    bs->select0(i+1);
  }
  for (uint i=0;i<n1;i++){
    bs->select1(i+1);
  }

  return true;
}
bool testBitSequence(BitString & a, BitSequence * bs) {
  size_t rank0SoFar = 0;
  size_t rank1SoFar = 0;
  for(size_t i=0; i<a.getLength(); i++) {
    if(a[i]) {
        rank1SoFar++;
        if(bs->select1(rank1SoFar)!=i) {
            cerr << "SELECT1 ERROR " << i << endl;
            cerr << "rank1SoFar: " << rank1SoFar << endl;
            cerr << "obtained: " << bs->select1(rank1SoFar) << endl;
            return false;
        }
        //~ if(i>0 && bs->selectNext1(i)!=i) {
            //~ cout << "i=" << i << "sn=" << bs->selectNext1(i) << endl;
           //~ cerr << "SELECTNEXT1 ERROR" << endl;
           //~ return false;
        //~ }
    } else {
        rank0SoFar++;
        if (rank0SoFar==97)
          cerr << endl;
        if(bs->select0(rank0SoFar)!=i) {
           cerr << "SELECT0 ERROR " << rank0SoFar << endl;
           return false;
        }
        //~ if(i>0 && bs->selectNext0(i)!=i) {
           //~ cerr << "SELECTNEXT0 ERROR" << endl;
           //~ return false;
        //~ }
    }
    if(bs->rank1(i)!=rank1SoFar){ 
        cerr << "RANK1 ERROR. Expected: " << rank1SoFar << ", obtained: " << bs->rank1(i) << endl;
        return false;
	}
    if(bs->rank0(i)!=rank0SoFar){ 
        cerr << "RANK0 ERROR. Expected: " << rank0SoFar << ", obtained: " << bs->rank0(i) << endl;
        return false;
	}
    if(bs->access(i)!=a[i]){ 
        cerr << "ACCESS ERROR. Expected: " << a[i] << ", obtained: " << bs->access(i) << endl;
        return false;
	}
  size_t r;
  uint acc;
    acc = bs->access(i,r);
    if (acc!=a[i]){
     cerr << "ACCESS-RANK ERROR. Expected at pos: "<< i << " is " << a[i] << ", obtained: " << acc << endl;
        return false;   
    }
    if (acc){
      if (r!=rank1SoFar){
        cerr << "ACCESS-RANK1 ERROR. Expected at pos: "<< i << " is " << rank1SoFar << ", obtained: " << r << endl;
        return false;
      }
    }else{
      if (r!=rank0SoFar){
        cerr << "ACCESS-RANK0 ERROR. Expected at pos: "<< i << " is "  << rank0SoFar << ", obtained: " << r << endl;
        return false;
      }
    }
  }
  return true;
}



bool testBitSequence2(BitString & a, BitSequence * bs) {
  size_t rank0SoFar = 0;
  size_t rank1SoFar = 0;
  for(size_t i=0; i<a.getLength(); i++) {
    if(a[i]) {
        rank1SoFar++;
        if(bs->select1(rank1SoFar)!=i) {
            cerr << "SELECT1 ERROR " << i << endl;
            return false;
        }
        if(i>0 && bs->selectNext1(i)!=i) {
            cout << "i=" << i << "sn=" << bs->selectNext1(i) << endl;
           cerr << "SELECTNEXT1 ERROR" << endl;
           return false;
        }
    } else {
        rank0SoFar++;
        if(bs->select0(rank0SoFar)!=i) {
           cerr << "SELECT0 ERROR" << endl;
           return false;
        }
        if(i>0 && bs->selectNext0(i)!=i) {
           cerr << "SELECTNEXT0 ERROR" << endl;
           return false;
        }
    }
    if(bs->rank1(i)!=rank1SoFar) 
        return false;
    if(bs->rank0(i)!=rank0SoFar)
        return false;
    if(bs->access(i)!=a[i])
        return false;
  }
  return true;
}
int main(int argc, char ** argv) {

  if(argc!=4) {
    cout << "Checks the bitsequence classes generating <length> bitmaps with density <dens> using <seed> as seed for the numbers generation" << endl << endl;
    cout << "usage: " << argv[0] << " <seed> <length> <dens>" << endl;
    return 0;
  }

  srand(transform(string(argv[1])));
  uint len = transform(string(argv[2]));
  uint dens = transform(string(argv[3]));

  uint act_dens = 0;
  BitString a(len);
  uint *data = new uint[len/W+1];
  for (uint i=0;i<len/W+1;i++)
	data[i]=0;
  while(act_dens < dens) {
      size_t pos = rand()%len;
      if(!a[pos]) {
          act_dens++;
          a.setBit(pos,true);
      }
  }
  
  for (uint i=0;i<len;i++)
	if (a.getBit(i))
		bitset(data,i);
		//~ 
  //~ BitSequenceRG bsRG(a,20);
  //~ BitSequence * s = saveLoad(&bsRG);
  //~ if(!testBitSequence(a,s)) {
      //~ cerr << "ERROR TESTING BitSequenceRG" << endl;
      //~ return -1;
  //~ }
	//~ cout << "RG OK\n" << endl; 
  //~ delete (BitSequenceRG *)s;
//~ 
	//~ BitSequenceDArray bsDArray(a);
	//~ s = saveLoad(&bsDArray);
	//~ if(!testBitSequence(a,s)) {
		//~ cerr << "ERROR TESTING BitSequenceDArray" << endl;
		//~ return -1;
	//~ }
	//~ cout << "DArray OK\n" << endl; 
	//~ delete (BitSequenceDArray *)s;
//~ 
	//~ BitSequenceSDArray bsSDArray(a);
  //~ s = saveLoad(&bsSDArray);
  //~ if(!testBitSequence(a,s)) {
      //~ cerr << "ERROR TESTING BitSequenceSDArray" << endl;
      //~ return -1;
  //~ }
	//~ cout << "SDArray OK\n" << endl;
  //~ delete (BitSequenceSDArray *)s;
// BitSequence * s;
	//~ BitSequenceRRR bsRRR(a,33);
  //~ s = saveLoad(&bsRRR);
  //~ if(!testBitSequence(a,s)) {
      //~ cerr << "ERROR TESTING BitSequenceRRR" << endl;
      //~ return -1;
  //~ }
	//~ cout << "RRR OK\n" << endl;
  //~ delete (BitSequenceRRR *)s;
  
  BitSequence * s;
	// BitSequenceRPSC bsRPSC(data,len,nullptr,new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)),4,2,8u);
 //  s = saveLoad(&bsRPSC);
   BitSequenceRPSN bsRPSN(data,len,nullptr,new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)),10,2,8u);
   s = saveLoad(&bsRPSN);

 //   assert(bsRPSN.getLength()==len);
 //   uint z = bsRPSN.rank1(bsRPSN.getLength()-1);
 //   size_t expect = -1;
 //   //assert(expect == bsRPSN.select1(z+1));
  if(!testBitSequence(a,s)) {
      cerr << "ERROR TESTING BitSequenceRP" << endl;
      return -1;
  }
	cout << "Test RPSC OK\n" << endl;
  // BitSequencePlain bsPlain(data,len,33);
  //  s = saveLoad(&bsPlain);
  // if(!testBitSequence(a,s)) {
  //     cerr << "ERROR TESTING BitSequenceRRR" << endl;
  //     return -1;
  // }
  // cout << "PlainOK\n" << endl;
  // BitSequencePlain *s = (BitSequencePlain::builder).
  //             s1factor(32).s0factor(32).build();//(a.getData(),a.getLength(),33);
  //int *x = BitSequencePlain::ldap;
  
  //BitSequencePlain *s1 = BitSequencePlain::BuilderSequencePlain::createBuilderSequencePlain(a.getData(),a.getLength(),33)->s1factor(32).s0factor(32).build();
  //BitSequencePlain *s1 = BitSequencePlain::BuilderSequencePlain::createBuilderSequencePlain(a.getData(),a.getLength(),33)->build();

  //BitSequencePlain *s = new BitSequencePlain (a.getData(),a.getLength(),33);
  //double t0=getTime(); 
  // if(!testBitSequence(a,s1)) {
  //     cerr << "ERROR TESTING BitSequenceRRR" << endl;
  //     return -1;
  // }
  //testTimeSelect(a, s1);
  // double t1= getTime();
  // cerr << "Time: " << (t1-t0)*1000000/a.getLength()<<endl;
  // cout << "PlainOK\n" << endl;
  return 0;
}

