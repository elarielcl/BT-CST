
#include <cstdlib>

#include <libcdsBasics.h>
#include <BitSequence.h>
#include <Mapper.h>
#include <Sequence.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <Parentheses.h>
#include <iostream>
#include <cstdio>

using namespace std;
using namespace cds_static;

double getTime (void){
	
   double usertime;
   struct rusage usage;

   getrusage (RUSAGE_SELF, &usage);

   usertime = (double) usage.ru_utime.tv_sec +
       (double) usage.ru_utime.tv_usec / 1000000.0;

   return (usertime );
}

uint getExcess(BitSequence &bs, size_t pos){
    return pos-2*bs.rank0(pos)+1;
}

void testSequence(uint *a, uint n, Parentheses & p) {
   
   BitSequence *bs = new BitSequenceRG(a, (size_t)n, 16);

}

bool resEQ(uint index, uint res1, uint res2, char *s = NULL){
  if (res1!=res2){
    cerr << "index " << index << ", res1: " << res1 << ", res2: " << res2 ;
    if (s){
      cerr << " "<< s;
    }
    cerr << endl;
  }
  return res1==res2;
}

Parentheses * saveLoadFILE(Parentheses * bs) {
    //ofstream ofs("parentheses.tmp");
    FILE *f = fopen("parentheses.tmp","wb");
    bs->save(*f);
    fclose(f);
    FILE * ifs = fopen("parentheses.tmp","rb");
    Parentheses * ret = Parentheses::load(*ifs);
    fclose(ifs);
    return ret;
}
Parentheses * saveLoad(Parentheses * bs) {
    ofstream ofs("parentheses.tmp");
    bs->save(ofs);
    ofs.close();
    ifstream ifs("parentheses.tmp");
    Parentheses * ret = Parentheses::load(ifs);
    ifs.close();
    return ret;
}

void testParentheses(uint *input, size_t len) {
	BitSequenceRG bs(input,len,16);
    int ini=1000;
    int fin = 40000;
    uint excess = getExcess(bs,ini-1);
    uint minE = excess;
    uint minPos = ini;

    for (uint i=ini;i<fin;i++){
        excess+=(bs.access(i))?1:-1;
        if (excess<minE){
            minE = excess;
            minPos = i;
        }
    }
    excess = 0;
    for (uint i=0;i<=minPos;i++){
        excess+=(bitget(input,i))?1:-1;
    }
    cerr << "excess: " << excess << endl;
    cerr << "M using bitmap: " << getExcess(bs, minPos) << endl;
    cerr << "M: " << minE << " at pos " << minPos << endl;
	
}
#if 0
int main(int argc, char ** argv) {  

  if (argc<3){
    cerr << "***Usage: " << argv[0] << " input.parentheses (Format: [1 bit/ bit len times]) <type: 0:BP, 1:GCT>" << endl;
    exit(0);
  }
  uint *bmp;
  ifstream iff(argv[1]);
  
  struct stat sdata;
  stat(argv[1],&sdata);
  size_t len = sdata.st_size;
  uint *new_bmp;
  uint nints = uint_len(len,1);
  uint *buffer = new uint[nints];
  iff.read((char*)buffer,len);
  iff.close();
  uint n = 0;
  assert(bitget(buffer,0));
  uint excess=1;
  for (size_t i=0;i<len;i++){
    if (bitget(buffer,i)){
      excess++;
    }else{
      excess--;
    }
    if (excess==0){
      n = i;
      break;
    }
  }

  Parentheses *ps = GCT::newGCTFromRevBitmap(1,10,buffer,n);
  cerr << "ps (bpb): " << ps->getSize()*8.0 / n << endl;

  return 0;
}
#endif
#if 1
int main(int argc, char ** argv) {  

  if (argc<3){
    cerr << "***Usage: " << argv[0] << " input.parentheses (Format: [len;1 bit/ bit len times]) <type: 0:BP, 1:GCT>" << endl;
    exit(0);
  }
  uint *bmp;
  ifstream iff(argv[1]);
  uint len = loadValue<uint>(iff);
  bmp = loadValue<uint>(iff,uint_len(len,1));

  Parentheses *ps = NULL;
  Parentheses *bp = NULL;
  // uint *new_bmp;
  // createEmptyBitmap(&new_bmp, len);
  // uint nints = uint_len(len,1);
  // for (uint i=0;i<nints;i++){
  //   uint _W = W/2;
  //   for (uint j=0;j<_W;j++){
  //     uint l = bitget(bmp+i,W-j-1);
  //     uint r = bitget(bmp+i,j);
  //     if (l){
  //       bitset(new_bmp+i,j);
  //     }else{
  //       bitclean(new_bmp+i,j);
  //     }
  //     if(r){
  //       bitset(new_bmp+i,W-j-1);
  //     }else{
  //       bitclean(new_bmp+i,W-j-1);
  //     }
  //   }
  // }
  // for (uint i=0;i<5;i++){
  //   uint b = (bitget(inputBmp,i))?1:0;
  //   cerr << "bit-"<<i << ": "<< b << endl;
  // }
  
  // ps = new GCT(1,12,bmp,len);
  // cerr << "ps (bpb): " << ps->getSize()*8.0 / len << endl;
  //ps = GCT::newGCTFromRevBitmap(1,10,bmp,len);
  //cerr << "ps->getSize()" << ps->getSize()*8.0/ps->getLength() << endl;
  bp = new BP(bmp,(size_t)len,(uint)OPT_DEGREE);
  
  cerr << "bp->getSize()" << bp->getSize()*8.0/ps->getLength() << endl;
  // for (uint i=0;i<len;i++){
  //   uint resps,resbp;
  //   if (bitget(bmp,i)){
  //     resps = ps->inspect(i);
  //     resbp = bp->inspect(i);
  //     if (resps!=resbp){
  //       cerr << "Diferentes access"<< i << endl;
  //     }
  //   }
  // }
  // cerr << "Inpsect OK" << endl;
  double t0, t1;
  t0 = getTime();
  for (uint i=0;i<len;i++){
    uint resps,resbp;
    if (bitget(bmp,i)){
      //resps = ps->firstChild(i);
      resbp = bp->firstChild(i);
      // if (resps!=resbp){
      //   cerr << "Diferentes "<< i << endl;
      // }
    }
  }
  t1 = getTime();
  cerr << "Time: " << 2*(t1-t0)*1000000.0/len << endl;
  cerr << "First Child OK" << endl;
  // for (uint i=0;i<len;i++){
  //   uint resps,resbp;
  //   if (bitget(bmp,i)){
  //     resps = ps->findClose(i);
  //     resbp = bp->findClose(i);
  //     if (!resEQ(i, resps, resbp,"findClose")){
  //       resps = ps->findClose(i);
  //       resbp = bp->findClose(i);
  //       abort();
  //     }
  //   }
  // }
  t0 = getTime();
  for (uint i=0;i<len;i++){
    uint resps,resbp;
    if (bitget(bmp,i)){
      //resps = ps->nextSibling(i);
       resbp = bp->nextSibling(i);
      // if (resps!=resbp){
      //   cerr << "Diferentes "<< i << endl;
      // }
    }
  }
  t1 = getTime();
  cerr << "Time: " << 2*(t1-t0)*1000000.0/len << endl;
  // ps = new GCT(1,4,bmp,len);
  // Parentheses *ps2 = new BP(bmp,len);
  // Parentheses *naive = new BPNaive(bmp,len);
  // uint excess=0;
  // uint E[2];E[0]=-1;E[1]=1;
  // for (uint i=0;i<len;i++){
  //   uint bit = (bitget(bmp,i))?1:0;
  //   excess+=E[bit];
  //   //cerr << bit <<"("<<i<<",e="<<excess<< ")"<< endl;
  //   if (!bit){
  //     uint a,b;
  //     a = naive->findOpen(i);
  //     b = ps->findOpen(i);
  //     if (!(a==b )){
  //       cerr << naive->findOpen(i) << ", "<<endl;
  //       cerr  << ps->findOpen(i) << endl;
  //     }
  //     assert(naive->findOpen(i)==ps->findOpen(i));
  //   }
  // }
  //ps = saveLoad(ps);
  
  //naive = saveLoadFILE(naive);
  // cerr << "findclose(0): " << naive->findClose(0) << endl;
  // cerr << "findclose(0): " << ps->findClose(0) << endl;
  
  
  // int m;
  // cerr << "rmq: " << naive->rmq(1000u,40000u,m)<< endl;
  // cerr << "rmq: " << ps->rmq(1000u,40000u,m)<<endl;
  // cerr << "ps->size(): " << ps->getSize()*8.0/ps->getLength() << endl;
  // delete [] bmp;
  
  // delete ps;
  // delete naive;
  return 0;
}
#endif
#if 0
int main(int argc, char ** argv) {

	if (argc<3){
		cerr << "***Usage: " << argv[0] << " input.parentheses (1 char/ bit) <type: 0:BP, 1:GCT>" << endl;
		exit(0);
	}
	struct stat sdata;
	stat(argv[1],&sdata);
	char *input = new char[sdata.st_size];
	FILE *fin = fopen(argv[1],"rb");
	fread(input, sdata.st_size, 1, fin);
	fclose(fin);
	uint *inputBmp;
	createEmptyBitmap(&inputBmp, sdata.st_size);
    size_t i;
	for (i=0;i<sdata.st_size;i++){
		if (input[i]=='1'){
			bitset(inputBmp,i);
		}else if (input[i]!='0'){
			break;
		}
	}
    size_t len = i;
	delete [] input;
  uint option = atoi(argv[2]);
  Parentheses *ps = NULL;
  // for (uint i=0;i<5;i++){
  //   uint b = (bitget(inputBmp,i))?1:0;
  //   cerr << "bit-"<<i << ": "<< b << endl;
  // }
  
  if (option==0){
    uint *inputRevert;
    createEmptyBitmap(&inputRevert, len);
    uint nints = uint_len(len, 1);
    for (uint i=0;i<nints;i++){
      for (uint j=0;j<W;j++){
        uint f,l;
        f = bitget(inputBmp+i,j);
        l = bitget(inputBmp+i,W-j-1);
        if (f) bitset(inputRevert+i, W-j-1);
        if (l) bitset(inputRevert+i, j);
      }
    }
    ps = new BP (inputRevert,len);
    delete [] inputRevert;
  }else if (option==1){
    ps = new GCT(1,10,inputBmp,len);
  }else{
    cerr << "option " << option << " not know. Valid values are 0 for BP, 1 for GCT" << endl;
    abort();
  }
  ps = saveLoad(ps);
  Parentheses *naive = new BPNaive(inputBmp,len);
  //naive = saveLoadFILE(naive);
  cerr << "findclose(0): " << naive->findClose(0) << endl;
  cerr << "findclose(0): " << ps->findClose(0) << endl;
  int m;
  cerr << "rmq: " << naive->rmq(1000u,40000u,m)<< endl;
  cerr << "rmq: " << ps->rmq(1000u,40000u,m)<<endl;
  delete [] inputBmp;
  
  delete ps;
  delete naive;
  return 0;
}

#endif