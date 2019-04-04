
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


void testSequence(uint *a, uint n, Sequence & bs) {
    double t0,t1;
    t0 = getTime();
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
    t1 = getTime();
    cerr << "Time: " << (t1-t0)/n*1000000 << endl;
    cerr << "Test sequence OK" << endl;
}


void testSequenceTimes(uint *a, uint n, uint maxv, uint nqueries, Sequence & bs) {
	
	srand(0);
    double t0,t1;
    uint *queries = new uint[nqueries];
    uint *queriesR = new uint[nqueries];
    uint *queriesOc = new uint[nqueries];
    uint *occ = new uint[maxv+1];
    for (uint i=0;i<=maxv;i++)occ[i]=0;
    for (uint i=0;i<n;i++)
		occ[a[i]]++;
    for (uint i=0;i<nqueries;){
		queries[i]   = rand()%n; 
		if (occ[a[queries[i]]]==0)continue;
		queriesOc[i] = max((uint)(rand()%occ[a[queries[i]]])+1,occ[a[queries[i]]]);
		i++;
	}
	delete [] occ;
	
	Sequence *seq = &bs;	
    t0 = getTime();
    for (uint i=0;i<nqueries;i++){
		seq->access(queries[i]);
	}
	t1 = getTime();
	cerr << "Time access (micro/query):" << ((t1-t0)*1000000)/nqueries << endl;
	t0 = getTime();
    for (uint i=0;i<nqueries;i++){
		seq->rank(a[queries[i]],queries[i]);
	}
	t1 = getTime();
	cerr << "Time rank (micro/query):" << ((t1-t0)*1000000)/nqueries << endl;
	
	t0 = getTime();
    for (uint i=0;i<nqueries;i++){
		seq->select(a[queries[i]],queriesOc[i]);
	}
	t1 = getTime();
	cerr << "Time select (micro/query):" << ((t1-t0)*1000000)/nqueries << endl;
	delete [] queries;
	delete [] queriesOc;	
}

int main(int argc, char ** argv) {

  if(argc!=7 && argc!=5) {
    cout << "usage: " << argv[0] << " <seed> <length> <maxv> <structure: 0=AlphPartRepSC;1=AlphPartRepSN;2=Classical> <sampleC> <sampleR>" << endl;
    cout << "OR"<< endl << "usage: " << argv[0] << " <collection> <structure>(where collection is a binary sequence with one int (32 bits) for each word" << endl;
    return 0;
  }
	uint len,maxv=0,*data;
  #if 0
	  //bitmap i used to drive the development of the GCT 
	  len=66;
	  maxv=1;
	  data[66]={1,1,0,1,1,1,0,1,0,0,1,0,0,1,1,0,1,1,0,1,0,0,1,1,0,1,0,0,1,1,0,1,0,0,1,1,0,1,0,0,0,1,1,0,1,0,0,1,0,1,1,0,1,1,0,1,0,0,0,1,1,0,1,0,0,0};
  #endif 
  
  uint structure=0;
  uint sampleC=10;uint sampleR=2;
  if (argc==7){
	len = transform(string(argv[2]));
	maxv = transform(string(argv[3]));

	data = new uint[len];
	for(uint i=0;i<len;i++){
		data[i] = rand()%maxv;
	}
	structure = atoi(argv[4]);
	sampleC = atoi(argv [5]);
	sampleR = atoi(argv[6]);
  }else if (argc==5){
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
		cerr << "input size: " << len*sizeof(uint) << endl;
		
		uint *occ = new uint[maxv+1];
		for (uint i=0;i<=maxv;i++)occ[i]=0;
		for (uint i=0;i<len;i++)
			occ[data[i]]++;
		double h0=0;
		for (uint i=0;i<=maxv;i++){
			double pi = ((double)occ[i])/len;
			assert(pi>0);
			h0 = h0 + pi*(log(pi)/log(2));
		}
		cerr << "h0: " << -h0 << endl;
		delete [] occ;	
		structure = atoi(argv[2]);
		sampleC = atoi(argv [3]);
		sampleR = atoi(argv[4]);
  }
  
  cerr << "n: " << len << endl << "maxv: " << maxv << endl;
  cerr << "log(sigma): " << log(maxv)/log(2) << endl;
  cerr << "sampleC: " << sampleC << ", sampleR: " << sampleR << endl;
  // Repair *rp = new Repair((int*)data,len);
  // cerr << "rp bps: " << (double)rp->getSize()/len << endl;
  // delete rp;
  Sequence *seq;
  if (structure==0){
	  cerr << "SC" << endl;
	  SequenceBuilder *seqClasses = new SequenceBuilderSequenceRepairSC(new BitSequenceBuilderRRR(32),
				new MapperNone(),new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)),sampleC,sampleR,8u);
	  BitSequenceBuilder **bmpsBuilders = new BitSequenceBuilder*[1];
	  //~ bmpsBuilders[0] = new BitSequenceBuilderRG(32);//new BitSequenceBuilderRPSC(new BitSequenceBuilderRRR(32),8,4,8u);
	  bmpsBuilders[0] = new BitSequenceBuilderRPSC(new BitSequenceBuilderRRR(32),8,4,8u);
	  
	  //SequenceBuilder *wtRepairBuilder = new SequenceBuilderWaveletTreeNoptrs(bmpsBuilders,1,new MapperNone());
	  SequenceBuilder *wtRepairBuilder = seqClasses;
	  seqClasses = new SequenceBuilderWaveletTreeNoptrs(bmpsBuilders,1,new MapperNone());
	  SequenceBuilder *seqFirstClasses = new SequenceBuilderSequenceRepairSC(new BitSequenceBuilderRRR(32),
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
	  seq = new SequenceAlphPartRepReg(data,len,5,6,7,seqClasses,seqOffsets,seqFirstClasses,new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)));
	}else if (structure==1){
	  cerr << "SN" << endl;
	  SequenceBuilder *seqClasses = new SequenceBuilderSequenceRepairSN(new BitSequenceBuilderRRR(32),
				new MapperNone(),new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)),sampleC,sampleR,6u);
	  SequenceBuilder *seqFirstClasses = new SequenceBuilderSequenceRepairSN(new BitSequenceBuilderRRR(32),
				new MapperNone(),new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)),sampleC,sampleR,6u);
	  SequenceBuilder *seqOffsets = new SequenceBuilderWaveletMatrix(new BitSequenceBuilderRG(128), new MapperNone());
	  
		seq = new SequenceAlphPartRep(data,len,6,5,seqClasses,seqOffsets,seqFirstClasses,new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)));
		
	}else{
		cerr << "Canonical WT" << endl;
		seq = new CanonicalWTNoptrs(data,len,new BitSequenceBuilderRRR(128),new MapperNone(),new MappingTableBuilder());
	}
  
  cerr << "seq size: " << seq->getSize() << endl;
  cerr << "seq size(bps): " << (float)seq->getSize()*8/len << endl;
  testSequenceTimes(data,len,maxv,10000,*seq);
  //testSequence(data,len,*seq);
  //~ delete seq;
  delete [] data;
  //~ cerr << "Test Sequence OK!" << endl;
  return 0;

}

