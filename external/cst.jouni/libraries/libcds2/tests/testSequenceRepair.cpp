
#include <cstdlib>

#include <libcdsBasics.h>
#include <BitSequence.h>
#include <Mapper.h>
#include <Sequence.h>

using namespace std;
using namespace cds_static;

void testSequence(Array & a, Sequence & bs) {
    //~ ofstream outfs("sequence.tmp");
    //~ bs.save(outfs);
    //~ outfs.close();
    //~ ifstream infs("sequence.tmp");
    //~ Sequence * seq = Sequence::load(infs);
    //~ infs.close();
    Sequence *seq = &bs;
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


void testSequence(uint *a, uint n, Sequence & bs) {
    //~ ofstream outfs("sequence.tmp");
    //~ bs.save(outfs);
    //~ outfs.close();
    //~ ifstream infs("sequence.tmp");
    //~ Sequence * seq = Sequence::load(infs);
    //~ infs.close();
    Sequence *seq = &bs;
    uint maxv = 0;
    for (uint i=0;i<n;i++) if (a[i]>maxv) maxv = a[i];
    size_t count[maxv+1];
    for(size_t i=0;i<=maxv;i++)
        count[i] = 0;
    for(size_t i=0;i<n;i++) {
        count[a[i]]++;
        if(a[i]!=seq->access(i)) {
            cerr << "ERROR ACCESS" << endl;
            cerr << "(pos: " << i << "), Got:" << seq->access(i) << " Expected:" << a[i] << endl;
            exit(-3);
        }
      
        for(uint j=a[i];j<=a[i];j++) {
            if(seq->rank(j,i)!=count[j]) {
                cerr << "ERROR RANK " << endl;
                cerr << " Rank(symbol="<<j<<",pos="<<i<<") result: " << bs.rank(j,i) << " count=" << count[j] << endl;
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
}


int main(int argc, char ** argv) {

  if(argc!=4 && argc!=2) {
    cout << "usage: " << argv[0] << " <seed> <length> <maxv>" << endl;
    cout << "OR"<< endl << "usage: " << argv[0] << " <collection> (where collection is a binary sequence with one int (32 bits) for each word" << endl;
    return 0;
  }


  #if 0
    uint len,maxv=0;
	  //bitmap i used to drive the development of the GCT 
	  len=66;
	  maxv=1;
      
	  uint data[66]={1,1,0,1,1,1,0,1,0,0,1,0,0,1,1,0,1,1,0,1,0,0,1,1,0,1,0,0,1,1,0,1,0,0,1,1,0,1,0,0,0,1,1,0,1,0,0,1,0,1,1,0,1,1,0,1,0,0,0,1,1,0,1,0,0,0};
  #endif 
  #if 1
  uint len,maxv=0,*data;
  
  if (argc==4){
	len = transform(string(argv[2]));
	maxv = transform(string(argv[3]));

	data = new uint[len];
	for(uint i=0;i<len;i++){
		data[i] = rand()%maxv;
	}
  }else if (argc==2){
		streampos begin,end;
		ifstream myfile (argv[1], ios::binary);
		begin = myfile.tellg();
		myfile.seekg (0, ios::end);
		end = myfile.tellg();
		myfile.seekg(0);
		len = end-begin;
		data = new uint[len/sizeof(uint)];
		myfile.read((char*)data,len);
		myfile.close();
		len /=sizeof(uint);
		len = min(len,len);
		MapperCont *mc = new MapperCont(data,len,BitSequenceBuilderRG(32));
		for (uint i=0;i<len;i++){
			data[i]=mc->map(data[i])-1;
			assert((int)data[i]>=0);
		}
		for (uint i=0;i<len;i++)
			maxv=max(maxv,data[i]);
        uint *noccs = new uint[maxv+1];
        for (uint i=0;i<=maxv;i++)
            noccs[i]=0;
        for (uint i=0;i<len;i++)
            noccs[data[i]]++;
        double h0=0.0;
        for (uint i=0;i<=maxv;i++){
            double pi = ((double)noccs[i])/len;
            h0 += pi*log(pi)/log(2);
        }
        cerr << "n: " << len << endl;
        cerr << "h0: " << -h0 << endl;
        cerr << "sigma: " << maxv+1 << endl;
		
  }
  #endif

  Repair *rp  = new Repair((int*)data,len);
  cerr << "rp size (bps): " << rp->getSize()*1.0/len << endl;

  ofstream of("tmp.bin");
  rp->save(of);
  of.close();
  ifstream fp("tmp.bin");
  Repair *rp2 = Repair::load(fp);
  fp.close();
  assert(rp->getNC()==rp2->getNC());
  assert(rp->getNTerm()==rp2->getNTerm());
  assert(rp->getNRules()==rp2->getNRules());
  assert(rp->getAlpha()==rp2->getAlpha());
  for (uint i=0;i<rp->getNC();i++)
    assert(rp->getC(i)==rp2->getC(i));
  for (uint i=0;i<rp->getNTerm();i++){
    assert(rp->getLeft(i)==rp2->getLeft(i));
    assert(rp->getRight(i)==rp2->getRight(i));
  }
// for (uint i=0;i<rp->getNC();i++){
//     cerr << rp->getC(i) << ", ";
// }
cerr << endl;
  //  BitSequenceBuilder **bmpsBuilders = new BitSequenceBuilder*[1];
  //  bmpsBuilders[0] = new BitSequenceBuilderRPSC(new BitSequenceBuilderRRR(32),8,4,8u);
  //  SequenceBuilder *wtRepairBuilder = new SequenceBuilderWaveletTreeNoptrs(bmpsBuilders,1,new MapperNone());
  //  Sequence *wtr = wtRepairBuilder->build(data,len);
   
  // testSequence(data,len,*wtr);
  // delete wtr;
  //delete [] data;
  // cerr << "Test Sequence OK!" << endl;
  return 0;
}

