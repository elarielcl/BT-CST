/*  
 * Copyright (C) 2013-current-year, Alberto Ordóñez, all rights reserved.
 *
 * Alberto Ordóñez <alberto.ordonez@udc.es>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <direct_access/DirectAccess.h>

namespace cds_static{
	

DirectAccess::DirectAccess(){}

DirectAccess * DirectAccess::load(ifstream &fp){
	uint r = loadValue<uint>(fp);
	size_t pos = fp.tellg();
		fp.seekg(pos-sizeof(uint));
		switch(r) {
			case DAC_HDR: return DAC::load(fp);
			case DAC_INC_HDR: return DACInc::load(fp);
			case ARRAY_DA_HDR: return new ArrayDA(fp);
			// case DAC_E_HDR: return DAC::load(fp);
		}
		return NULL;
}

uint DirectAccess::getLength() const{
	return len;
}

uint * DirectAccess::optimizationk(uint * list,int listLength, int * nkvalues){
	
	int t,i,m,k;
		
	uint maxInt=0;
	
	for(i=0;i<listLength;i++)
		if(maxInt<list[i])
			maxInt = list[i];

	uint nBits = bits(maxInt)-1;
	
	uint tamAux = nBits+2;
	uint * weight=NULL;
	try{
		weight = new uint[maxInt+1];//(uint *) malloc(sizeof(uint)*(maxInt+1));
	}catch (std::bad_alloc& ba){
    	std::cerr << "bad_alloc caught: " << ba.what() << "trying to allocate " << maxInt+1 << " ints" << endl;
   	}

	for(uint i=0;i<=maxInt;i++)
		weight[i]=0;

	for(i=0;i<listLength;i++)
		weight[list[i]]++;
	
	
	uint * acumFreq = new uint[tamAux];//(uint *) malloc(sizeof(uint)*tamAux);
	
	uint acumValue = 0;
	
	acumFreq[0]=0;	
	acumValue = 0;
	uint cntb = 1;
	for(uint i=0;i<=maxInt;i++){
		if(i==(1<<cntb)){
			acumFreq[cntb]=acumValue;
			cntb++;
		}
			
		acumValue += weight[i];
		
	}
	delete [] weight;
		
	acumFreq[cntb]=listLength;
	
	long * s = new long[max(1u,nBits+1)];//(long *) malloc(sizeof(long)*(nBits+1));

	uint * l = new uint[max(1u,nBits+1)];//(uint *) malloc(sizeof(uint)*(nBits+1));
	uint * b = new uint[max(1u,nBits+1)];//(uint *) malloc(sizeof(uint*)*(nBits+1));


	ulong currentSize;
	
	m=nBits;

	s[m]=0;
	l[m]=0;
	b[m]=0;
		
	uint * fc = acumFreq;
	
	ulong minSize;
	uint minPos;
	for(t=m;t>=0;t--){
		minSize=-1;
		minPos=m;
		for(i=m;i>=t+1;i--){
			currentSize = s[i]+(fc[m+1]-fc[t])*(i-t+1)+(fc[m+1]-fc[t])/FACT_RANK;
			if(minSize>currentSize){
				minSize = currentSize;
				minPos=i;
			}
		}

		if(minSize < ((fc[m+1]-fc[t])*(m-t+1))){
			s[t]=minSize;
			l[t]=l[minPos]+1;
			b[t]=minPos-t;
			
		}
	
		else{
			s[t]=(fc[m+1]-fc[t])*(m-t+1);
			l[t]=1;
			b[t]=m-t+1;				
			
		}
		
	}
	
	
	int L = l[0];
	if (L<0) L=1;
	
	uint *kvalues = new uint[L];//(ushort*)malloc(sizeof(ushort)*L);
	
	t=0;
	for(k=0;k< L;k++){
		kvalues[k]=b[t];
		t = t+b[t];
	}
	
	//TODO uncoment this lines
	delete [] acumFreq;
	delete [] l;
	delete [] b;
	delete [] s;
	
	(*nkvalues)=L;
	return kvalues;

	
}
	
}
