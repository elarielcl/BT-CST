//
// Created by alberto on 4/13/15.
//

#include <sequence/MultiArityInternalNode.h>

namespace cds_static{

    MultiArityInternalNode::MultiArityInternalNode(uint *_seq, size_t len, uint level, MultiArityCoder *coder, SequenceBuilder *seq_builder):MultiArityNode(){
            //TODO: manage base case
            vector<uint> sub_symbols(len);
            uint nchildren = 0;
            for (size_t i=0UL;i<len;i++){
                sub_symbols[i] = coder->get_chunk(_seq[i],level);
                nchildren = max(nchildren,sub_symbols[i]);
            }
            vector<uint> counter(nchildren,0);
            vector<vector<uint>> children_symbols(nchildren+1);
            for (size_t i=0UL;i<len;i++) {
                counter[sub_symbols[i]]++;
                children_symbols[sub_symbols[i]].push_back(_seq[i]);
            }
            //build the sequence (not the bitmap) associated with each WT level.
            seq = seq_builder->build(&sub_symbols[0],len);
            for (uint i=0;i<=nchildren;i++){
                if (counter[i]==0){
                    children.push_back(nullptr);
                }else{
                    if (level < (coder->get_max_level(children_symbols[i][0])-1)){
                        children.push_back(new MultiArityInternalNode(&children_symbols[i][0],children_symbols[i].size(),level+1,coder,seq_builder));
                    }else{
                        children.push_back(new MultiArityLeafNode(children_symbols[i][0]));
                    }
                }
            }
        }


    MultiArityInternalNode::~MultiArityInternalNode(){
            for (const auto &child: children){
                if (child)
                    delete child;
            }
        }

        size_t MultiArityInternalNode::rank(uint c, size_t i, uint level, MultiArityCoder *coder) const {
            uint code = coder->get_chunk(c,level);
            assert(children[code]!=nullptr);
            return children[code]->rank(c, static_cast<size_t>(seq->rank(code,i)-1),level+1,coder);
        }

        size_t MultiArityInternalNode::select(uint c, size_t i, uint level, MultiArityCoder *coder) const{
            uint code = coder->get_chunk(c,level);
            assert(children[code]!=nullptr);
            uint s = children[code]->select(c,i,level+1,coder);
            return seq->select(code,s)+1;
        }

        uint MultiArityInternalNode::access(size_t pos) const{
            size_t rOpt = 0;
            uint value = seq->access(pos,rOpt);
            assert(value<children.size());
            assert(children[value]!=nullptr);
            return children[value]->access(rOpt-1);
        }

        uint MultiArityInternalNode::access(size_t pos, size_t &r) const{
            size_t rOpt = 0;
            uint value = seq->access(pos,rOpt);
            assert(value<children.size());
            assert(children[value]!=nullptr);
            return children[value]->access(rOpt-1);
        }

        size_t MultiArityInternalNode::getSize() const{
            size_t size = sizeof(this);
            for (auto &child: children){
                size+=child->getSize();
            }
            return size;
        }

        void MultiArityInternalNode::save(ofstream & fp) const{
            saveValue(fp,(uint)internal_node);
            saveValue(fp,(uint)children.size());
            seq->save(fp);
            for (const auto ch: children)
                ch->save(fp);
        }

        MultiArityInternalNode*  MultiArityInternalNode::load(ifstream & fp){
            multi_arity_node_type t= static_cast<multi_arity_node_type>(loadValue<uint>(fp));
            if (t!=internal_node)
                return nullptr;
            MultiArityInternalNode *node = new MultiArityInternalNode();
            uint nchildren = loadValue<uint>(fp);
            node->seq = Sequence::load(fp);
            for(uint i=0U;i<nchildren;i++){
                node->children[i] = MultiArityNode::load(fp);
            }
            return node;
        }


}

