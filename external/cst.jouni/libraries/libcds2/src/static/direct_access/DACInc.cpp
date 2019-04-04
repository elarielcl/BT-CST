#include <direct_access/DACInc.h>
#include <utils/StringUtils.h>

namespace cds_static{


DACInc::DACInc(uint *_input,uint list_length, uint _ss, bool del):DirectAccess(list_length){
	if (list_length==0){
		len = 0;
		return;
	}

	uint *input;
	if (!del){
		input = new uint[list_length];
		for (size_t i=0;i<list_length;i++)
			input[i]=_input[i];
	}else{
		input = _input;
	}
	this->ss = _ss;
	// uint max_v= getMaxV(input, list_length);
	uint nss = (list_length) / ss +((list_length%ss)?1:0);

	uint *ssamples = new uint[nss];
	uint pointerSSamples=0;
	for (uint i=0;i<list_length;i++){
		if (i%ss==0){
			//if the sample is at a multiple of ss, then we store the absolute value in 
			//ssamples and we set the samples at that position to 0 (as the difference)
			//w.r.t. ssamples	
			ssamples[i/ss]=input[i];
			input[i]=0;
			pointerSSamples++;
		}else{
			// assert(i>0);
			input[i]=input[i]-ssamples[i/ss];
			
		}
	}
	assert(pointerSSamples==nss);
	base = new DAC(input,list_length,true);
	superSampling = new Array(ssamples,nss);
	delete [] ssamples;
	
	if (!del){
		delete [] input;
	}
}

DACInc::~DACInc(){
	if (len==0) return;
	delete base;
	delete superSampling;	
}

uint DACInc::access(size_t pos) const{
	uint ssample = pos/ss;
	return (*superSampling)[ssample]+base->access(pos);
}

size_t DACInc::getSize() const{
	if (len==0) return sizeof(this);
	return sizeof(this)+base->getSize()+superSampling->getSize();
}

DACInc::DACInc():DirectAccess(){}

void DACInc::save(ofstream &fp) const{
	//saveValue(fp,listLength);
	uint wr = DAC_INC_HDR;
	saveValue(fp, wr);
	saveValue(fp, ss);
	saveValue(fp, len);
	if (len>0){
		base->save(fp);
		superSampling->save(fp);
	}
}

DACInc *DACInc::load(ifstream &fp){
	DACInc *d = NULL;

	try{
		d = new DACInc();
		uint rd = loadValue<uint>(fp);
		if (rd!=DAC_INC_HDR){
			abort();
		}
		d->ss = loadValue<uint>(fp);
		d->len = loadValue<size_t>(fp);
		if (d->len==0){
			d->base = nullptr;
			d->superSampling = nullptr;
		}else{
			d->base = DAC::load(fp);
			d->superSampling = new Array(fp);
		}

	}catch(...){
		return NULL;
	}	
	return d; 
}

}
