//
// Created by sking32 on 5/31/18.
//

#ifndef BLOCKTREE_BPSPCBLOCKTREE_H
#define BLOCKTREE_BPSPCBLOCKTREE_H
#include <compressed/PCBlockTree.h>
class BPSPCBlockTree : public PCBlockTree {
public:
    std::vector<sdsl::bit_vector::select_1_type*> bt_bv_select_1_;
    std::vector<sdsl::rrr_vector<63>::select_1_type*> c_bt_bv_select_1_;


    int open_;
    int new_alphabet_open_;
    int n_last_level_;
    int n_pre_last_level_;
    int n_internal_nodes_;


    sdsl::int_vector<>* bt_first_level_prefix_leaf_ranks_;
    sdsl::int_vector<>* bt_first_level_cumulated_leaf_ranks_;

    std::vector<sdsl::int_vector<>*> bt_prefix_leaf_ranks_;
    std::vector<sdsl::int_vector<>*> bt_cumulated_leaf_ranks_;
    std::vector<sdsl::int_vector<>*> bt_leaf_ranks_;

    std::vector<sdsl::int_vector<>*> bt_first_leaf_ranks_;
    std::vector<sdsl::int_vector<>*> bt_second_leaf_ranks_;


    std::vector<sdsl::bit_vector*> bt_min_in_first_block_;
    std::vector<sdsl::bit_vector*> bt_min_in_both_blocks_;
    std::vector<sdsl::rank_support_v<1>*>  bt_min_in_both_blocks_rank_;
    std::vector<sdsl::bit_vector*> bt_max_in_first_block_;
    std::vector<sdsl::bit_vector*> bt_max_in_both_blocks_;
    std::vector<sdsl::rank_support_v<1>*>  bt_max_in_both_blocks_rank_;

    //sdsl::int_vector<>* bt_jea15_first_level_prefix_leaf_ranks_; //Jea15 encoding
    //sdsl::int_vector<>* bt_jea15_first_level_cumulated_leaf_ranks_; //Jea15 encoding

    //std::vector<sdsl::int_vector<>*> bt_jea15_prefix_leaf_ranks_; //Jea15 encoding
    //std::vector<sdsl::int_vector<>*> bt_jea15_cumulated_leaf_ranks_; //Jea15 encoding
    std::vector<sdsl::int_vector<>*> bt_jea15_leaf_ranks_; //Jea15 encoding

    std::vector<sdsl::int_vector<>*> bt_jea15_first_leaf_ranks_; //Jea15 encoding
    std::vector<sdsl::int_vector<>*> bt_jea15_second_leaf_ranks_; //Jea15 encoding

    sdsl::int_vector<>* bt_first_level_prefix_zeros_ranks_; //Jea15 encoding
    sdsl::int_vector<>* bt_first_level_cumulated_zeros_ranks_; //Jea15 encoding

    std::vector<sdsl::int_vector<>*> bt_prefix_zeros_ranks_; //Jea15 encoding
    std::vector<sdsl::int_vector<>*> bt_cumulated_zeros_ranks_; //Jea15 encoding
    std::vector<sdsl::int_vector<>*> bt_zeros_ranks_; //Jea15 encoding

    std::vector<sdsl::int_vector<>*> bt_first_zeros_ranks_; //Jea15 encoding
    std::vector<sdsl::int_vector<>*> bt_second_zeros_ranks_; //Jea15 encoding

    std::vector<sdsl::bit_vector*> bt_starts_with_end_leaf_;
    std::vector<sdsl::bit_vector*> bt_suffix_start_with_end_leaf_;
    std::vector<sdsl::bit_vector*> bt_prefix_start_with_end_leaf_;

    //ESTO
    sdsl::int_vector<>* top_excess_;
    sdsl::int_vector<>* top_min_excess_;
    sdsl::int_vector<>* top_max_excess_;
    sdsl::int_vector<>* top_min_count_;

    sdsl::int_vector<>* bottom_excess_; //HARCODED
    sdsl::int_vector<>* bottom_min_excess_; //HARCODED
    sdsl::int_vector<>* bottom_max_excess_; //HARCODED
    sdsl::int_vector<>* bottom_min_count_; //HARCODED
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bottom_excess_; //HARCODED
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bottom_min_excess_; //HARCODED
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bottom_max_excess_; //HARCODED
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bottom_min_count_; //HARCODED
    std::vector<int> bottom_min_count_2_; //HARCODED
    std::vector<int> bottom_excess_2_; //HARCODED
    std::vector<int> bottom_min_excess_2_; //HARCODED
    std::vector<int> bottom_max_excess_2_; //HARCODED
    int bottom_bit_size_; //HARCODED

    /*
    sdsl::int_vector<>* bt_deleted_excess_;
    sdsl::int_vector<>* bt_deleted_min_excess_;
    sdsl::int_vector<>* bt_deleted_max_excess_;
    sdsl::int_vector<>* bt_deleted_min_count_;
*/

    std::vector<sdsl::int_vector<>*> bt_excess_; //Starts with the local excess of first level
    std::vector<sdsl::int_vector<>*> bt_excess_first_block_;
    std::vector<sdsl::int_vector<>*> bt_excess_second_block_;

    //sdsl::int_vector<>* bt_first_level_prefix_min_excess_;
    std::vector<sdsl::int_vector<>*> bt_prefix_min_excess_; //Starts with the local min of first level
    std::vector<sdsl::int_vector<>*> bt_prefix_min_excess_first_block_;
    std::vector<sdsl::int_vector<>*> bt_prefix_min_excess_second_block_;

    std::vector<sdsl::int_vector<>*> bt_prefix_min_excess_back_block_;

    //sdsl::int_vector<>* bt_first_level_prefix_max_excess_;
    std::vector<sdsl::int_vector<>*> bt_prefix_max_excess_; //Starts with the local max of first level
    std::vector<sdsl::int_vector<>*> bt_prefix_max_excess_first_block_;
    std::vector<sdsl::int_vector<>*> bt_prefix_max_excess_second_block_;

    std::vector<sdsl::int_vector<>*> bt_prefix_max_excess_back_block_;

    //sdsl::int_vector<>* bt_first_level_min_count_; //DO not what to put here, It could be prefix and suffix mincount
    std::vector<sdsl::int_vector<>*> bt_min_count_; //Starts with the local count of first level
    std::vector<sdsl::int_vector<>*> bt_min_count_first_block_;
    std::vector<sdsl::int_vector<>*> bt_min_count_second_block_;

    std::vector<sdsl::int_vector<>*> bt_min_count_back_block_;

    //Compressed Versions -> no significant time penalty

    std::vector<sdsl::rrr_vector<63>*> c_bt_min_in_first_block_;
    std::vector<sdsl::rrr_vector<63>*> c_bt_min_in_both_blocks_;
    std::vector<sdsl::rrr_vector<63>::rank_1_type*>  c_bt_min_in_both_blocks_rank_;
    std::vector<sdsl::rrr_vector<63>*> c_bt_max_in_first_block_;
    std::vector<sdsl::rrr_vector<63>*> c_bt_max_in_both_blocks_;
    std::vector<sdsl::rrr_vector<63>::rank_1_type*>  c_bt_max_in_both_blocks_rank_;


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

    //sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bt_jea15_first_level_prefix_leaf_ranks_; //Jea15 encoding
    //sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bt_jea15_first_level_cumulated_leaf_ranks_; //Jea15 encoding

    //std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_jea15_prefix_leaf_ranks_; //Jea15 encoding
    //std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_jea15_cumulated_leaf_ranks_; //Jea15 encoding
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_jea15_leaf_ranks_; //Jea15 encoding

    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_jea15_first_leaf_ranks_; //Jea15 encoding
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_jea15_second_leaf_ranks_; //Jea15 encoding



    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bt_first_level_prefix_zeros_ranks_; //Jea15 encoding
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bt_first_level_cumulated_zeros_ranks_; //Jea15 encoding

    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_prefix_zeros_ranks_; //Jea15 encoding
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_cumulated_zeros_ranks_; //Jea15 encoding
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_zeros_ranks_; //Jea15 encoding

    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_first_zeros_ranks_; //Jea15 encoding
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_second_zeros_ranks_; //Jea15 encoding

    //ESTO
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_top_excess_;
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_top_min_excess_;
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_top_max_excess_;
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_top_min_count_;
/*
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bt_deleted_excess_;
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bt_deleted_min_excess_;
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bt_deleted_max_excess_;
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bt_deleted_min_count_;
*/

    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_excess_;
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_excess_first_block_;
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_excess_second_block_;

    //sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bt_first_level_prefix_min_excess_;
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_prefix_min_excess_;
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_prefix_min_excess_first_block_;
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_prefix_min_excess_second_block_;

    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_prefix_min_excess_back_block_;

    //sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bt_first_level_prefix_max_excess_;
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_prefix_max_excess_; //Starts with the local max of first level
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_prefix_max_excess_first_block_;
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_prefix_max_excess_second_block_;

    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_prefix_max_excess_back_block_;

    //sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bt_first_level_min_count_; //DO not what to put here, It could be prefix and suffix mincount
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_min_count_; //Starts with the local count of first level
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_min_count_first_block_;
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_min_count_second_block_;

    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_min_count_back_block_;


    BPSPCBlockTree(PBlockTree*);
    ~BPSPCBlockTree();


    bool is_leaf_rank_alternative(int, int&);
    bool is_leaf_rank_alternative_noback(int, int&);

    int excess(int);
    int excess_alternative(int);
    int excess_alternative_noback(int);
    int better_excess(int);

    int rank_1(int);
    int rank_1_alternative(int);
    int rank_1_alternative_noback(int);
    int better_rank_1(int);

    int rank_0(int);
    int rank_0_alternative(int);
    int rank_0_alternative_noback(int);
    int better_rank_0(int);

    int select_1(int);
    int select_1_alternative(int);
    int select_1_alternative_noback(int);
    int better_select_1(int);

    int select_0(int);
    int select_0_alternative(int);
    int select_0_alternative_noback(int);
    int better_select_0(int);

    int rank_10(int);
    int rank_10_alternative(int);
    int better_rank_10(int);

    int rank_10_alternative_noback(int);
    int recursive_rank_10_alternative_noback(int, int, int, int);

    int select_10(int);
    int select_10_alternative(int);
    int better_select_10(int);

    int select_10_alternative_noback(int);

    int fwdsearch(int,int);
    int fwdsearch(int,int,int,int,int, int, int&);
    int next_block_fwdsearch(int,int,int&);
    int faster_next_block_fwdsearch(int,int,int&);


    int my_fwdsearch_nobackmin(int,int);
    int my_fwdsearch_nobackmin(int,int,int,int,int, int, int&);


    int my_fwdsearch_cback(int,int);
    int my_fwdsearch_cback(int,int,int,int,int, int, int&);

    int my_fwdsearch(int,int);
    int my_fwdsearch(int,int,int,int,int, int, int&);
    int my_next_block_fwdsearch(int,int,int&);
    int my_faster_next_block_fwdsearch(int,int,int&);


    int bwdsearch(int,int);
    int bwdsearch(int,int,int,int,int, int, int&);
    int next_block_bwdsearch(int,int,int&);
    int faster_next_block_bwdsearch(int,int,int&);


    int c_bwdsearch(int,int);
    int c_bwdsearch(int,int,int,int,int, int, int&);
    int c_next_block_bwdsearch(int,int,int&);
    int c_faster_next_block_bwdsearch(int,int,int&);



    int my_bwdsearch_cback(int, int);
    int my_bwdsearch_cback(int,int,int,int,int, int, int&);

    int my_bwdsearch_nobackmin(int,int);
    int my_bwdsearch_nobackmin(int,int,int,int,int, int, int&);

    int my_bwdsearch(int,int);
    int my_bwdsearch(int,int,int,int,int, int, int&);
    int my_next_block_bwdsearch(int,int,int&);
    int my_faster_next_block_bwdsearch(int,int,int&);

    int myc_bwdsearch(int,int);
    int myc_bwdsearch(int,int,int,int,int, int, int&);
    int myc_next_block_bwdsearch(int,int,int&);
    int myc_faster_next_block_bwdsearch(int,int,int&);



    int positive_fwdsearch(int, int);
    int positive_fwdsearch(int,int,int,int,int, int, int&);
    int next_block_positive_fwdsearch(int,int,int&);
    int faster_next_block_positive_fwdsearch(int,int,int&);


    int my_positive_fwdsearch_nobackmax(int, int);
    int my_positive_fwdsearch_nobackmax(int,int,int,int,int, int, int&);

    int my_positive_fwdsearch(int, int);
    int my_positive_fwdsearch(int,int,int,int,int, int, int&);
    int my_next_block_positive_fwdsearch(int,int,int&);
    int my_faster_next_block_positive_fwdsearch(int,int,int&);


    int positive_bwdsearch(int,int);
    int positive_bwdsearch(int,int,int,int,int, int, int&);
    int next_block_positive_bwdsearch(int,int,int&);
    int faster_next_block_positive_bwdsearch(int,int,int&);


    int my_positive_bwdsearch_nobackmax(int,int);
    int my_positive_bwdsearch_nobackmax(int,int,int,int,int, int, int&);

    int my_positive_bwdsearch(int,int);
    int my_positive_bwdsearch(int,int,int,int,int, int, int&);
    int my_next_block_positive_bwdsearch(int,int,int&);
    int my_faster_next_block_positive_bwdsearch(int,int,int&);

    int min_excess(int,int);
    int min_excess(int,int,int,int, int, int&);
    int next_block_min_excess(int,int,int&);
    int faster_next_block_min_excess(int,int,int&);


    int my_min_excess_nobackmin(int,int);
    int my_min_excess_nobackmin(int,int,int,int, int, int&);

    int my_min_excess_cback(int,int);
    int my_min_excess_cback(int,int,int,int, int, int&);

    int my_min_excess(int,int);
    int my_min_excess(int,int,int,int, int, int&);
    int my_next_block_min_excess(int,int,int&);
    int my_faster_next_block_min_excess(int,int,int&);


    int my_min_count_nobackmin(int,int);
    int my_min_count_nobackmin(int,int,int,int, int, int&, int&);

    int my_min_count(int,int);
    int my_min_count(int,int,int,int, int, int&, int&);
    int my_next_block_min_count(int,int,int&,int&);
    int my_faster_next_block_min_count(int,int,int&,int&);

    int min_count(int,int);
    int min_count(int,int,int,int, int, int&, int&);
    int next_block_min_count(int,int,int&,int&);
    int faster_next_block_min_count(int,int,int&,int&);


    int my_min_select_nobackmin(int,int,int);
    int my_min_select_nobackmin(int,int,int&,int,int, int, int&, int);

    int my_min_select(int,int,int,int = 3);
    int my_min_select(int,int,int&,int,int, int, int&, int);
    int my_next_block_min_select(int,int,int&,int&,int);
    int my_faster_next_block_min_select(int,int,int&,int&,int);

    int min_select(int,int,int);
    int min_select(int,int,int&,int,int, int, int&, int);
    int next_block_min_select(int,int,int&,int&,int);
    int faster_next_block_min_select(int,int,int&,int&,int);




    int max_excess(int,int);
    int max_excess(int,int,int,int, int, int&);
    int next_block_max_excess(int,int,int&);
    int faster_next_block_max_excess(int,int,int&);

    int my_max_excess_nobackmax(int,int);
    int my_max_excess_nobackmax(int,int,int,int, int, int&);

    int my_max_excess(int,int);
    int my_max_excess(int,int,int,int, int, int&);
    int my_next_block_max_excess(int,int,int&);
    int my_faster_next_block_max_excess(int,int,int&);

    void print_statistics();
};

int64_t encoded_excess(int64_t);
int64_t decoded_excess(int64_t);
#endif //BLOCKTREE_BPSPCBLOCKTREE_H
