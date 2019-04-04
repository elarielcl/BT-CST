//
// Created by alberto on 4/13/15.
//

#ifndef ALL_MULTIARYCODERFIXED_H
#define ALL_MULTIARYCODERFIXED_H
#include <sequence/MultiArityCoder.h>

namespace cds_static {
//Implements a coder where all the codes have the same length. The codes
//are read in chunks of chunk_length blocks.
    class MultiArityCoderFixed : public MultiArityCoder {
    public:
        //max_length: max length of a code (the same for all symbols)
        //chunk_length: chunk length (it is a multiary coder).
        MultiArityCoderFixed(const uint _chuck_length, const uint max_v);
        virtual ~MultiArityCoderFixed();
        virtual uint get_chunk(const uint symbol,const  uint level)const;
        virtual uint get_max_level(const uint symbols)const;
        virtual void save(ofstream &fp)const;
        static MultiArityCoderFixed *load(ifstream &fp);

    protected:
        MultiArityCoderFixed();
        uint chunk_length;
        uint mask_chunk_length;
        uint max_v;//sigma
        uint max_level;

    };
};
#endif //ALL_MULTIARYCODERFIXED_H
