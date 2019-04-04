//
// Created by alberto on 4/9/15.
//

#ifndef _ALL_MULTIARYWT_H
#define _ALL_MULTIARYWT_H
#include <sequence/MultiArityNode.h>
#include <sequence/MultiArityCoder.h>
#include <sequence/Sequence.h>

using namespace std;

namespace cds_static
{

    class MultiArityWT: public Sequence{

    friend class MultiArityWTHelper;

    public:
        MultiArityWT(uint *seq, size_t len, MultiArityCoder *coder, SequenceBuilder *seq_builder);
        virtual size_t rank(uint c, size_t i) const;
        virtual size_t select(uint c, size_t i) const;
        virtual uint access(size_t i) const;
        virtual uint access(size_t i, size_t & r) const;
        virtual size_t getSize() const;
        virtual size_t getLength() const { return length; }
        virtual void save(ofstream & fp) const;
        static Sequence * load(ifstream & fp);
    protected:
        wt_coder *wtc;
        MultiArityNode *root;
        MultiArityCoder *coder;
        MultiArityWT();
    };

};

#endif //_ALL_MultiAryWT<T>_H_
