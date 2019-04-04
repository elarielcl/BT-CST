//
// Created by sking32 on 5/28/18.
//

#ifndef BLOCKTREE_BPSCBLOCKTREE_H
#define BLOCKTREE_BPSCBLOCKTREE_H

#include <compressed/CBlockTree.h>
class BPSCBlockTree : public CBlockTree {
public:
    int open_;
    sdsl::int_vector<>* bt_first_level_prefix_leaf_ranks_;
    sdsl::int_vector<>* bt_first_level_cumulated_leaf_ranks_;

    std::vector<sdsl::int_vector<>*> bt_prefix_leaf_ranks_;
    std::vector<sdsl::int_vector<>*> bt_cumulated_leaf_ranks_;
    std::vector<sdsl::int_vector<>*> bt_leaf_ranks_;

    std::vector<sdsl::int_vector<>*> bt_first_leaf_ranks_;
    std::vector<sdsl::int_vector<>*> bt_second_leaf_ranks_;

    std::vector<sdsl::bit_vector*> bt_starts_with_end_leaf_;
    std::vector<sdsl::bit_vector*> bt_suffix_start_with_end_leaf_;
    std::vector<sdsl::bit_vector*> bt_prefix_start_with_end_leaf_;

    //Compressed Versions -> no significant time penalty
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bt_first_level_prefix_leaf_ranks_;
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bt_first_level_cumulated_leaf_ranks_;
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_prefix_leaf_ranks_;
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_cumulated_leaf_ranks_;
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_leaf_ranks_;

    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_first_leaf_ranks_;
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_second_leaf_ranks_;
    std::vector<sdsl::rrr_vector<63>*> c_bt_starts_with_end_leaf_;
    std::vector<sdsl::rrr_vector<63>*> c_bt_suffix_start_with_end_leaf_;
    std::vector<sdsl::rrr_vector<63>*> c_bt_prefix_start_with_end_leaf_;


    BPSCBlockTree(BlockTree*);
    ~BPSCBlockTree();

    int excess(int);
    int rank_10(int);
    int rank_10_alternative(int);
    int better_rank_10(int);

    int select_10(int);
    int select_10_alternative(int);
    int better_select_10(int);

    void print_statistics();
};

#endif //BLOCKTREE_BPSCBLOCKTREE_H
