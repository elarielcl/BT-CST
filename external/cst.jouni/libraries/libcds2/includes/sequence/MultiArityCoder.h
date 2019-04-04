    //
// Created by alberto on 4/9/15.
//

#ifndef _ALL_MULTIARY_CODERS_H_
#define _ALL_MULTIARY_CODERS_H_

#include <utils/libcdsBasicsGeneric.h>

using namespace cds_utils;
using namespace std;
namespace cds_static{

class MultiArityCoder{
    public:
        enum multi_arity_coder_types{mact_fixed=0};
        virtual ~MultiArityCoder(){};
        //Given a symbol and a level, returns the chunk corresponding to that level
        //the first chunk is in the rightmost position
        virtual uint get_chunk(const uint symbol, const uint level)const=0;
        //returns the max number of chunks of a symbol
        virtual uint get_max_level(const uint symbol)const=0;
        //saves the coder to disk
        virtual void save(ofstream &fp)const=0;
        //loads the coder from disk
        static MultiArityCoder *load(ifstream &fp);
    protected:
        MultiArityCoder(){};
    };
};
#include <sequence/MultiArityCoderFixed.h>

#endif //_ALL_MULTIARYCODERS_H_
