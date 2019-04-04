//
// Created by elarielcl on 1/30/19.
//

#ifndef BLOCKTREE_PBTLCSALCSACST_H
#define BLOCKTREE_PBTLCSALCSACST_H
#include <compressed/BPSPCBlockTree.h>
#include <compressed/PBTCTree.h>
#include <LCP_RLCSA.h>
#include <rlcsa/rlcsa.h>
#include <lcsa/repair_differential_array_with_lengths.hpp>

//#include <algo13/LCP_RLCSA.h>

class PBTLCSALCSACST {
public:

    static const int PAPER = 0;
    static const int PAPER_NO_CLEAN = 1;
    static const int PRUNIG_C = 2;
    static const int HEURISTIC = 3;
    static const int LIBERAL = 4;
    static const int CONSERVATIVE = 5;

    TextIndexes::RLCSA* rlcsa_;
    TextIndexRLCSA* index_;
    repair_differential_array_with_lengths::repair_differential_array_with_lengths* dsa_;
    repair_differential_array_with_lengths::repair_differential_array_with_lengths* disa_;
    static const int disa_constant = 32;
    //PCBlockTree* input_block_tree_;
    cds_static::LCP_RLCSA* lcp_rlcsa_;
    PBTCTree* pbtcTree_;



    int64_t* children;
    //std::unordered_map<int, std::pair<int, int>> C_; //UNUSED?

    int64_t node_ = -2;
    int64_t i_ = -2;
    int64_t psi_ = -2;
    int n_;
    //int pa_;
    //int pa2_;

    PBTLCSALCSACST(std::string&, int = PAPER, int = 2, int = 16, int = 2, int = 32, int = 128);
    PBTLCSALCSACST(std::string&, std::string&, int = PAPER, int = 2, int = 16, int = 2, int = 32, int = 128);
    ~PBTLCSALCSACST();


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
    std::string string(int, int, int);
    int labeled_child(int, int);
    int bin_search_labeled_child(int, int);
    int bin_search_count_labeled_child(int, int);
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
    std::vector<int> locate(int);
    void print_statistics();
};
#endif //BLOCKTREE_PBTLCSALCSACST_H
