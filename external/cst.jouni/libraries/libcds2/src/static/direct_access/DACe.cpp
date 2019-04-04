#include <direct_access/DACe.h>

namespace cds_static{


DACe::DACe(uint *input,uint list_length,bool del):DirectAccess(list_length){
	bool swap=(list_length==1);
	uint *newin;
	if (list_length==1){
		cerr << "Warning: In constructor DACe(input,len,del), len<=1. This will cause problems" << endl;
		 newin = new uint[2];
		for (int i=0;i<list_length;i++)
			newin[i]=input[i];
		newin[list_length]=newin[list_length-1]+1;
		uint *tmp = input;
		input = newin;
		newin = tmp;
	}
	
	uint *levelSizeAux;
	uint *cont;	
	uint *contB;
	
	uint* kvalues;
	int nkvalues;
	
	
	register uint i;
	int j, k;
	uint value, newvalue;
	uint bits_BS_len = 0;
	
	uint *list;
	if (del) list=input;
	else{
		list = new uint[len];
		for (uint i=0;i<len;i++){
			list[i]=input[i];
			assert(list[i]>=0);
		}
	}
	
	kvalues = optimizationk(list,(int)len,&nkvalues);
	
	uint kval;
	uint oldval =0;
	uint newval =0;
	
	i=0;
	uint multval=1;
	do{
		oldval=newval;
		if(i>=nkvalues){
			kval = 1<<(kvalues[nkvalues-1]);
		}
		else
			kval=1<<(kvalues[i]);	
		multval*=kval;
		newval = oldval+multval;

		i++;
	}
	while(oldval<newval);
	
	tamtablebase = i;
	tablebase = new uint[tamtablebase];//(uint *) malloc(sizeof(uint)*tamtablebase);
	levelSizeAux = new uint[tamtablebase];//(uint *) malloc(sizeof(uint)*tamtablebase);
	cont = new uint[tamtablebase];//(uint *) malloc(sizeof(uint)*tamtablebase);
	contB = new uint[tamtablebase];//(uint *) malloc(sizeof(uint)*tamtablebase);

	oldval =0;
	newval =0;
	multval=1;	
	for(i=0;i<tamtablebase;i++){
		oldval=newval;
		if(i>=nkvalues){
			kval = 1<<(kvalues[nkvalues-1]);
		}
		else
			kval=1<<(kvalues[i]);	
		multval*=kval;
		newval = oldval+multval;
		tablebase[i]=oldval;
	}	
	
	acumPosLevel = new uint[tamtablebase];
	for(i=0;i<tamtablebase;i++){
		levelSizeAux[i]=acumPosLevel[i]=0;
	}

	uint max_value = 0;
	for (i=0;i<len;i++){
		max_value = max(max_value,list[i]);
		value = list[i];
		for(j=0;j<tamtablebase;j++)
			if(value<tablebase[j]){
				assert((j-1)>=0);
				//count the number of codes that remain at each level
				levelSizeAux[j-1]++;
				break;
			}
		for (j=0;j<tamtablebase;j++)
			if (value>=tablebase[j])
				acumPosLevel[j]++;
	}
	
	nLevels=j=0;
	while((j<tamtablebase)){
		if ((levelSizeAux[j]!=0))nLevels++;
		j++;
	}
	nLevelsMinus1 = nLevels-1;
		
	//now starts the party
	seqs = new uint*[nLevels];

	lenChunkLevel = new uint[nLevels];
	for (uint i=0;i<nLevels-1;i++){
		lenChunkLevel[i]=bits(tablebase[i+1]-1);
	}
	lenChunkLevel[nLevels-1]=bits(max_value);
	for (uint i=1;i<nLevels;i++){
		assert(lenChunkLevel[i-1] && lenChunkLevel[i]);
		assert(lenChunkLevel[i]>lenChunkLevel[i-1]);
	}
	for (uint i=0;i<nLevels;i++){
		uint nints = uint_len(levelSizeAux[i],lenChunkLevel[i]);
		seqs[i] = new uint[nints];
		for (uint j=0;j<nints;j++)
			seqs[i][j]=0;
	}
	
	uint bmpLen=0;
	for (uint i=0;i<nLevels-1;i++)
		bmpLen += acumPosLevel[i];
	uint *bmp = new uint[uint_len(bmpLen,1)];
	for (uint i=0;i<uint_len(bmpLen,1);i++)bmp[i]=0;
	uint pointer,pointerBmp,l,newListLen;
	newListLen=len;
	l=pointerBmp=0;
	while(newListLen>0){
		pointer=0;
		for (uint i=0;i<newListLen;i++){
			if (list[i]>=tablebase[l+1]){
				list[pointer++]=list[i];
				//we don't need the bitmap in the last level
				assert(l<nLevels-1);
				bitset(bmp,pointerBmp+i);
			}else{
				bitwrite(seqs[l],lenChunkLevel[l]*(i-pointer),lenChunkLevel[l],list[i]);
			}
		}
		pointerBmp+=newListLen;
		newListLen=pointer;
		l++;
	}
	assert(l==nLevels);
	bitmap = new BitSequenceRG(bmp,pointerBmp,20);
	
	for (uint i=1;i<nLevels;i++){
		acumPosLevel[i]=acumPosLevel[i-1]+acumPosLevel[i];
		acumPosLevel[i]--;
	}
	acumPosLevel[0]--;
		
	rank0Level = new uint[nLevels];
	rank1Level = new uint[nLevels];
	acumPosMinusRank1 = new uint[nLevels];
	rank0Level[0]=rank1Level[0]=0;
	acumPosMinusRank1[0]=acumPosLevel[0];
	for (uint i=1;i<nLevels;i++){
		rank1Level[i]=bitmap->rank1(acumPosLevel[i-1]);
		rank0Level[i]=bitmap->rank0(acumPosLevel[i-1]);
		acumPosMinusRank1[i]=acumPosLevel[i]-rank1Level[i];
	}

	
	if (swap){
		delete [] input;
		input = newin;
	}
	delete [] cont;
	delete [] contB;
	delete [] levelSizeAux;
	delete [] kvalues;

}
DACe::~DACe(){
}

uint DACe::access(uint pos) const{
	size_t r=0;uint l;
	l=0;
	while(l<nLevelsMinus1 && bitmap->access(pos)){
		pos = acumPosMinusRank1[l]+bitmap->rank1(pos);
		l++;
	}
	if (l<nLevelsMinus1){
		return bitread(seqs[l],lenChunkLevel[l]*(bitmap->rank0(pos)-rank0Level[l]-1),lenChunkLevel[l]);
	}
	return get_field(seqs[l],lenChunkLevel[l]*(pos-acumPosLevel[l-1]-1),lenChunkLevel[l]);
}

size_t DACe::getSize() const{
	return sizeof(uint)*tamtablebase 
	+ sizeof(ushort)*nLevels
	+ sizeof(ushort)*nLevels
	+ sizeof(uint)*(nLevels+1)
	+ sizeof(uint)*(nLevels)
	+ sizeof(uint)*(nLevels)
	+ sizeof(uint)*(tamCode/W+1)
	+ bS->getSize();
}


void DACe::save(ofstream &of) const{
	cerr << "NOT IMPLEMENTED YET (DACe->save())" << endl;
}

DACe* DACe::load(ifstream &fp){
	cerr << "NOT IMPLEMENTED YET (DACe->load())" << endl;
	return NULL;
}
}
