//
// Created by alberto on 4/13/15.
//

#include <sequence/MultiArityCoderFixed.h>
#include <utils/cppUtils.h>

namespace cds_static {

    //max_length: max length of a code (the same for all symbols)
    //chunk_length: chunk length (it is a multiary coder).
    MultiArityCoderFixed::MultiArityCoderFixed(const uint _chuck_length, const uint _max_v) : MultiArityCoder(), chunk_length(_chuck_length), max_v(_max_v){

        mask_chunk_length = (1U << chunk_length) - 1U;
        //we index the max_level in [0...max_level]
        max_level = bits(max_v) / chunk_length + ((bits(max_v) % chunk_length) ? 1 : 0) -1 ;

    }

    MultiArityCoderFixed::MultiArityCoderFixed():MultiArityCoder(){}

    MultiArityCoderFixed::~MultiArityCoderFixed(){};

    uint MultiArityCoderFixed::get_chunk(const uint symbol,const  uint level) const{
        return (symbol >> (level * chunk_length)) & mask_chunk_length;
    }

    uint MultiArityCoderFixed::get_max_level(const uint symbols) const{
        return max_level+1;
    }

    void MultiArityCoderFixed::save(ofstream &fp)const{
        saveValue(fp, static_cast<uint>(mact_fixed));
        saveValue(fp,chunk_length);
        saveValue(fp,mask_chunk_length);
        saveValue(fp,max_level);
        saveValue(fp,max_v);
    }

    MultiArityCoderFixed *MultiArityCoderFixed::load(ifstream &fp){
        uint wr = static_cast<uint>(loadValue<uint>(fp));
        if (wr!=static_cast<uint>(mact_fixed)){
            return nullptr;
        }
        MultiArityCoderFixed *ret = new MultiArityCoderFixed();
        ret->chunk_length = loadValue<uint>(fp);
        ret->mask_chunk_length = loadValue<uint>(fp);
        ret->max_level = loadValue<uint>(fp);
        ret->max_v = loadValue<uint>(fp);
        return ret;
    }

};