//
// Created by alberto on 4/9/15.

#include <sequence/MultiArityLeafNode.h>

namespace cds_static {

        MultiArityLeafNode::MultiArityLeafNode(uint _symbol) : MultiArityNode() {
            this->symbol = _symbol;
        }

        MultiArityLeafNode::~MultiArityLeafNode() { }

        size_t MultiArityLeafNode::rank(uint c, size_t i, uint level, MultiArityCoder *coder) const {
            return i + 1;
        }

        size_t MultiArityLeafNode::select(uint c, size_t i, uint level, MultiArityCoder *coder) const {
            return i;
        }

        uint MultiArityLeafNode::access(size_t i) const {
            return symbol;
        }

        uint MultiArityLeafNode::access(size_t i, size_t &rank) const {
            rank = i + 1;
            return symbol;
        }

        size_t MultiArityLeafNode::getSize() const {
            return sizeof(this);
        }

        void MultiArityLeafNode::save(ofstream &fp) const {
            saveValue(fp, (uint) leaf_node);
            saveValue(fp, symbol);
        }
//
        MultiArityLeafNode *MultiArityLeafNode::load(ifstream &fp) {
            multi_arity_node_type t = static_cast<multi_arity_node_type>(loadValue<uint>(fp));
            if (t != leaf_node)
                return nullptr;
            MultiArityLeafNode *node = new MultiArityLeafNode();
            node->symbol = loadValue<uint>(fp);
            return node;
        }
}
