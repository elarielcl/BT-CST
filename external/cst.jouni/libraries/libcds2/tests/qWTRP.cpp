/* 
Programmed by Alberto Ordóñez 
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
#include <Sequence.h>
#include <cstring>
#include "timeMeasure.h"

using namespace std;
using namespace cds_static;
using namespace cds_utils;

ulong loadBinary(uint *&dest, char *file){
	ifstream in(file);
	assert(in.good());
	in.seekg(0,ios_base::end);
	size_t len = in.tellg();
	in.seekg(0,ios_base::beg);
	assert(len%sizeof(uint)==0);
	dest = new uint[len];
	in.read((char*)dest,len);
	in.close();
	return len/sizeof(uint);
}


int main(int argc, char ** argv) {
	
	if (argc < 7){
		cout << "Usage: " << argv[0] << " <inputSequence> <queries> <nqueries: (if 0, all q are run)> <sampleC> <sampleR (delta)> <ss_rate>" << endl; 
		exit(0); 
	}
	uint *input=nullptr;
	ulong len = loadBinary(input, argv[1]);
	
	char *queriesName = argv[2];
	ulong nqueries = atoi(argv[3]);
	uint sampleC = atoi(argv[4]);
	uint delta = atoi(argv[5]);
	uint ss_rate = atoi(argv[6]);
	char fqAccess[4096];
	char fqRank[4096];
	char fqSelect[4096];
	strcpy(fqAccess,queriesName);
	strcat(fqAccess,".access");
	strcpy(fqRank,queriesName);
	strcat(fqRank,".rank");
	strcpy(fqSelect,queriesName);
	strcat(fqSelect,".select");
	ulong nqAccess;
	ulong nqRank;
	ulong nqSelect;
	uint *qAccess;
	uint *qSelect;
	uint *qRank;
	nqAccess = loadBinary(qAccess,fqAccess)/2;
	nqRank = loadBinary(qRank,fqRank)/2;
	nqSelect = loadBinary(qSelect,fqSelect)/2;

	if (nqueries==0){
		nqAccess = min(nqAccess,nqueries);
		nqRank = min(nqRank,nqueries);
		nqSelect = min(nqSelect,nqueries);
	}

	BitSequenceBuilder *bs = new BitSequenceBuilderRPSC(new BitSequenceBuilderRRR(5),sampleC,delta,ss_rate);
	Sequence *seq = new WaveletTreeNoptrs(input, len, bs, new MapperNone(),true);
	cerr << "sampleC: "<< sampleC << ";delta: "<<delta<<";ss_rate:"<<ss_rate<<endl;
	cerr << "bps:"<<seq->getSize()*8.0/len << endl;

	uint *answer = new uint[max(nqAccess,max(nqRank,nqSelect))];
	/**		TIME MEASURE		*/
	double t0,t1;
	t0 = getTime();
		for (uint i=0;i<nqAccess;i++){
			answer[i]=seq->access(qAccess[i<<1]);
		}
	t1 = getTime();
	cout << "Access time(mus/q):" << (t1-t0)*1000000.0/nqAccess << endl;

	delete [] qAccess;
	delete [] qRank;
	delete [] qSelect;
	delete [] input;
	exit(0);
}
