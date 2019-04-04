/* 
Programmed by Alberto Ordóñez 
Copyright (C) 2012  Antonio Fariña, Alberto Ordóñez and José R. Paramá
Author's contact: alberto.ordonez@udc.es

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <iostream>
#include <fstream>
#include <cassert>
 
using namespace std;
//~ using namespace cds_static; 

//~ #include <WaveletTreeNoptrs.h>
//~ #include <Sequence.h>
//~ #include <Mapper.h>
//~ #include <BitSequenceBuilder.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <BitSequence.h>
#include <BitSequenceBuilder.h>
#include <Sequence.h>
#include <SequenceBuilder.h>
#include <wt_coder.h>


using namespace cds_static;

typedef uint word; 


double getTime (void){
	
   double usertime;
   struct rusage usage;

   getrusage (RUSAGE_SELF, &usage);

   usertime = (double) usage.ru_utime.tv_sec +
       (double) usage.ru_utime.tv_usec / 1000000.0;

   return (usertime );
}

uint loadBinaryFile(char *inputFileName, word **numbers){
	
	FILE *fin;
	struct stat sdata;
	int i;
	fin = fopen(inputFileName,"rb");

	stat(inputFileName,&sdata);
	//~ fprintf(stderr,"Size of input data file=%d\n",sdata.st_size);
	*numbers = (word *) malloc(sdata.st_size);
	if ((i=fread(*numbers,sdata.st_size,1,fin))<0){
		printf("Problem while loading input file\n");//,strerror(errno));
		exit(0);
	}	

	fclose(fin);
	return sdata.st_size/sizeof(word);
}

BitSequenceBuilder * parse_bitsequence_builder(string s, uint sample_rate){
	if (s==string("rrr02")){
		cout << "bitmap rrr02, sample_rate: " << sample_rate << endl; 
		return new BitSequenceBuilderRRR(sample_rate); 
	}else if (s==string("plain")){
		return new BitSequenceBuilderRG(sample_rate); 
	}else if (s==string("plain35")){
		return new BitSequenceBuilderPlain(4); 
	}else{
		return NULL;
	}
}
void testSequence2(uint *a, uint n, Sequence & bs) ;

int main(int argc, char ** argv) {
	
	if (argc < 5){
		cerr << "Usage: " << argv[0] << " inputSequence queries_access queries_rank queries_select" << endl;
		exit(0); 
	}
	
	uint *seq; uint n; 
	n = loadBinaryFile(argv[1],&seq); 
	
	

		uint maxv = 0;
		for (uint i=0;i<n;i++){
			if (seq[i]>maxv) maxv=seq[i]; 
		}
		uint *noccs; 
		noccs =   new uint[maxv+1];
		for (uint i=0;i<=maxv;i++){
			noccs[i]=0;
		}
		for (uint i=0;i<n;i++){
			noccs[seq[i]]++;
		}
		
		double entrop=0.0;
		for (uint i=0;i<=maxv;i++){
			entrop = entrop + ((double)noccs[i]/n)*log(((double)noccs[i]/n))/log(2);
		}
		cerr << "H0="<<-entrop << endl;
		
		uint maxFreq = 0;
		for (uint i=0;i<=maxv;i++) if (noccs[i]>maxFreq) maxFreq=noccs[i];
		delete [] noccs;
		
	cout << "MaxFreq: " << maxFreq << endl; 
	MapperNone *mp = new MapperNone();
	MapperNone *mp2 = new MapperNone();
	
		wt_coder_huff *wc= new wt_coder_huff(seq,n,new MapperNone());
		//~ wt_coder_huff_can *wcc = new wt_coder_huff_can(wc,maxv);
		//~ cerr << "avgcodeLength optimal: " << wcc->avgCodeLength() << endl;
		wt_coder_SubOptimalAdd *wccs = new wt_coder_SubOptimalAdd(wc,maxv,(float)0.8);
		cerr << "avgcodeLength suboptimal: " << wccs->avgCodeLength() << endl;
	
	BitSequenceBuilder *bsb;
	bsb = new BitSequenceBuilderRG(32);
		
		cout << "WT Base Type: WT-NPTRS" << endl;
		Sequence *wtBase = new WaveletTreeNoptrs(seq,n,bsb,mp,false);
	
		cout << "WT Canonical Type: Canonical WT" << endl;
		MappingBuilder *mt = new MappingTableBuilder();
		
		Sequence *wtHuff = new CanonicalWTNoptrs(seq,n,wccs,bsb,mp,mt);
	
		cout << "Test Huff WT size: " << wtHuff->getSize() << endl;
		cout << "bps (Huff): " << (double)wtHuff->getSize()/wtHuff->getLength()<<endl;
		
	
		double t0,t1;
		uint nqa, nqr, nqs;
		uint *qa,*qr,*qs;
		nqa = loadBinaryFile(argv[2],&qa); 
		nqr = loadBinaryFile(argv[3],&qr);
		nqs = loadBinaryFile(argv[4],&qs);
		//~ cerr << "nqa: " << nqa << "nqr: " << nqr << ", nqs: " << nqs << endl;
		uint REP=10;

		t0 = getTime();
		for (uint j=0;j<REP;j++){
			for (uint i=0;i<nqa;i+=2){
				if (wtBase->access(qa[i])!=wtHuff->access(qa[i])){
					cout << "Error en access" << endl;
				}
			}
		}
		t1 = getTime();
		
		cout << "Time access: " << (t1-t0)/(double)REP<< endl;
		
		t0 = getTime();
		for (uint j=0;j<REP;j++){
			for (uint i=0;i<nqr;i+=2){
				if (wtBase->rank(qr[i],qr[i+1]) != wtHuff->rank(qr[i],qr[i+1])){
					cout << "i: " << i << ", Error en rank. Base=" << wtBase->rank(qr[i],qr[i+1]) <<", huff: " <<   wtHuff->rank(qr[i],qr[i+1]) << endl;
				}
			}
		}
		t1 = getTime();

		//~ cout << "Time rank: " << (t1-t0)/(double)REP*1000000/(nqr/2) << endl;
		cout << "Time rank: " << (t1-t0)/(double)REP<< endl;
		
		
		t0 = getTime();
		for (uint j=0;j<REP;j++){
			for (uint i=0;i<nqs;i+=2){
				if (wtBase->select(qs[i],qs[i+1])!=wtHuff->select(qs[i],qs[i+1])){
					cout << "i: " << i << ", Error en select. Base" << wtBase->select(qs[i],qs[i+1]) << ", q: " << qs[i] <<", pos: " << qs[i+1] << endl;
					cout << "i: " << i << ", Error en select. Huff" << wtHuff->select(qs[i],qs[i+1]) << ", q: " << qs[i] <<", pos: " << qs[i+1] << endl;
				}
			}
		}
		t1 = getTime();
		//~ cout << "Time select: " << (t1-t0)/(double)REP*1000000/(nqs/2)<<endl;//*1000000/nqs << endl;
		cout << "Time select: " << (t1-t0)/(double)REP<<endl;//*1000000/nqs << endl;
		
	
	
	delete [] seq;
	delete wtBase;
	delete wtHuff;
	delete qa;
	delete qs;
	delete qr;
	
	
}
