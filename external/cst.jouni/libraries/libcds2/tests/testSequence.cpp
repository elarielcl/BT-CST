
#include <cstdlib>

#include <libcdsBasics.h>
#include <BitSequence.h>
#include <Mapper.h>
#include <Sequence.h>
#include <Repair.h>
using namespace std;
using namespace cds_static;


void testSequence(Array & a, Sequence & bs) {
    ofstream outfs("sequence.tmp");
    bs.save(outfs);
    outfs.close();
    ifstream infs("sequence.tmp");
    Sequence * seq = Sequence::load(infs);
    infs.close();
    
    uint maxv = a.getMax();
    size_t count[maxv+1];
    for(size_t i=0;i<=maxv;i++)
        count[i] = 0;
    for(size_t i=0;i<a.getLength();i++) {
        count[a[i]]++;
        if(a[i]!=seq->access(i)) {
            cerr << "ERROR ACCESS" << endl;
            cerr << "(pos: " << i << "), Got:" << seq->access(i) << " Expected:" << a[i] << endl;
            exit(-3);
        }
        for(uint j=a[i];j<=a[i];j++) {
            if(seq->rank(j,i)!=count[j]) {
                cerr << "ERROR RANK " << endl;
                cerr << " Rank result: " << bs.rank(j,i) << " count=" << count[j] << endl;
                cerr << " symbol=" << j << " position=" << i << endl;
                exit(-1);
            }
        }
        if(seq->select(a[i],count[a[i]])!=i) {
            cerr << "ERROR SELECT " << endl;
            cerr << "a[i]=" << a[i] << " maxv=" << maxv << endl;
            cerr << "bs.select=" << bs.select(a[i],count[a[i]]) << " i=" << i << endl;
            exit(-2);
        }

    }
    cout << "Tests OK" << endl;
    delete seq;
}


void testSequence2(uint *a, uint n, Sequence & bs) {
    ofstream outfs("sequence.tmp");
    bs.save(outfs);
    outfs.close();
    ifstream infs("sequence.tmp");
    Sequence * seq = Sequence::load(infs);
    infs.close();
    uint maxv = 0;
    for (uint i=0;i<n;i++) if (a[i]>maxv) maxv = a[i];
    size_t count[maxv+1];
    for(size_t i=0;i<=maxv;i++)
        count[i] = 0;
    for(size_t i=0;i<n;i++) {
        count[a[i]]++;
        cout << i << endl;
        if(a[i]!=seq->access(i)) {
            cerr << "ERROR ACCESS" << endl;
            cerr << "(pos: " << i << "), Got:" << seq->access(i) << " Expected:" << a[i] << endl;
            exit(-3);
        }
        for(uint j=a[i];j<=a[i];j++) {
            if(seq->rank(j,i)!=count[j]) {
                cerr << "ERROR RANK " << endl;
                cerr << " Rank result: " << bs.rank(j,i) << " count=" << count[j] << endl;
                cerr << " symbol=" << j << " position=" << i << endl;
                exit(-1);
            }
        }
        if(seq->select(a[i],count[a[i]])!=i) {
            cerr << "ERROR SELECT " << endl;
            cerr << "a[i]=" << a[i] << " maxv=" << maxv << endl;
            cerr << "bs.select=" << bs.select(a[i],count[a[i]]) << " i=" << i << endl;
            exit(-2);
        }

    }
    delete seq;
}


int main(int argc, char ** argv) {

  if(argc!=4) {
    cout << "Checks the array class generating <length> elements between 0 and <maxv> using <seed> as seed for the numbers generation" << endl << endl;
    cout << "usage: " << argv[0] << " <seed> <length> <maxv>" << endl;
    return 0;
  }

  srand(transform(string(argv[1])));
  uint len = transform(string(argv[2]));
  uint maxv = transform(string(argv[3]));

  uint *data = new uint[len];
  Array a(len,maxv);
  for(uint i=0;i<len;i++) { 
    data[i] = rand()%maxv;
    a.setField(i,data[i]);
  }
  #if 0
  SequenceMatrix s(a,new BitSequenceBuilderRG(32),new MapperNone());
  
  testSequence(a,s);
  cerr << "Done" << endl;
  #endif
  #if 1
  //~ len = 13;
  //~ maxv=6;
  //~ uint *data = new uint[len];
  //~ Array a(len,maxv);
//~ 
  //~ a.setField(0,2);a.setField(1,1);a.setField(2,4);a.setField(3,0);
  //~ a.setField(4,2);a.setField(5,6);a.setField(6,5);a.setField(7,3);
  //~ a.setField(8,3);a.setField(9,0);a.setField(10,5);a.setField(11,0);
  //~ a.setField(12,4);
  //~ for (int i=0;i<len;i++)data[i]=a.getField(i);
  //BitmapsSequence  bs(a,new MapperNone(),new BitSequenceBuilderRRR(33));
  //testSequence(a, bs);

	
  Mapper * mapper = new MapperCont(a, BitSequenceBuilderRG(20));
  maxv=0;
  for (int i=0;i<len;i++){
	  data[i]=mapper->map(data[i])-1;
	  if (data[i]>maxv)maxv=data[i];
	  a.setField(i,data[i]);
  }
  

  Mapper * mapper2 = new MapperNone();
  //~ cout << "Test 2 : Wavelet Matrix" << endl;
  //~ WaveletMatrix wm(a,new BitSequenceBuilderRRR(32),new MapperNone());
  //~ testSequence(a,wm);
  //~ wt_coder_huff * wc = new wt_coder_huff(data,len,new MapperNone());
	//~ wt_coder_huff_can * wcc = new wt_coder_huff_can(wc,maxv);
  //~ WaveletTree wt(a,wcc,new BitSequenceBuilderRRR(32),new MapperNone());
  //~ testSequence(a,wt);
  wt_coder_huff * wc = new wt_coder_huff(data,len,mapper2);
  MappingBuilder *mt = new MappingTableBuilder();
  CanonicalWTNoptrs wt(data,len,wc,new BitSequenceBuilderRRR(32),new MapperNone(),mt);
  testSequence(a,wt);
  //  BitSequenceBuilder **bmpsBuilders = new BitSequenceBuilder*[1];
  //  bmpsBuilders[0] = new BitSequenceBuilderRPSC(new BitSequenceBuilderRRR(32),8,4,8u);
  //  SequenceBuilder *wtRepairBuilder = new SequenceBuilderWaveletTreeNoptrs(bmpsBuilders,1,new MapperNone());
  //  Sequence *wt = wtRepairBuilder->build(data,len);
  //  testSequence(a,*wt); 
  // //~ 
  //~ CanonicalWTNoptrs(uint * symbols, uint n, wt_coder_huff *wc, BitSequenceBuilder * bmb, Mapper * am, MappingBuilder *msb,  bool deleteSymbols=false); 
  //~ cout << "bs.size() = " << wt.getSize() << endl;
  //~ testSequence(a,wt);
  //~ mapper->unuse();
  //~ mapper2->unuse();
  #endif
  return 0;
}

