//
// Created by alberto on 4/13/15.
//

#ifndef ALL_MULTIARITYNODE_H
#define ALL_MULTIARITYNODE_H
#include <utils/libcdsBasicsGeneric.h>
#include <sequence/MultiArityCoder.h>

using namespace cds_utils;

namespace cds_static
{
        enum multi_arity_node_type {leaf_node=0,internal_node=1};
        class MultiArityNode{

        public:

        virtual ~MultiArityNode(){}
        virtual size_t rank(uint c, size_t i,uint level, MultiArityCoder *coder) const=0;
        virtual size_t select(uint c, size_t i, uint level, MultiArityCoder *coder) const=0;
        virtual uint access(size_t i) const=0;
        virtual uint access(size_t i, size_t & r) const=0;
        virtual size_t getSize() const=0;
        virtual void save(ofstream & fp) const=0;
        static MultiArityNode * load(ifstream & fp);
        protected:
            MultiArityNode(){}
        };
};
#include <sequence/MultiArityInternalNode.h>
#include <sequence/MultiArityLeafNode.h>

#endif //ALL_MULTIARITYNODE_H
