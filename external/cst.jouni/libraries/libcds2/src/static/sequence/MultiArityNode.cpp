//
// Created by alberto on 4/13/15.
//

#include <sequence/MultiArityNode.h>

namespace cds_static {

    MultiArityNode *MultiArityNode::load(ifstream & fp) {
        multi_arity_node_type t;
        fp.read((char *) &t, sizeof(uint));
        size_t pos = static_cast<size_t>(fp.tellg());
        fp.seekg(pos - sizeof(uint), ios::beg);
        if (t == leaf_node) {
            return MultiArityLeafNode::load(fp);
        } else if (t == internal_node) {
            return MultiArityInternalNode::load(fp);
        } else {
            return nullptr;
        }
    }
}