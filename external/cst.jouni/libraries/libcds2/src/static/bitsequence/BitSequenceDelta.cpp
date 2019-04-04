/* 
 * File:   BitSequenceDelta.cpp
 * Author: andres
 * 
 * Created on September 25, 2011, 10:23 PM
 */

#include <bitsequence/BitSequenceDelta.h>
//#include <basics.h>

namespace cds_static{


BitSequenceDelta::BitSequenceDelta(){
    deltacodes=NULL;
    sampled_pointer=NULL;
    sampled_total=NULL;
    n=0;
    sampling=0;
    total_size=0;
}
BitSequenceDelta::BitSequenceDelta(unsigned int* array, unsigned int n, unsigned int sampling){
    this->n = n;
    this->sampling = sampling;
    sampled_pointer =  new unsigned int[this->n/this->sampling+2]();
    sampled_total = new unsigned int[this->n/this->sampling+2]();
    sampled_pointer[0]=0;
    sampled_total[0]=0;
    this->total_size=0;
    for(unsigned int i=0;i<this->n; i++){


        unsigned int bits_array = bits(array[i]);
        this->total_size += 1 + 2*(bits(bits_array)-1) + (bits_array-1);
    }
    deltacodes = new unsigned int[(this->total_size-1)/32+2]();
    unsigned int pos = 0;
    unsigned int total = 0;
    for(unsigned int i=0;i<this->n;i++){
        if(i%this->sampling==0){
            this->sampled_pointer[i/this->sampling] = pos;
            this->sampled_total[i/this->sampling] = total;
        }
        total += array[i];
        /*if(array[i]==0){
            std::cerr<<"DCodes array["<<i<<"]=0"<<std::endl;
        }*/
        encodeDelta(array[i],this->deltacodes,&pos);
        //std::cerr<<"array: "<<array[i] <<std::endl;
         //std::cerr<<"DCodes: "<<this->deltacodes[pos] <<std::endl;
    }
    if(this->n%this->sampling==0){
        this->sampled_pointer[this->n/this->sampling] = pos;
        this->sampled_total[this->n/this->sampling] = total;
    }
    //std::cout<<"Total: "<<total<<" "<<total_size<<std::endl;
   /* for(unsigned int i=0;i<n/sampling;i++){
        if(sampled_total[i]<i*sampling){
            std::cerr<<"Sampled total="<<sampled_total[i]<<" pos="<<i*sampling<<std::endl;
        }
    }
    std::cerr<<"Pos: "<<pos<<" total_size: "<<this->total_size<<std::endl;
    */
}
BitSequenceDelta::~BitSequenceDelta(){
    if(this->deltacodes!=NULL)delete[] deltacodes;
    if(this->sampled_pointer!=NULL)delete[] sampled_pointer;
    if(this->sampled_total!=NULL)delete[] sampled_total;
}

size_t BitSequenceDelta::select1(const size_t total) const{
    unsigned int pos = total/this->sampling;
    unsigned int check = total%this->sampling;
    unsigned int s = this->sampled_total[pos];
    unsigned int pointer = this->sampled_pointer[pos];
    for(unsigned int i=0;i<check;i++){
        s += decodeDelta(this->deltacodes,&pointer);
    }
    return s-1;
}

size_t BitSequenceDelta::select2(unsigned int total, unsigned int* delta) const{
    unsigned int pos = total/this->sampling;
    unsigned int check = total%this->sampling;
    unsigned int s = this->sampled_total[pos];
    unsigned int pointer = this->sampled_pointer[pos];
    for(unsigned int i=0;i<check;i++){
        s += decodeDelta(this->deltacodes,&pointer);
    }
    *delta = decodeDelta(this->deltacodes,&pointer);
    return s-1;
}

//returns the number of 1's found until position pos (included)
size_t BitSequenceDelta::rank1(const size_t pos) const{
    unsigned int select;
    return this->rank_select(pos,&select);
}
bool BitSequenceDelta::access(const size_t pos) const{
    unsigned int select;
    this->rank_select(pos,&select);
    return select == pos;
}
size_t BitSequenceDelta::rank_select(unsigned int pos, unsigned int* select) const{
    unsigned int upper_bound = std::min((pos+1)/this->sampling+1,this->n/this->sampling+1);
    unsigned int index = std::lower_bound(this->sampled_total,this->sampled_total+upper_bound,pos+1)-this->sampled_total;
    //std::cout<<"Index:"<<index<<std::endl;
    if(this->sampled_total[index]==pos+1){
        *select = pos;
        return index*this->sampling;
    }else{
        unsigned int pointer = this->sampled_pointer[index-1];
        unsigned int total = this->sampled_total[index-1];
        index=(index-1)*this->sampling;
        for(unsigned int i=0;i<this->sampling;i++){
            unsigned int val = decodeDelta(this->deltacodes,&pointer);
            if(val+total>pos+1){
                *select = total-1;
                return index+i;
            }else if(val+total==pos+1){
              *select=pos;
              return index+i+1;
            }
            total+=val;
        }
        return (unsigned int)-1;//should not return this.
    }
}

size_t BitSequenceDelta::select0_rank1(unsigned int total,unsigned int* rank) const{
    /*for(unsigned int i=0;i<n/sampling;i++){
        if(sampled_total[i]<i*sampling){
            std::cerr<<"S0R1 Sampled total="<<sampled_total[i]<<" pos="<<i*sampling<<std::endl;
        }
    }*/
    //std::cout<<"DCodes total: "<<total<<std::endl;
    //std::cout<<"DCodes: "<<n<<" "<<sampling<<std::endl;
    //First Binary Search over sampled_total[i] - s*i //number of zeroes
    total -= 1;
    unsigned int min_pos = 0;
    unsigned int max_pos = (this->n)/sampling;
    unsigned int m;
    while(max_pos > min_pos){
        m = (max_pos + min_pos)/2;
        if(sampled_total[m] - sampling*m > total)
            max_pos = m-1;
        else if(sampled_total[m] - sampling*m < total)
            min_pos = m+1;
        else
            break;
    }
    m = (max_pos + min_pos)/2;
    //std::cout<<"Dcodes m="<<m<<std::endl;
    //std::cout<<"SR: "<<sampled_total[m]<<" "<<sampled_total[m]-m*sampling<<" "<<m*sampling<<std::endl;
    if(sampled_total[m] - m*sampling > total)
        m -= 1;
    //std::cout<<"Dcodes m="<<m<<std::endl;
    unsigned int zeros = sampled_total[m]-sampling*m;
    unsigned int pointer = sampled_pointer[m];
    unsigned int pos = sampled_total[m];
    //std::cout<<"SR: "<<pos<<" "<<zeros<<" "<<m*sampling<<std::endl;
    *rank=m*sampling;
    while(pointer<total_size){
        unsigned int val = decodeDelta(this->deltacodes,&pointer);
        if(zeros + val - 1 <= total){
            pos += val;
            zeros += val - 1;
            *rank += 1;
        }else{
            break;
        }
    }
    if(pos<*rank){
        std::cerr<<"ERROR: select0_rank1: "<<pos<<" "<<*rank<<std::endl;
    }
    return pos-1;
}
size_t BitSequenceDelta::getSize() const{
    return sizeof(unsigned int)*2*(this->n/this->sampling+2)+sizeof(unsigned int)*((this->total_size-1)/32+2);
}

void BitSequenceDelta::encodeDelta(unsigned int val, unsigned int* deltaCodes, unsigned int* pos){
    unsigned int deltaCodesPos = *pos;
    unsigned int k = bits(val);
    unsigned int p = bits(k);
    //computes the first k-1 bits of val
    unsigned int code = val & ((1<<(k-1))-1);
    //unsigned int codeLenght = 2*p+k-2;

    //Add the initial p-1 0's
    deltaCodesPos += p-1;

    //Add p bits of k
    if( ((deltaCodesPos%32) + p) > 32 ) {
	    deltaCodes[deltaCodesPos/32] |= (k>>((deltaCodesPos%32)+p-32));
	    deltaCodes[deltaCodesPos/32+1] = (k<<(64-(deltaCodesPos%32)-p));
    }else{
	    deltaCodes[deltaCodesPos/32] |= (k<<(32-p-(deltaCodesPos%32)));
    }
    deltaCodesPos += p;

    //Add k-1 bits of val (without leading 1)
    if( ((deltaCodesPos%32) + (k-1)) > 32 ) {
	    deltaCodes[deltaCodesPos/32] |= (code>>((deltaCodesPos%32)+(k-1)-32));
	    deltaCodes[deltaCodesPos/32+1] = (code<<(64-(deltaCodesPos%32)-(k-1)));
    } else {
	    deltaCodes[deltaCodesPos/32] |= (code<<(32-(k-1)-(deltaCodesPos%32)));
    }
    deltaCodesPos += k-1;
    *pos = deltaCodesPos;
}

unsigned int BitSequenceDelta::decodeDelta(unsigned int* deltaCodes, unsigned int* pos) const{
    //std::cout<<"DecodeDelta Pos:"<<*pos<<std::endl;
    unsigned int code;
    unsigned int pointer = *pos;
    code = (deltaCodes[pointer/32] << (pointer%32)) |
	       ((pointer%32 != 0) * (deltaCodes[pointer/32+1] >> (32-(pointer%32))));

    //Compute number of 0's (p) in the left of the code
    //unsigned int p = P[((code & 0xF8000000)>>27)];//should check if p=6
    unsigned int p = 1;
    while(!(code & 0x80000000)) {
	    code <<= 1;
	    p++;
    }
    //code<<=p-1;

    //Compute length of binary representation of code (k)
    unsigned int k = code >> (32-p);

    //Update deltaCodes pointer
    pointer += 2*p-1;

    // Add binary representation of code
    code = (deltaCodes[pointer/32] << (pointer%32)) |
	       ((pointer%32 != 0) * (deltaCodes[pointer/32+1] >> (32-(pointer%32))));
    code = ((code >> 1) | 0x80000000) >> (32-k);
    pointer += k-1;
    *pos = pointer;
    return code;
}

void BitSequenceDelta::save(ofstream & fp) const{
    uint wr = DELTA;
    saveValue(fp,wr);
    saveValue(fp, n);
    saveValue(fp,  sampling);
    saveValue(fp,  total_size);
    saveValue(fp, sampled_total, n/sampling+2);
    saveValue(fp, sampled_pointer, n/sampling+2);
    saveValue(fp, deltacodes, (total_size-1)/32+2);


}
BitSequenceDelta* BitSequenceDelta::load(ifstream & fp){
    uint id = loadValue<uint>(fp);
        if(id!=DELTA) return NULL;
    BitSequenceDelta* ret = new BitSequenceDelta();
    ret->n = loadValue<unsigned int>(fp);
    ret->sampling = loadValue<unsigned int>(fp);
    ret->total_size = loadValue<unsigned int>(fp);

    ret->sampled_total = new unsigned int[ret->n/ret->sampling+2];
    if(ret->sampled_total == NULL){
        delete ret;
        return NULL;
    }
    ret->sampled_total = loadValue<unsigned int>(fp, ret->n/ret->sampling+2);

    ret->sampled_pointer = new unsigned int[ret->n/ret->sampling+2];
    if(ret->sampled_pointer == NULL){
        delete ret;
        return NULL;
    }
    ret->sampled_pointer = loadValue<unsigned int>(fp, ret->n/ret->sampling+2);

    ret->deltacodes = new unsigned int[(ret->total_size-1)/32+2];
    if(ret->deltacodes == NULL){
        delete ret;
        return NULL;
    }
    ret->deltacodes = loadValue<unsigned int>(fp, (ret->total_size-1)/32+2);

    return ret;
    /*for(unsigned int i=0;i<ret->n/ret->sampling;i++){
        if(ret->sampled_total[i]<i*ret->sampling){
            std::cerr<<"Sampled total="<<ret->sampled_total[i]<<" pos="<<i*ret->sampling<<std::endl;
        }
    }*/
}



}


