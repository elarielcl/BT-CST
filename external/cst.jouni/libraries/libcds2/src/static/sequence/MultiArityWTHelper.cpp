//
// Created by alberto on 4/14/15.
//
#include <sequence/MultiArityWTHelper.h>

namespace  cds_static {

        MultiArityWTHelper::MultiArityWTHelper() { }

        uint MultiArityWTHelper::getDepth(const MultiArityWT &wt) {
            return getDepthRec(wt.root);
        }

        const Sequence *MultiArityWTHelper::getSequence(const MultiArityWT &wt, const vector<uint> &node_id) {
            MultiArityNode *node = wt.root;
            MultiArityInternalNode *internal=nullptr;
            for (const auto &it: node_id){

                if (dynamic_cast<MultiArityLeafNode *>(node)) return nullptr;
                internal = dynamic_cast<MultiArityInternalNode *>(node);
                if (internal==nullptr)
                    return nullptr;
                if (it>=internal->children.size())
                    return nullptr;
                node = internal->children[it];
            }
            return internal->seq;
        }

        const vector<MultiArityNode *>* MultiArityWTHelper::getChildren(const MultiArityWT &wt, const vector<uint> &node_id){
            MultiArityNode *node = wt.root;
            assert(node_id[0]==0);
            auto it = 0;
            while(++it<node_id.size()){
                MultiArityInternalNode *internal = dynamic_cast<MultiArityInternalNode*>(node);
                if (internal==nullptr){
                    return nullptr;
                }else{
                    node = internal->children[it];
                }
            }

            return static_cast<const vector<MultiArityNode*>*>(&(dynamic_cast<MultiArityInternalNode*>(node)->children));

        }

        uint MultiArityWTHelper::getDepthRec(MultiArityNode *node) {
            MultiArityLeafNode *n = dynamic_cast<MultiArityLeafNode *>(node);
            if (n != nullptr) {
                return 0;
            } else {
                MultiArityInternalNode *n = dynamic_cast<MultiArityInternalNode *>(node);
                uint maxDepth = 0;
                for (const auto &ch: n->children) {
                    if (ch) {
                        maxDepth = max(maxDepth, getDepthRec(ch));
                    }
                }
                return 1+maxDepth;
            }
        }
}

