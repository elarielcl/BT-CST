
#include <trees/Parentheses.h>
#include <utils/cppUtils.h>
#include <cstdio>
#include <stdexcept>
#include <cstring>
//#include <utils/cppUtils.h>
using namespace cds_utils;


namespace cds_static{

Parentheses::Parentheses(ulong _n){
	this->n=_n;
}
Parentheses::Parentheses(){
	this->n=0;
}

bool Parentheses::isATree(uint *par, ulong len){
	int excess=0;
	for (int i=0;i<len;i++){
		if (bitget(par,i))
			excess++;
		else
			excess--;
		if (excess==0){
			return (len-1==i);
		}
	}
	return false;
}

Parentheses *Parentheses::load(ifstream &fp){
	uint r = loadValue<uint>(fp);
	size_t pos = fp.tellg();
	fp.seekg(pos-sizeof(uint));
	switch(r){
		case BP_HDR: return BP::load(fp);
		case BPNAIVE_HDR: return BPNaive::load(fp);
		default: return nullptr;
	}
	return NULL;
}


Parentheses *Parentheses::load(FILE &fp){
	uint fd;
	if ((fd=fileno(&fp))==-1){
		throw runtime_error(strerror(errno));
	}
	ifstream *in = ifstreamFromFd(fd);
	return load(*in);
}

void Parentheses::save(FILE &fp)const{
	uint fd;
	if ((fd=fileno(&fp))==-1){
		throw runtime_error(strerror(errno));
	}
	ofstream *of = ofstreamFromFd(fd);
	save(*of);
}


// bool Parentheses::isATree(uint *bmp, size_t len){
// 	int excess = 0;
// 	bool ret=false;
// 	size_t i;
// 	for (i=0;i<len;i++){
// 		excess+=(bitget(bmp,i))?1:-1;
// 		if (excess<=0){
// 			ret = (i==len-1);
// 			break;
// 		}
// 	}
// 	ofstream of("xmark1.par");
// 	saveValue(of, (uint)len);
// 	saveValue(of,bmp,uint_len(len,1));
// 	of.close();
// 	return ret;
// }

}
