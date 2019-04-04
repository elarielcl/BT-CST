//
// Created by elarielcl on 11/11/18.
//

#ifndef BLOCKTREE_CST_SADA_RLCSA_H
#define BLOCKTREE_CST_SADA_RLCSA_H

#include <compressed/BPSPCBlockTree.h>
#include <compressed/PBTCTree.h>
#include <LCP_RLCSA.h>
#include <rlcsa/rlcsa.h>
#include <sdsl/cst_sada.hpp>
class CST_SADA_RLCSA {
public:

    TextIndexes::RLCSA* rlcsa_;
    TextIndexRLCSA* index_;
    //PCBlockTree* input_block_tree_;
    cds_static::LCP_RLCSA* lcp_rlcsa_;
    sdsl::bit_vector* pm_bp;
    sdsl::bp_support_sada<>* pm_bp_support; // support for the balanced parentheses sequence
    sdsl::rank_support_v5<10,2>*    pm_bp_rank10;  // rank_support for leaves, i.e. "10" bit pattern
    sdsl::select_support_mcl<10,2>*  pm_bp_select10;// select_support for leaves, i.e. "10" bit pattern

    //std::unordered_map<int, std::pair<int, int>> C_; //UNUSED?

    int n_;
    //int pa_;
    //int pa2_;

    CST_SADA_RLCSA(std::string&, int = 32, int = 128);
    CST_SADA_RLCSA(std::string&, std::string&, int = 32, int = 128);
    ~CST_SADA_RLCSA();


    int access(int);

    int root();
    int is_leaf(int);
    int select_leaf(int);
    int string_depth(int);
    int node_depth(int);
    int size(int);
    int leftmost_leaf(int);
    int rightmost_leaf(int);
    int lb(int);
    int rb(int);
    int parent(int);
    int next_sibling(int);
    int string(int, int);
    int64_t node_ = -2;
    int64_t i_ = -2;
    int64_t psi_ = -2;
    std::string string(int, int, int);
    int labeled_child(int, int);
    int select_child(int, int);
    int edge(int, int);
    int lca(int, int);
    int suffix_link(int);
    int suffix_link(int, int);
    //int psi(int);
    //int weiner_link(int, int);
    int sn(int);
    int id(int);
    int inv_id(int);
    int nodes();
    int node(int, int);
    int degree(int);
    int count(int);
    int locus(std::string);
    int string_ancestor(int, int);
    int level_ancestor(int, int);
    std::vector<int> locate(int);
    void print_statistics();
    uint64_t inorder(uint64_t v)const
    {
        return (*pm_bp_rank10)(pm_bp_support->find_close(v+1)+1);
    }
};

#endif //BLOCKTREE_CST_SADA_RLCSA_H
