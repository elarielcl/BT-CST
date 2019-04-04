
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
#include <GCT.h>

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

	if (argc!=3){
		cerr << "***Usage: " << argv[0] << "input input.repair" << endl;
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
	for (size_t i=0;i<sdata.st_size;i++){
		if (input[i]=='1'){
			bitset(inputBmp,i);
		}else if (input[i]!='0'){
			break;
		}
	}
	delete [] input;
	GCT *gct = new GCT(1,10,inputBmp,argv[2]); 
	cerr << "gct (bps): " << gct->getSize()*8.0/sdata.st_size << endl;

	return 0;
}

