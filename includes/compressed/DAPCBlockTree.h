//
// Created by sking32 on 7/31/18.
//

#ifndef BLOCKTREE_DAPCBLOCKTREE_H
#define BLOCKTREE_DAPCBLOCKTREE_H
#include <string>
#include <unordered_map>

#include <sdsl/bit_vectors.hpp>
#include <sdsl/vectors.hpp>
#include <sdsl/wavelet_trees.hpp>
#include "padding_blocktree/PBlockTree.h"

class DAPCBlockTree {
public:
    int r_; // Arity
    int leaf_length_;
    int first_level_length_;
    int number_of_levels_;
    int number_of_blocks_first_level_;
    std::basic_string<int64_t> winput_;

    std::vector<sdsl::bit_vector*> bt_bv_; // 1 when is Internal Block
    std::vector<sdsl::rank_support_v<1>*> bt_bv_rank_;
    std::vector<sdsl::int_vector<>*> bt_offsets_;
    sdsl::int_vector<>* leaf_string_;
    sdsl::int_vector<>* sum_leaf_string_; //COMPLETE

    //sdsl::int_vector<>* alphabet_; //Problem here
    //std::unordered_map<char,int> mapping_; //Problem here


    sdsl::int_vector<>* bt_first_level_prefix_sum_;
    sdsl::int_vector<>* bt_first_level_cumulated_sum_;

    std::vector<sdsl::int_vector<>*> bt_prefix_sum_;
    std::vector<sdsl::int_vector<>*> bt_cumulated_sum_;
    std::vector<sdsl::int_vector<>*> bt_sum_;

    std::vector<sdsl::int_vector<>*> bt_first_sum_;
    std::vector<sdsl::int_vector<>*> bt_second_sum_;


    //Compressed Versions -> no significant time penalty
    std::vector<sdsl::rrr_vector<63>*> c_bt_bv_; // 1 when is Internal Block
    std::vector<sdsl::rrr_vector<63>::rank_1_type*> c_bt_bv_rank_;


    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_offsets_;
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_leaf_string_;
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_sum_leaf_string_; // COMPLETE


    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bt_first_level_prefix_sum_;
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_bt_first_level_cumulated_sum_;
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_prefix_sum_;
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_cumulated_sum_;
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_sum_;

    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_first_sum_;
    std::vector<sdsl::dac_vector_dp<sdsl::rrr_vector<>>*> c_bt_second_sum_;


    //sdsl::wt_blcd<>* leaf_string_wt_;


    sdsl::sd_vector<>* leaf_string_map_;
    sdsl::rank_support_sd<1>* leaf_string_map_m_rank_support_;
    sdsl::select_support_sd<1>* leaf_string_map_m_select_support_;
    sdsl::int_vector<>* mapped_leaf_string_;
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_mapped_leaf_string_;

    sdsl::sd_vector<>* sum_leaf_string_map_;
    sdsl::rank_support_sd<1>* sum_leaf_string_map_m_rank_support_;
    sdsl::select_support_sd<1>* sum_leaf_string_map_m_select_support_;
    sdsl::int_vector<>* sum_mapped_leaf_string_;
    sdsl::dac_vector_dp<sdsl::rrr_vector<>>* c_sum_mapped_leaf_string_;

    DAPCBlockTree(PBlockTree*);
    virtual ~DAPCBlockTree();

    int64_t access(int);
    int64_t differential_access(int);
    int64_t sdifferential_access(int);
    int64_t differential_access_alternative(int);
    int64_t sdifferential_access_alternative(int);
    int64_t better_differential_access(int);
    int64_t sbetter_differential_access(int);



    int64_t caccess(int);
    int64_t cdifferential_access(int);
    int64_t csdifferential_access(int);
    int64_t cdifferential_access_alternative(int);
    int64_t csdifferential_access_alternative(int);
    int64_t cbetter_differential_access(int);
    int64_t csbetter_differential_access(int);



    int64_t mapped_differential_access_alternative(int);
    int64_t mapped_sdifferential_access_alternative(int);

    int64_t mapped_cdifferential_access_alternative(int);
    int64_t mapped_csdifferential_access_alternative(int);

    virtual void print_statistics();
};

int64_t encoded_sum(int64_t);
int64_t decoded_sum(int64_t);

#endif //BLOCKTREE_DAPCBLOCKTREE_H
