//
// Created by alberto on 4/14/15.
//

#ifndef ALL_MULTIARITYINTERNALNODE_H
#define ALL_MULTIARITYINTERNALNODE_H
#include <sequence/Sequence.h>

#include <sequence/MultiArityNode.h>
#include <vector>
#include "MultiArityWTHelper.h"

namespace cds_static
{
class MultiArityInternalNode : public MultiArityNode{
    friend class MultiArityWTHelper;
    //enum multi_ary_node_type {leaf_node=0,internal_node=1};
    public:
        MultiArityInternalNode(uint *_seq, size_t len, uint level, MultiArityCoder *coder, SequenceBuilder *seq_builder);
        virtual ~MultiArityInternalNode();
        virtual size_t rank(uint c, size_t i,uint level, MultiArityCoder *coder) const;
        virtual size_t select(uint c, size_t i, uint level, MultiArityCoder *coder) const;
        virtual uint access(size_t i) const;
        virtual uint access(size_t i, size_t & r) const;
        virtual size_t getSize() const;
        virtual void save(ofstream & fp) const;
        static MultiArityInternalNode * load(ifstream & fp);
protected:
    MultiArityInternalNode(){}
    Sequence *seq;
    vector<MultiArityNode*> children;
    };
};


#endif //ALL_MULTIARITYINTERNALNODE_H
