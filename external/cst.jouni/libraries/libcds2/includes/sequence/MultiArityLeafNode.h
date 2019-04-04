//
// Created by alberto on 4/14/15.
//

#ifndef ALL_MULTIARITYLEAFNODE_H
#define ALL_MULTIARITYLEAFNODE_H
#include <sequence/MultiArityNode.h>
namespace cds_static{
class MultiArityLeafNode : public MultiArityNode{

    public:
        MultiArityLeafNode(uint symbol);
        virtual ~MultiArityLeafNode();
        virtual size_t rank(uint c, size_t i,uint level, MultiArityCoder *coder) const;
        virtual size_t select(uint c, size_t i, uint level, MultiArityCoder *coder) const;
        virtual uint access(size_t i) const;
        virtual uint access(size_t i, size_t & r) const;
        virtual size_t getSize() const;
        virtual void save(ofstream & fp) const;
        static MultiArityLeafNode * load(ifstream & fp);
protected:
    MultiArityLeafNode(){}
    uint symbol;
    };
};


#endif //ALL_MULTIARITYLEAFNODE_H
