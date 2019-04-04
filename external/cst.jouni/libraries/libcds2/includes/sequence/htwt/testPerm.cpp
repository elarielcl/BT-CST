
#include <iostream>
#include <fstream>
#include <cassert>
 
using namespace std;

#include <sys/times.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <PermWT.h> 
#include <BitsequenceBuilder.h>


double getTime (void){
	
   double usertime;
   struct rusage usage;

   getrusage (RUSAGE_SELF, &usage);

   usertime = (double) usage.ru_utime.tv_sec +
       (double) usage.ru_utime.tv_usec / 1000000.0;

   // systime = (double) usage.ru_stime.tv_sec +
       // (double) usage.ru_stime.tv_usec / 1000000.0;

   //return (usertime + systime);
   return (usertime );
}



int main(int argc, char ** argv) {
	
	if (argc < 2){
		cout << "Usage: " << argv[0] << " inputSequence" << endl; 
		exit(0); 
	}
	
	FILE *f = fopen(argv[1],"rb"); 
	struct stat st; 
	uint n, maxDistinctCodes; 
	fread(&n,sizeof(uint),1,f); 
	uint *seq = new uint[n];
	
	fread(&maxDistinctCodes,sizeof(uint),1,f); 
	int *weights = new int[maxDistinctCodes];
	fread(seq,sizeof(uint)*n,1,f);
	fread(weights,sizeof(int)*maxDistinctCodes,1,f);  
	//~ uint sum=0;
	//~ for (uint i=0;i<maxDistinctCodes;i++) {
	//~ cout << "w[" << i << "]: " << weights[i] << endl; 	
		//~ sum+=weights[i]; 
	//~ }
	//~ cout << "sum: " << sum << endl; 
	fclose(f); 
	
	PermWT *pPtrs, *pNoPtrs; 
	
	pNoPtrs =  new huTuckerPerm(weights,maxDistinctCodes,seq, n, 8, new static_bitsequence_builder_plain(8)); 
	pPtrs =new HuTuckerPermPtrs(weights,maxDistinctCodes,seq, n, 8, new static_bitsequence_builder_rrr02(8));
	
	//~ assert(pNoPtrs 
	uint leaf1, leaf2;
	double t0, t1;
	uint REP = 1; 
	t0 = getTime(); 
	for (uint j = 0; j < REP; j++){
		for (uint i=0;i<n;i++){
			//~ pNoPtrs->directApplication(i);
			 pPtrs->directApplication(i);
			//~ assert(pNoPtrs->directApplication(i)==pPtrs->directApplication(i)); 
		}
	}
	 t1 = getTime();
	cout << "Time: " << (t1-t0)/REP << endl; 
	//~ exit(0);
	for (uint i=0;i<n;i++){
		assert(pNoPtrs->inverseApplication(i, leaf1)==pPtrs->inverseApplication(i,leaf2)); 
	}
	for (uint i=0;i<1000;i++){
		for (uint j=0;j<100;j++){
			assert(pNoPtrs->rank(i, j, leaf1)==pPtrs->rank(i, j, leaf2)); 
			assert(leaf1==leaf2); 
			//~ cout << "leaf1: " << leaf1 << ", leaf2: " << leaf2 << endl; 
		}
	}
	cout << "Test1 OK" << endl;

	char * fname = new char[string(argv[1]).length()+10];
	sprintf(fname,"%s.test",argv[1]);
	f = fopen(fname,"wb"); 
	pPtrs->save(f); 
	fclose(f); 
	f = fopen(fname,"rb"); 
	pPtrs = NULL; 
	
	pPtrs = PermWT::load(f); 
	cout << "after loading " << endl; 
	
	for (uint i=0;i<n;i++){
		//~ cout << "noptrs: " << pNoPtrs->directApplication(i) << ", ptrs: " << pPtrs->directApplication(i) << endl; 
		assert(pNoPtrs->directApplication(i)==pPtrs->directApplication(i)); 
	}
	for (uint i=0;i<n;i++){
		assert(pNoPtrs->inverseApplication(i, leaf1)==pPtrs->inverseApplication(i,leaf2)); 
	}
	for (uint i=0;i<1000;i++){
		for (uint j=0;j<100;j++){
			assert(pNoPtrs->rank(i, j, leaf1)==pPtrs->rank(i, j, leaf2)); 
			assert(leaf1==leaf2); 
			//~ cout << "leaf1: " << leaf1 << ", leaf2: " << leaf2 << endl; 
		}
	}
	
	
	cout << "Test OK!" << endl; 
	delete [] weights; 
	delete [] seq; 
	exit(0);
}
