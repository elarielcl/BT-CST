
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

  if(argc!=1 && argc!=5 && argc!=3) {
    cout << "usage: " << argv[0] << " <seed> <length> <maxv> <limitSets>" << endl;
    cout << "OR"<< endl << "usage: " << argv[0] << " <collection> <limitSets>(where collection is a binary sequence with one int (32 bits) for each word" << endl;
    return 0;
  }
	uint len,maxv=0;
	bool deleteData;
  #if 0
	  //bitmap i used to drive the development of the GCT 
	deleteData=false;
	len = 24;
	uint data[24] = {0,1,0,1,2,4,2,4,3,5,0,1,0,1,2,4,3,5,3,5,3,5,0,1};
	maxv=5;
  #endif 
  
  #if 1
  deleteData=true;
  uint *data;
  int limitSets;
  if (argc==5){
	len = transform(string(argv[2]));
	maxv = transform(string(argv[3]));
	limitSets = atoi(argv[4]);
	data = new uint[len];
	for(uint i=0;i<len;i++){
		data[i] = rand()%maxv;
	}
  }else if (argc==3){
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
		cerr << "sigma: " << maxv << endl;
		limitSets = atoi(argv[2]);
  }
  
  #endif
    
/*  uint nruns=0;
  uint runSize=0;
  double avgRunSize=0;
  uint prev=data[0];
  for (size_t i=0;i<len;i++){
    if (data[i]<prev){
      prev=data[i];
      avgRunSize+=runSize;
      nruns++;
      runSize=0;
    }
    prev=data[i];
    runSize++;
  }
  if (runSize){
    avgRunSize+=runSize;
    nruns++;
  }
  cerr << "nruns: " << nruns << endl;
  cerr << "avgRunSize: " << avgRunSize/nruns << endl;
  exit(0);*/
  // Repair *rp = new Repair((int*)data,len);
  // cerr << "rp size (bps): " << (double)rp->getSize()/len << endl;

  BitSequenceBuilder **bmpsBuilders = new BitSequenceBuilder*[1];
  bmpsBuilders[0] = new BitSequenceBuilderRPSC(new BitSequenceBuilderRRR(32),8,4,8u);
  SequenceBuilder *wtRepairBuilder = new SequenceBuilderWaveletTreeNoptrs(bmpsBuilders,1,new MapperNone());
  
  /*Sequence *wt1 = wtRepairBuilder->build(data,len);
  cerr << "wt1->size (bpc):" << (double)wt1->getSize()*8/len << endl;
  cerr << "Testing wtnoptrs-rep" << endl;
  testSequence(data,max(len,10u),*wt1);
*/
  Mapper * mapper = new MapperNone();
  cerr << "limitSets= " << limitSets << endl;
  Sequence *wtr = new SequenceWTRP(data,len,limitSets, bmpsBuilders[0] ,mapper,false);
  //cerr << "Total size: " << wtr->getSize() << endl;
  //testSequence(data,len,*wtr);
  //cerr << "Test Sequence OK!" << endl;
  delete wtr;
  if (deleteData) delete [] data;
  
  return 0;
}

