#include <direct_access/DAC.h>

namespace cds_static{

//TODO correct this for 64 bits
DAC::DAC(uint *input,uint list_length,bool del):DirectAccess(list_length){
	if (list_length == 0){
		len = list_length;
		return;
	}

	bool swap=(list_length==1);
	uint *newin;
	if (list_length==1){
		cerr << "Warning: In constructor DAC(input,len,del), len<=1. This will cause problems" << endl;
		//abort();
		 newin = new uint[W];
		auto i = 0;
		for (;i<list_length;i++)
			newin[i]=input[i];
		for (;i<W-1;i++)
			newin[i]=newin[list_length-1];
		newin[W-1]=newin[W-2];
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
	
	
	for(i=0;i<tamtablebase;i++){
		levelSizeAux[i]=0;

	}


	for (i=0;i<len;i++){
		value = list[i];
		for(j=0;j<tamtablebase;j++)
			if(value>=tablebase[j])
				levelSizeAux[j]++;
	}

	j=0;

	while((j<tamtablebase)&&(levelSizeAux[j]!=0)){
		 j++;
		}
	nLevels = j;

	levelsIndex = new uint[nLevels+1];//(uint *) malloc(sizeof(uint)*(nLevels+1));
	bits_BS_len =0;
	
	base = new uint[nLevels];//(uint *)malloc(sizeof(uint)*nLevels);
	base_bits = new ushort[nLevels];//(ushort *)malloc(sizeof(ushort)*nLevels);
	
	for(i=0;i<nLevels;i++){
			if(i>=nkvalues){
				base[i]=1<<(kvalues[nkvalues-1]);
				base_bits[i]=kvalues[nkvalues-1];
			}
		else{
			base[i]=1<<(kvalues[i]);
			base_bits[i]=kvalues[i];
		}
	}

	uint tamLevels =0;
		

	tamLevels=0;
	for(i=0;i<nLevels;i++)
		tamLevels+=base_bits[i]*levelSizeAux[i];

	iniLevel = new uint[nLevels];//(uint *)malloc(sizeof(uint)*nLevels);		
	tamCode=tamLevels;
	
	uint indexLevel=0;
	levelsIndex[0]=0;
	for(j=0;j<nLevels;j++){
		levelsIndex[j+1]=levelsIndex[j] + levelSizeAux[j];
			iniLevel[j] = indexLevel;
			cont[j]=iniLevel[j];
			indexLevel+=levelSizeAux[j]*base_bits[j];
		contB[j]=levelsIndex[j];

	}


	levels = new uint[(tamLevels/W+1)];//(uint *) malloc(sizeof(uint)*(tamLevels/W+1));

	bits_BS_len = levelsIndex[nLevels-1]+1; 

	uint * bits_BS = new uint[(bits_BS_len/W+1)];//(uint *) malloc(sizeof(uint)*(bits_BS_len/W+1));
	for(i=0; i<((bits_BS_len)/W+1);i++)
		bits_BS[i]=0;
	for(i=0;i<len;i++){
		value = list[i];
		j=nLevels-1;

		while(j>=0){
			if(value >= tablebase[j]){

				newvalue = value- tablebase[j];

				for(k=0;k<j;k++){
					

					bitwrite(levels,cont[k],base_bits[k],newvalue%base[k]);
					cont[k]+=base_bits[k];
					contB[k]++;

					newvalue = newvalue/base[k];
				}
				k=j;

					bitwrite(levels,cont[j],base_bits[j],newvalue%base[j]);
					cont[j]+=base_bits[j];
					contB[j]++;
				if(j<nLevels-1){
					bitset(bits_BS,contB[j]-1);

				}
									
				break;
							}
			j--;
		}

		
	}


	bitset(bits_BS,bits_BS_len-1);

	bS = new BitSequenceRG(bits_BS, bits_BS_len , 20); 	

	delete [] bits_BS;


	
	rankLevels = new uint[nLevels];//(uint *) malloc(sizeof(uint)*nLevels);
	for(j=0;j<nLevels;j++)
 			rankLevels[j]= bS->rank1(levelsIndex[j]-1);

		
	if (swap){
		delete [] input;
		input = newin;
	}
	delete [] cont;
	delete [] contB;
	delete [] levelSizeAux;
	delete [] kvalues;

}
DAC::~DAC(){
	if (len==0) return;
	delete [] levelsIndex;
	delete [] iniLevel;
	delete [] rankLevels;
	delete [] tablebase;
	delete [] levels;
	delete     bS;
	delete [] base;
	delete [] base_bits;
}

uint DAC::access(size_t param) const{

 uint mult=0;
 register int j;
 uint partialSum=0;
 uint ini = param;
 uint * level;
 uint readByte;
 uint cont,pos, rankini;

		partialSum=0;
		j=0;
		level=levels ;

		pos=levelsIndex[j]+ini;
			
			mult=0;

		cont = iniLevel[j]+ini*base_bits[j];


		readByte = bitread(level,cont,base_bits[j]);
		 if(nLevels == 1){
           return readByte;
		}
       
		while((!bS->access(pos))){
			rankini = bS->rank1(levelsIndex[j]+ini-1) - rankLevels[j];
			ini = ini-rankini;
			partialSum = partialSum+ (readByte<<mult);
			mult+=base_bits[j];
			j++;
			cont = iniLevel[j]+ini*base_bits[j];
			pos=levelsIndex[j]+ini;
			readByte = bitread(level,cont,base_bits[j]);
			if(j==nLevels-1){
				break;
			}
		}

		partialSum = partialSum + (readByte<<mult) + tablebase[j];

	return partialSum;

}

size_t DAC::getSize() const{
	if (len==0){
		return sizeof(this);
	}
	return sizeof(uint)*tamtablebase 
	+ sizeof(ushort)*nLevels
	+ sizeof(ushort)*nLevels
	+ sizeof(uint)*(nLevels+1)
	+ sizeof(uint)*(nLevels)
	+ sizeof(uint)*(nLevels)
	+ sizeof(uint)*(tamCode/W+1)
	+ bS->getSize();
}

DAC::DAC():DirectAccess(){}

void DAC::save(ofstream &fp) const{
	//saveValue(fp,listLength);
	uint wr = DAC_HDR;
	saveValue(fp, wr);
	saveValue(fp, len);//uint
	if (len==0){
		return;
	}
	saveValue(fp, nLevels);//uchar
	saveValue(fp, tamCode);//uint
	saveValue(fp, tamtablebase);//uint
	saveValue(fp, tablebase, tamtablebase);
	saveValue(fp, base_bits, nLevels);
	saveValue(fp, base, nLevels);
	saveValue(fp, levelsIndex, nLevels+1);
	saveValue(fp, iniLevel, nLevels);
	saveValue(fp, rankLevels, nLevels);
	saveValue(fp, levels,tamCode/W+1);
	bS->save(fp);
}

DAC *DAC::load(ifstream &fp){
	DAC *d = NULL;

	try{
		d = new DAC();
		uint rd = loadValue<uint>(fp);
		if (rd!=DAC_HDR){
			abort();
		}
		d->len = loadValue<size_t>(fp);
		if (d->len==0)
			return d;
		d->nLevels = loadValue<uchar>(fp);
		d->tamCode = loadValue<uint>(fp);
		d->tamtablebase = loadValue<uint>(fp);
		d->tablebase = loadValue<uint>(fp,d->tamtablebase);
		d->base_bits = loadValue<ushort>(fp,d->nLevels);
		d->base = loadValue<uint>(fp,d->nLevels);
		d->levelsIndex = loadValue<uint>(fp,d->nLevels+1);
		d->iniLevel = loadValue<uint>(fp,d->nLevels);
		d->rankLevels = loadValue<uint>(fp,d->nLevels);
		d->levels = loadValue<uint>(fp,d->tamCode/W+1);
		d->bS = BitSequence::load(fp);

	}catch(...){
		return NULL;
	}	
	return d; 
}

}
