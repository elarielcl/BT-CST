//
// Created by elarielcl on 11/29/18.
//

#ifndef BLOCKTREE_PBTCTREENOBACK_H
#define BLOCKTREE_PBTCTREENOBACK_H
#include <compressed/BPSPCBlockTree.h>


class PBTCTreenoback {
public:

    static const int PAPER = 0;
    static const int PAPER_NO_CLEAN = 1;
    static const int PRUNIG_C = 2;
    static const int HEURISTIC = 3;
    static const int LIBERAL = 4;
    static const int CONSERVATIVE = 5;

    int root_ = 0;
    BPSPCBlockTree* bps_;

    PBTCTreenoback(std::string&, int = PAPER, int = 2, int = 16, int = 2);
    ~PBTCTreenoback();


    int root();
    int first_child(int);
    int last_child(int);
    int next_sibling(int);
    int previous_sibling(int);
    int parent(int);
    bool is_leaf(int);
    bool is_leaf_rank(int, int&);
    int node_map(int);
    int node_select(int);
    int postorder(int);
    int postorder_select(int);
    int depth(int);
    int subtree_size(int);
    bool is_ancestor(int, int);
    int level_ancestor(int, int);
    int leaf_rank(int);
    int number_of_leaves(int);
    int leaf_select(int);
    int lca(int, int);
    int child_rank(int);
    int child(int, int);
    int number_of_children(int);
    int deepest_node(int);
    int height(int);


    void print_statistics();
    int get_size();
};

#endif //BLOCKTREE_PBTCTREENOBACK_H
