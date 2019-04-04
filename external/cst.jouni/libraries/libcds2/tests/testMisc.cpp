
#include <cstdlib>

#include <utils/libcdsBasics.h>
#include <bitsequence/BitSequence.h>
#include <mapper/Mapper.h>
#include <sequence/Sequence.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <trees/Parentheses.h>
#include <iostream>
#include <rmq/RMQ.h>
#include <utils/BinarySearchTree.h>
#include <sequence/SequenceBuilder.h>
#include <bitsequence/BitSequenceBuilder.h>
#include "timeMeasure.h"
#include <../../external/lzisa/includes/LZISA.h>
#include <utils/StringUtils.h>

using namespace std;
using namespace cds_static;
using namespace cds_lzisa;

void saveLoadSequence(Sequence *&seq){
  ofstream fp("tmp.bin");
  seq->save(fp);
  fp.close();
  delete seq;
  ifstream in("tmp.bin");
  seq=Sequence::load(in);
  in.close();
}

bool testBST(BinarySearchTree<uint> *node){
  if (!node) return true;
  if (node->getLeft()){
    assert(node->getLeft()->getValue()<node->getValue());
    assert(testBST(node->getLeft()));
  }
  if (node->getRight()){
    assert(node->getRight()->getValue()>node->getValue());
    assert(testBST(node->getRight()));
  }
  return true;
}
int do_testBST(int argc, char **argv){
  BinarySearchTree<uint> *bst = new BinarySearchTree<uint>();
  assert(!bst->isInitialized());
  assert(bst->searchKey(0)==nullptr);

  uint len=8;
  uint values[8] = {2,1,7,3,4,10,8,11};
  bst->insertKey(2);
  assert(bst->isInitialized());
  assert(bst->getValue()==2);

  for (uint i=1;i<len;i++){
    bst->insertKey(values[i]);
  } 
  //check if the tree is correct
  assert(bst->getRoot()->getValue()==2);
  assert(bst->getRoot()->getLeft()->getValue()==1);
  assert(bst->getRoot()->getRight()->getValue()==7);
  assert(bst->getRoot()->getRight()->getLeft()->getValue()==3);
  assert(bst->getRoot()->getRight()->getLeft()->getRight()->getValue()==4);
  assert(bst->getRoot()->getRight()->getRight()->getValue()==10);
  assert(bst->getRoot()->getRight()->getRight()->getLeft()->getValue()==8);
  assert(bst->getRoot()->getRight()->getRight()->getRight()->getValue()==11);
  
  //check if it is a BST
  assert(testBST(bst));
  for (uint i=0;i<len;i++){
    BinarySearchTree<uint> *b = bst->searchKey(values[i]);
    assert(b->getValue()==values[i]);    
  }
  //those not inserted keys are not found
  assert(bst->searchKey(0)==nullptr);
  assert(bst->searchKey(5)==nullptr);
  assert(bst->searchKey(6)==nullptr);
  assert(bst->searchKey(9)==nullptr);
  assert(bst->searchKey(12)==nullptr);

  //Testing sucessor
  assert(bst->successor(0)->getValue()==1);
  assert(bst->successor(1)->getValue()==1);
  assert(bst->successor(2)->getValue()==2);
  assert(bst->successor(3)->getValue()==3);
  assert(bst->successor(4)->getValue()==4);
  assert(bst->successor(5)->getValue()==7);
  assert(bst->successor(8)->getValue()==8);
  assert(bst->successor(9)->getValue()==10);
  assert(bst->successor(11)->getValue()==11);
  assert(bst->successor(12)==nullptr);
  cerr << "Test BST OK" << endl;

  return 0;
}

int do_DACInc(int argc, char **argv){
  size_t len = 1024; 
  uchar *input = fibonacciWord(len);
  uint b = 64;
  uint *in = new uint[len];
  for (size_t i=0;i<len;i++)
    in[i]=(input[i]=='0')?0:1;
  
  uint *counter = new uint[len];
  uint c = 0;
  for (size_t i =0; i < len;i++){
    if (in[i]==0) c++;
    counter[i]=c;
  }

  DirectAccess *d = new DACInc(counter, len, 10, false);
  for (uint i=0;i<len;i++){
    assert(d->access(i)==counter[i]);
  }
  cerr << "Test OK" << endl;
  delete [] in;
  delete [] counter;
  delete [] input;
  delete d;
  return 0;
}

int do_testRMQ(int argc, char **argv){
  size_t len = 15;
  int input[15] = {3,2,7,1,9,14,0,4,13,12,5,11,6,10,8};

  RMQ *r = new RMQ(input,len,RMQ_MIN);
 srand(0);
  cerr << r->getPos(0, 15-1) << endl;
  for (uint i=0;i<100;i++){
   uint ini = rand()%(len-2);
   uint fin = ini+1+rand()%(len-ini-1);
   assert(ini<fin);
   assert(ini<len-1);
   assert(fin<len);
   uint p = r->getPos(ini,fin);
   uint m,posm;
   m=input[ini];
   posm=ini;
    
   for (uint j=ini+1;j<=fin;j++){
     if (input[j]<input[posm]){
       m=input[j];
       posm=j;
     }
   }
   assert(p == posm);

  }

  int max_v=0;
  for (uint i=0;i<len;i++){
    max_v = max(max_v,input[i]);
  }
  int *new_input =new int[len];
  for (uint i=0;i<len;i++){
    new_input[i] = max_v-input[i];
  }
  RMQ *rmax = new RMQ(new_input,len,RMQ_MAX);
 srand(0);
  cerr << r->getPos(0, 15-1) << endl;
  for (uint i=0;i<100;i++){
   uint ini = rand()%(len-2);
   uint fin = ini+1+rand()%(len-ini-1);
   assert(ini<fin);
   assert(ini<len-1);
   assert(fin<len);
   uint p = rmax->getPos(ini,fin);
   uint m,posm;
   m=new_input[ini];
   posm=ini;
   // assert(rmax->getValue(p)==new_input[p]);
   for (uint j=ini+1;j<=fin;j++){
     if (new_input[j]>new_input[posm]){
       m=new_input[j];
       posm=j;
     }
   }
   assert(p == posm);

  }

  cerr << "Test RMQ OK" << endl;

  return 0;
}



int do_testGrammar(int argc, char **argv){
  
  if (argc<2){
    cerr << "Usage: " << argv[0] << " <input-file.txt>" << endl;
    return 0;
  }

 // struct stat sdata;
 //  stat(argv[1],&sdata);
 //  uint len = sdata.st_size;//it is adding a blank space at the end..i do not want it-->i use it for the $.
 //  char *input_text = new char[len];
  
 //  ifstream iff(argv[1]);
 //  iff.read((char*)input_text,len);
 //  iff.close();

 //  uint *input = new uint[len];//
 //  for (uint i=0;i<len;i++)
 //    input[i] = (uint)input_text[i];

      //Fibonacci bitVectors
    size_t len = 100000000; 
    uchar *input_text = fibonacciWord(len);
    uint b = atoi(argv[1]);
    uint *input = new uint[len];
    for (size_t i=0;i<len;i++)
      input[i]=(input_text[i]=='0')?0:1;
  //len-1? at the very end position,the editor is adding something. We do not 
  //want that something. 
  Mapper *mp = new MapperCont(input, len-1, BitSequenceBuilderRG(32));
  uint dic[256];
  for (uint i=0;i<256;i++)
    dic[i]=0;
  for (uint i=0;i<len-1;i++){
    uint v = mp->map(input[i]);
    dic[input[i]]=v;
    input[i]= v;
    assert(input[i]>0);
  }
  input[len-1]=0;

  Repair *rp = new Repair((int*)input,len);
  cerr << "rp rules: " << rp->getNRules() << endl;
  cerr << "C length: " << rp->getNC() << endl;
  uint sampleC=4;
  uint sampleR=1;

    SequenceBuilder *seqClasses = new SequenceBuilderSequenceRepairSC(new BitSequenceBuilderRRR(32),
        new MapperNone(),new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)),sampleC,sampleR,8u);
    
    SequenceBuilder *seqFirstClasses = new SequenceBuilderSequenceRepairSN(new BitSequenceBuilderRRR(32),
        new MapperNone(),new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)),sampleC,sampleR,8u);
    //~ BitSequenceBuilder **bitseqs = new BitSequenceBuilder*[2];
    //~ bitseqs[0] = new BitSequenceBuilderRRR(32);
    //~ bitseqs[1] = new BitSequenceBuilderRPSC(new BitSequenceBuilderRRR(32),sampleC,sampleR,8u);
    //~ 
  //~ SequenceBuilder *seqClasses = new SequenceBuilderWTNoptrsHybrid(bitseqs,2);
  //~ SequenceBuilder *seqFirstClasses = new SequenceBuilderWTNoptrsHybrid(bitseqs,2);
  //~ SequenceBuilder * sb2 = new SequenceBuilderGMRChunk(new BitSequenceBuilderRG(20),new PermutationBuilderMRRR(20,new BitSequenceBuilderRG(20)));
  //~ SequenceBuilder * sb3 = new SequenceBuilderGMR(new BitSequenceBuilderRG(20), sb2);
  //~ SequenceBuilder *seqOffsets = sb3;
    //SequenceBuilder *seqOffsets = new SequenceBuilderWaveletMatrix(new BitSequenceBuilderRG(128), new MapperNone());
    SequenceBuilder *seqOffsets = new SequenceBuilderWaveletMatrix(new BitSequenceBuilderRPSC(new BitSequenceBuilderRRR(32),8,4,8u), new MapperNone());
    
    //~ seq = new SequenceAlphPartRep(data,len,5,6,seqClasses,seqOffsets,seqFirstClasses,new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)));
    //seq = new SequenceAlphPartRep(data,len,5,6,seqClasses,seqOffsets,seqFirstClasses,new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)));

   Sequence* seq = new SequenceAlphPartRepReg(input,len,10,6,7,seqClasses,seqOffsets,seqFirstClasses,new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)));

   cerr << "Seq size (bps): " << seq->getSize()*8.0 / len << endl;

   cerr << "Seq size (bps): " << seq->getSize()*8.0 / len << endl;
 
  // t0 = getTime();
  // for (uint i=0;i<nqueries;i++){
  //   // cerr << "i: " << i << endl;
  //   uint acc = seq->access(i);
  //   assert(acc == (input[i]-1));
  // }
  // t1 = getTime();
  // cerr << "Test access OK!" << endl;
  // cerr << "Time per query: " << (t1-t0)*1000000.0/(nqueries) << endl;
  uint *counter = new uint[256];
  for (uint i=0;i<256;i++){
    counter[i]=0;
  }
  uint nqueries = min((uint)len-1,1000u);
  double t0,t1; 
  uint *queries = new uint[nqueries];
  for (uint i=0;i<nqueries;i++){
    queries[i] = rand()%(len-1);
  }

  // t0 = getTime();
  // for (uint i=0;i<nqueries;i++){
  //   uint acc = seq->access(queries[i]);
  // }
  // t1 = getTime();
  // cerr << "Time per query: " << (t1-t0)*1000000.0/(nqueries) << endl;
  t0 = getTime();
  for (uint i=0;i<nqueries;i++){
      seq->rank(input[queries[i]]-1, queries[i]);
    // seq->rank(input[i]-1, i);
  }

    // for (uint i=0;i<nqueries;i++){
      // counter[input[i]]++;
       // cerr << i << endl;
     
      // uint r = seq->rank(input[i]-1, i);
      // if (!(r==counter[input[i]]))
      //   cerr << "(i:" << i << "), obtained: " << r << ", expected: " << counter[input[i]] << endl;
      // assert(r==counter[input[i]]);
    // }
  t1 = getTime();
  delete [] queries;
  cerr << "Time per query: " << (t1-t0)*1000000.0/nqueries << endl;
  cerr << "Test rank OK!"<< endl;
  delete [] input_text;
  delete [] input;
  return 0;  
}

uint testLZISA(int argc, char **argv){
    if (argc<2){
        cerr << "Usage: " << argv[0] << " <input.txt> [<true> if the input is a bitvector]" << endl;
        return -1;
    }
    char *in_file = argv[1];
    if (argc==3){
        ifstream fp(in_file);
        size_t len;
        fp.read((char*)&len,sizeof(size_t));
        uint *input;
        createEmptyBitmap(&input, len);
        fp.read((char*)input,uint_len(len,1)*sizeof(uint));
        fp.close();
        char *salida = new char[len];
        for (size_t i = 0; i < len;i++){
            if (bitget(input,i)){
                salida[i]='1';
            }else{
                salida[i]='0';
            }
        }

        char *out = new char[strlen(in_file)+2048];
        strcpy(out, in_file);
        strcat(out,".char");
        ofstream of(out);
        of.write(salida,sizeof(char)*len);
        of.close();
        delete [] out;
        delete [] salida;
        in_file = out;
    }

    double t0,t1;
    t0 = getTime();
    cerr << "ldap" << endl;
    LZISA *a = LZISA::getInstance9(argv[1]);
    t1 = getTime();
    cerr << "Time: " << t1-t0 << endl;
    cerr << "done!" << endl;
    return 0;
}
int main(int argc, char ** argv) {
  
  //return do_testBST(argc, argv);
  //return do_testLZEndCorrectness(argc, argv);
  // return do_testLZEndPerformance(argc, argv);
  // return do_testGrammar(argc, argv);
  //return do_testRMQ(argc,argv);
   // return testLZISA(argc,argv);
  //return do_testLZBlockPerformance(argc,argv);
  //return do_DACInc(argc,argv);
   // return testLZISA(argc,argv);

}
// int main(int argc, char ** argv) {


// }

// int main(int argc, char ** argv) {

// 	if (argc<3){
// 		cerr << "***Usage: " << argv[0] << " input.parentheses output" << endl;
// 		exit(0);
// 	}
//   size_t len;
//   ifstream iff(argv[1]);
//   //len = iff.tellg();
//   iff.seekg(0,ios::end);
//   len=iff.tellg();
//   iff.seekg(0);
//   assert(iff.tellg()==0);
//   char * input = new char[len];
//   input = loadValue<char>(iff,len);

//   uint lim = 10000000;
//   int excess = 0;
//   uint i;
//   for (i=0;i<lim;i++)
//     excess += (input[i]=='0')?-1:1;
//   while(excess){
//     input[i++]='0';
//     excess--;
//   }
//   excess = 0;
//   lim = i;
//   for (i=0;i<lim;i++)
//     excess += (input[i]=='0')?-1:1;
//   assert(excess==0);

//   ofstream off(argv[2]);
//     saveValue(off, input,lim);
//   off.close();
//   assert(i>=lim);
//   delete [] input;
// 	return 0;
// }
