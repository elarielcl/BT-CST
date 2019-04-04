#include <cstdlib>
#include <iostream>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/resource.h>
using namespace std;

#ifndef EINSTEIN 
#define EINSTEIN "einstein.de.txt"
#endif

#ifndef INFLUENZA
#define INFLUENZA "influenza"
#endif

#ifndef ESCHERICHIA
#define ESCHERICHIA "Escherichia_Coli"
#endif

#ifndef PARA
#define PARA "para"
#endif

#ifndef DNA1
#define DNA1 "dna.1"
#endif
#ifndef DNA01
#define DNA01 "dna.01"
#endif
#ifndef DNA001
#define DNA001 "dna.001"
#endif
#ifndef DNA0001
#define DNA0001 "dna.0001"
#endif

void deleteDatabase(uint COL_ID,uint operation,uint ITERATION,uint technique, uint delta, uint bs){
            cout << "DELETE FROM \"result\" WHERE \"collection\"=";
            cout << "\'" << COL_ID << "\' " ;
            cout << "AND \"operation\"=" ; 
            cout << "\'" << operation << "\'";
            cout << "AND \"iteration\"=" ; 
            cout << "\'" << ITERATION << "\' ";
            cout << "AND \"delta\"=" ; 
            cout << "\'" << delta << "\' ";
            cout << "AND \"bs\"=" ; 
            cout << "\'" << bs << "\' ";
            cout << "AND \"technique\"=" ; 
            cout << "\'" << technique << "\';";
}

void insertDatabase(uint COL_ID,uint delta, uint bs, uint operation, double totalSize_bpe, double time_op, uint ITERATION,uint technique){

	cout << "INSERT INTO  \"result\" (\"collection\" ,\"delta\" ,\"bs\" ,\"operation\",\"bpc\",\"time\",\"iteration\",\"technique\")  values (";
	cout << "\'" << COL_ID << "\'," ;
	cout << "\'" << delta << "\'," ;
	cout << "\'" << bs << "\'," ;
	cout << "\'" << operation << "\',";
	cout << "\'" << totalSize_bpe << "\',";
	cout << "\'" << time_op << "\',";
	cout << "\'" << ITERATION << "\',";
	cout << "\'" << technique << "\');" << endl;
}

 enum OP_TYPE{COUNT=1,ANCESTOR=2,PARENT=3,FCHILD=4,NSIBLING=5,LCA=6,TDEPTH=7,LAQT=8,SDEPTH=9,LOCATE=10,SLINK=11,CHILD=12,LETTER=13,LAQs=14,LETTER1=15,LETTER2=16,LETTER3=17,LETTER4=18};

double getTime (void){
	
   double usertime;
   struct rusage usage;

   getrusage (RUSAGE_SELF, &usage);

   usertime = (double) usage.ru_utime.tv_sec +
       (double) usage.ru_utime.tv_usec / 1000000.0;

   return (usertime );
}

/**
 * Returns a random value in [lowest,highest]
 * */
int my_random(unsigned long long lowest, unsigned long long highest ){

	double r = (double)rand();
	r = r/RAND_MAX;
	r = r*(highest-lowest)+lowest;
	return (int) r;
}
