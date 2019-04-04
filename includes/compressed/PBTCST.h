//
// Created by elarielcl on 26/09/18.
//

#ifndef BLOCKTREE_PBTCST_H
#define BLOCKTREE_PBTCST_H

#include <compressed/BPSPCBlockTree.h>
#include <compressed/PBTCSA.h>
#include <compressed/PBTCISA.h>
#include <compressed/PBTCLCPA.h>
#include <compressed/PBTCTree.h>



class PBTCST {
public:

    static const int PAPER = 0;
    static const int PAPER_NO_CLEAN = 1;
    static const int PRUNIG_C = 2;
    static const int HEURISTIC = 3;
    static const int LIBERAL = 4;
    static const int CONSERVATIVE = 5;

    PCBlockTree* input_block_tree_;
    PBTCSA* pbtcSA_;
    PBTCISA* pbtcISA_;
    PBTCLCPA* pbtcLCPA_;
    PBTCTree* pbtcTree_;
    std::unordered_map<int, std::pair<int, int>> C_;

    int n_;

    PBTCST(std::string&, int = PAPER, int = 2, int = 4, int = 2, int = PAPER, int = 2, int = 4, int = 2, int = PAPER, int = 2, int = 4, int = 2, int = PAPER, int = 2, int = 4, int = 2, int = PAPER, int = 2, int = 16, int = 2);
    ~PBTCST();


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
    int select_child(int, int);
    int edge(int, int);
    int lca(int, int);
    int suffix_link(int);
    int suffix_link(int, int);
    int psi(int);
    int weiner_link(int, int);
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

#endif //BLOCKTREE_PBTCST_H
