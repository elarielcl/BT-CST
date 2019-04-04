//
// Created by sking32 on 5/31/18.
//

#include <compressed/BPSPCBlockTree.h>

BPSPCBlockTree::BPSPCBlockTree(PBlockTree * bt) : PCBlockTree(bt) {
    open_ = bt->open_;
    new_alphabet_open_ = mapping_[open_];
    std::vector<PBlock*> first_level = {bt->root_block_};

    bool is_first_level = false;
    int deleted_levels = 0;
    while (!is_first_level) {
        for (PBlock* b: first_level) {
            is_first_level = is_first_level || b->is_leaf();
        }
        if (is_first_level) break;
        first_level = bt->next_level(first_level);
        ++deleted_levels;
    }

    /*
    first_level = {bt->root_block_};


    //ESTO
    std::vector<int> deleted_excess;
    std::vector<int> deleted_min_excess;
    std::vector<int> deleted_max_excess;
    std::vector<int> deleted_min_count;
    for (int dl = 0; dl < deleted_levels; ++dl) {
        for (PBlock* b: first_level) {
            int excess = 0;
            for (auto pair : b->ranks_) {
                if (pair.first == bt->input_[0]) excess+= pair.second;
                else excess -= pair.second;
            }
            deleted_excess.push_back(excess);
            deleted_min_excess.push_back(b->min_prefix_excess_);
            deleted_max_excess.push_back(b->max_prefix_excess_);
            deleted_min_count.push_back(b->prefix_min_count_);
        }
        first_level = bt->next_level(first_level);
    }

    bt_deleted_excess_ = new sdsl::int_vector<>(deleted_excess.size());
    for (int k = 0; k < deleted_excess.size(); ++k) {
        (*bt_deleted_excess_)[k] = deleted_excess[k];
    }
    sdsl::util::bit_compress(*bt_deleted_excess_);
    c_bt_deleted_excess_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*(bt_deleted_excess_));

    bt_deleted_min_excess_ = new sdsl::int_vector<>(deleted_min_excess.size());
    for (int k = 0; k < deleted_min_excess.size(); ++k) {
        (*bt_deleted_min_excess_)[k] = deleted_min_excess[k];
    }
    sdsl::util::bit_compress(*bt_deleted_min_excess_);
    c_bt_deleted_min_excess_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*(bt_deleted_min_excess_));

    bt_deleted_max_excess_ = new sdsl::int_vector<>(deleted_max_excess.size());
    for (int k = 0; k < deleted_max_excess.size(); ++k) {
        (*bt_deleted_max_excess_)[k] = deleted_max_excess[k];
    }
    sdsl::util::bit_compress(*bt_deleted_max_excess_);
    c_bt_deleted_max_excess_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*(bt_deleted_max_excess_));

    bt_deleted_min_count_ = new sdsl::int_vector<>(deleted_min_count.size());
    for (int k = 0; k < deleted_min_count.size(); ++k) {
        (*bt_deleted_min_count_)[k] = deleted_min_count[k];
    }
    sdsl::util::bit_compress(*bt_deleted_min_count_);
    c_bt_deleted_min_count_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*(bt_deleted_min_count_));
*/

    int close = 0;
    for (auto& pair : bt->root_block_->ranks_) {
        if (pair.first != open_) close = pair.first;
    }


    int first_level_prefix_ranks_ = 0;
    sdsl::int_vector<>* first_level_prefix_leaf_ranks = new sdsl::int_vector<>(first_level.size());
    sdsl::int_vector<>* first_level_cumulated_leaf_ranks = new sdsl::int_vector<>(first_level.size());
    sdsl::int_vector<>* first_level_leaf_ranks = new sdsl::int_vector<>(first_level.size());

    sdsl::int_vector<>* first_level_prefix_zeros_ranks = new sdsl::int_vector<>(first_level.size());
    sdsl::int_vector<>* first_level_cumulated_zeros_ranks = new sdsl::int_vector<>(first_level.size());
    sdsl::int_vector<>* first_level_zeros_ranks = new sdsl::int_vector<>(first_level.size());

    //sdsl::int_vector<>* jea15_first_level_prefix_leaf_ranks = new sdsl::int_vector<>(first_level.size()); // JEA15 encoding
    //sdsl::int_vector<>* jea15_first_level_cumulated_leaf_ranks = new sdsl::int_vector<>(first_level.size()); // JEA15 encoding worried till here of the other cases
    sdsl::int_vector<>* jea15_first_level_leaf_ranks = new sdsl::int_vector<>(first_level.size()); // JEA15 encoding

    //ESTO
    //int first_level_prefix_excess = 0;
    //sdsl::int_vector<>* first_level_min_prefix_excess = new sdsl::int_vector<>(first_level.size());
    sdsl::int_vector<>* local_first_level_min_prefix_excess = new sdsl::int_vector<>(first_level.size());

    //sdsl::int_vector<>* first_level_max_prefix_excess = new sdsl::int_vector<>(first_level.size());
    sdsl::int_vector<>* local_first_level_max_prefix_excess = new sdsl::int_vector<>(first_level.size());

    //sdsl::int_vector<>* first_level_min_count = new sdsl::int_vector<>(first_level.size());
    sdsl::int_vector<>* local_first_level_min_count = new sdsl::int_vector<>(first_level.size());


    sdsl::int_vector<>* local_first_level_excess = new sdsl::int_vector<>(first_level.size());


    int level_length = first_level_length_;
    for (int i = 0; i < first_level.size(); ++i) {
        //ESTO
        //(*first_level_min_prefix_excess)[i] = first_level_prefix_excess + first_level[i]->min_prefix_excess_;
        (*local_first_level_min_prefix_excess)[i] = 1- first_level[i]->min_prefix_excess_;

        //(*first_level_max_prefix_excess)[i] = first_level_prefix_excess + first_level[i]->max_prefix_excess_;
        (*local_first_level_max_prefix_excess)[i] = 1+first_level[i]->max_prefix_excess_;

        //(*first_level_max_prefix_excess)[i] = first_level_prefix_excess + first_level[i]->max_prefix_excess_;
        (*local_first_level_min_count)[i] = first_level[i]->prefix_min_count_;

        int excess = 0;
        for (auto pair : first_level[i]->ranks_) {
            if (pair.first == open_) excess += pair.second;
            else excess -= pair.second;
        }
        (*local_first_level_excess)[i] = encoded_excess(excess);
        /*
        for (auto pair : first_level[i]->ranks_) {
            if (pair.first == bt->input_[0]) first_level_prefix_excess += pair.second;
            else first_level_prefix_excess -= pair.second;
        }
        */


        (*first_level_prefix_zeros_ranks)[i] = (*(bt_first_level_prefix_ranks_[close]))[i] - first_level_prefix_ranks_;
        (*first_level_zeros_ranks)[i] = (first_level[i]->ranks_)[close] - first_level[i]->leaf_rank_;
        (*first_level_cumulated_zeros_ranks)[i] = (*(bt_first_level_cumulated_ranks_[close]))[i] - first_level_prefix_ranks_ -  first_level[i]->leaf_rank_;

        (*first_level_prefix_leaf_ranks)[i] = first_level_prefix_ranks_;
        (*first_level_leaf_ranks)[i] = first_level[i]->leaf_rank_;
        first_level_prefix_ranks_ += first_level[i]->leaf_rank_;
        (*first_level_cumulated_leaf_ranks)[i] = first_level_prefix_ranks_;

        // JEA15 encoding
        (*jea15_first_level_leaf_ranks)[i] = level_length - 2*(first_level[i]->leaf_rank_)+1;//- (first_level[i]->min_prefix_excess_);

    }


    sdsl::util::bit_compress(*(local_first_level_excess));
    (bt_excess_).push_back(local_first_level_excess);
    (c_bt_excess_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*local_first_level_excess)));

    //ESTO
    //sdsl::util::bit_compress(*(first_level_min_prefix_excess));
    //bt_first_level_prefix_min_excess_ = first_level_min_prefix_excess;
    //c_bt_first_level_prefix_min_excess_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*(first_level_min_prefix_excess));
    sdsl::util::bit_compress(*(local_first_level_min_prefix_excess));
    (bt_prefix_min_excess_).push_back(local_first_level_min_prefix_excess);
    (c_bt_prefix_min_excess_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*local_first_level_min_prefix_excess)));

    //sdsl::util::bit_compress(*(first_level_max_prefix_excess));
    //bt_first_level_prefix_max_excess_ = first_level_max_prefix_excess;
    //c_bt_first_level_prefix_max_excess_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*(first_level_max_prefix_excess));
    sdsl::util::bit_compress(*(local_first_level_max_prefix_excess));
    (bt_prefix_max_excess_).push_back(local_first_level_max_prefix_excess);
    (c_bt_prefix_max_excess_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*local_first_level_max_prefix_excess)));

    sdsl::util::bit_compress(*(local_first_level_min_count));
    (bt_min_count_).push_back(local_first_level_min_count);
    (c_bt_min_count_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*local_first_level_min_count)));


    sdsl::util::bit_compress(*(first_level_prefix_leaf_ranks));
    bt_first_level_prefix_leaf_ranks_ = first_level_prefix_leaf_ranks;
    c_bt_first_level_prefix_leaf_ranks_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*(first_level_prefix_leaf_ranks));

    sdsl::util::bit_compress(*(first_level_cumulated_leaf_ranks));
    bt_first_level_cumulated_leaf_ranks_ = first_level_cumulated_leaf_ranks;
    c_bt_first_level_cumulated_leaf_ranks_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*(first_level_cumulated_leaf_ranks));

    sdsl::util::bit_compress(*(first_level_leaf_ranks));
    bt_leaf_ranks_.push_back(first_level_leaf_ranks);
    c_bt_leaf_ranks_.push_back( new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*(first_level_leaf_ranks)));


    sdsl::util::bit_compress(*(first_level_prefix_zeros_ranks));
    bt_first_level_prefix_zeros_ranks_ = first_level_prefix_zeros_ranks;
    c_bt_first_level_prefix_zeros_ranks_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*(first_level_prefix_zeros_ranks));

    sdsl::util::bit_compress(*(first_level_cumulated_zeros_ranks));
    bt_first_level_cumulated_zeros_ranks_ = first_level_cumulated_zeros_ranks;
    c_bt_first_level_cumulated_zeros_ranks_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*(first_level_cumulated_zeros_ranks));

    sdsl::util::bit_compress(*(first_level_zeros_ranks));
    bt_zeros_ranks_.push_back(first_level_zeros_ranks);
    c_bt_zeros_ranks_.push_back( new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*(first_level_zeros_ranks)));

    //JEA15 encoding
    sdsl::util::bit_compress(*(jea15_first_level_leaf_ranks));
    bt_jea15_leaf_ranks_.push_back(jea15_first_level_leaf_ranks);
    c_bt_jea15_leaf_ranks_.push_back( new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*(jea15_first_level_leaf_ranks)));


    std::vector<PBlock*> current_level = first_level;
    std::vector<PBlock*> next_level = bt->next_level(first_level);
    level_length = first_level_length_;
    while (next_level.size() != 0) {
        sdsl::bit_vector* current_level_starts_with_end_leaf = new sdsl::bit_vector(current_level.size() ,0);

        sdsl::int_vector<>* next_level_leaf_ranks = new sdsl::int_vector<>(next_level.size());
        sdsl::int_vector<>* next_level_prefix_leaf_ranks = new sdsl::int_vector<>(next_level.size());
        sdsl::int_vector<>* next_level_cumulated_leaf_ranks = new sdsl::int_vector<>(next_level.size());

        sdsl::int_vector<>* next_level_zeros_ranks = new sdsl::int_vector<>(next_level.size());
        sdsl::int_vector<>* next_level_prefix_zeros_ranks = new sdsl::int_vector<>(next_level.size());
        sdsl::int_vector<>* next_level_cumulated_zeros_ranks = new sdsl::int_vector<>(next_level.size());

        sdsl::int_vector<>* jea15_next_level_leaf_ranks = new sdsl::int_vector<>(next_level.size()); //JEA15 encoding

        //ESTO
        sdsl::int_vector<>* next_level_excess = new sdsl::int_vector<>(next_level.size());
        sdsl::int_vector<>* next_level_prefix_min_excess = new sdsl::int_vector<>(next_level.size());
        sdsl::int_vector<>* next_level_prefix_max_excess = new sdsl::int_vector<>(next_level.size());
        sdsl::int_vector<>* next_level_min_count = new sdsl::int_vector<>(next_level.size());

        int number_of_leaves = 0;
        for (int i = 0; i < current_level.size(); ++i) {
            if (current_level[i]->is_leaf()) {
                ++number_of_leaves;
            }
            if (current_level[i]->starts_with_end_leaf_) {
                (* current_level_starts_with_end_leaf)[i] = 1; //HERE! at the end tooo
            } else {
                (* current_level_starts_with_end_leaf)[i] = 0;
            }
        }


        for (int i = 0; i < next_level.size(); ++i) {

            (*next_level_zeros_ranks)[i] = (next_level[i]->ranks_)[close] - next_level[i]->leaf_rank_;
            (*next_level_prefix_zeros_ranks)[i] = (next_level[i]->prefix_ranks_)[close] - next_level[i]->prefix_leaf_rank_;
            (*next_level_cumulated_zeros_ranks)[i] = (next_level[i]->cumulated_ranks_)[close] - next_level[i]->cumulated_leaf_rank_;

            (*next_level_leaf_ranks)[i] = next_level[i]->leaf_rank_;
            (*next_level_prefix_leaf_ranks)[i] = next_level[i]->prefix_leaf_rank_;
            (*next_level_cumulated_leaf_ranks)[i] = next_level[i]->cumulated_leaf_rank_;

            (*jea15_next_level_leaf_ranks)[i] = level_length/r_ - 2*(next_level[i]->leaf_rank_)+1;//- next_level[i]->min_prefix_excess_; // JEA15 encoding

            //ESTO
            (*next_level_prefix_min_excess)[i] = 1-next_level[i]->min_prefix_excess_;
            (*next_level_prefix_max_excess)[i] = 1+next_level[i]->max_prefix_excess_;
            (*next_level_min_count)[i] = next_level[i]->prefix_min_count_;

            int excess = 0;
            for (auto pair : next_level[i]->ranks_) {
                if (pair.first == open_) excess += pair.second;
                else excess -= pair.second;
            }
            (*next_level_excess)[i] = encoded_excess(excess);
        }



        sdsl::bit_vector* current_level_suffix_start_with_end_leaf = new sdsl::bit_vector(number_of_leaves ,0);
        sdsl::bit_vector* current_level_prefix_start_with_end_leaf = new sdsl::bit_vector(number_of_leaves ,0);

        sdsl::bit_vector* current_level_min_in_first_block = new sdsl::bit_vector(number_of_leaves ,0);
        sdsl::bit_vector* current_level_min_in_both_blocks = new sdsl::bit_vector(number_of_leaves ,0);
        sdsl::bit_vector* current_level_max_in_first_block = new sdsl::bit_vector(number_of_leaves ,0);
        sdsl::bit_vector* current_level_max_in_both_blocks = new sdsl::bit_vector(number_of_leaves ,0);
        sdsl::int_vector<>* current_level_min_count_back_block = new sdsl::int_vector<>(number_of_leaves);


        sdsl::int_vector<>* current_level_first_leaf_ranks = new sdsl::int_vector<>(number_of_leaves);
        sdsl::int_vector<>* current_level_second_leaf_ranks = new sdsl::int_vector<>(number_of_leaves);

        sdsl::int_vector<>* current_level_first_zeros_ranks = new sdsl::int_vector<>(number_of_leaves);
        sdsl::int_vector<>* current_level_second_zeros_ranks = new sdsl::int_vector<>(number_of_leaves);

        sdsl::int_vector<>* jea15_current_level_first_leaf_ranks = new sdsl::int_vector<>(number_of_leaves); //JEA15 encoding
        sdsl::int_vector<>* jea15_current_level_second_leaf_ranks = new sdsl::int_vector<>(number_of_leaves); //JEA15 encoding

        // ESTOP
        sdsl::int_vector<>* current_level_min_count_first_block = new sdsl::int_vector<>(number_of_leaves);
        sdsl::int_vector<>* current_level_min_count_second_block = new sdsl::int_vector<>(number_of_leaves);

        sdsl::int_vector<>* current_level_prefix_min_first_block = new sdsl::int_vector<>(number_of_leaves);
        sdsl::int_vector<>* current_level_prefix_min_second_block = new sdsl::int_vector<>(number_of_leaves);

        sdsl::int_vector<>* current_level_prefix_max_first_block = new sdsl::int_vector<>(number_of_leaves);
        sdsl::int_vector<>* current_level_prefix_max_second_block = new sdsl::int_vector<>(number_of_leaves);

        sdsl::int_vector<>* current_level_excess_first_block = new sdsl::int_vector<>(number_of_leaves);
        sdsl::int_vector<>* current_level_excess_second_block = new sdsl::int_vector<>(number_of_leaves);


        int j = 0;
        /*
        int cmin = 0;
        int cmax = 0;
         */
        sdsl::int_vector<>* current_level_prefix_max_excess_back_block = new sdsl::int_vector<>(number_of_leaves);
        sdsl::int_vector<>* current_level_prefix_min_excess_back_block = new sdsl::int_vector<>(number_of_leaves);
        for (int i = 0; i < current_level.size(); ++i) {
            if (dynamic_cast<PBackBlock*>(current_level[i])) {
                (*current_level_first_zeros_ranks)[j] = (current_level[i]->first_ranks_)[close] - current_level[i]->first_leaf_rank_;
                (*current_level_second_zeros_ranks)[j] = (current_level[i]->second_ranks_)[close] - current_level[i]->second_leaf_rank_;

                (*current_level_first_leaf_ranks)[j] = current_level[i]->first_leaf_rank_;
                (*current_level_second_leaf_ranks)[j] = current_level[i]->second_leaf_rank_;


                (*jea15_current_level_first_leaf_ranks)[j] = current_level[i]->offset_ - 2*(current_level[i]->first_leaf_rank_) +1; //JEA15 encoding, I SHOULD NOW THE FIRST RANK MINIMUN
                (*jea15_current_level_second_leaf_ranks)[j] = level_length - (current_level[i]->offset_) - 2*(current_level[i]->second_leaf_rank_) +1;//- dynamic_cast<PBackBlock*>(current_level[i])->prefix_first_block_min_excess_; //JEA15 encoding

                PBackBlock* bb  = dynamic_cast<PBackBlock*>(current_level[i]);
                if (bb->suffix_start_with_end_leaf_) {
                    (*current_level_suffix_start_with_end_leaf)[j] = 1;
                } else {
                    (*current_level_suffix_start_with_end_leaf)[j] = 0;
                }
                if (bb->prefix_start_with_end_leaf_) {
                    (*current_level_prefix_start_with_end_leaf)[j] = 1;
                } else {
                    (*current_level_prefix_start_with_end_leaf)[j] = 0;
                }
                if (bb->min_in_first_block_) {
                    (*current_level_min_in_first_block)[j] = 1;
                } else {
                    (*current_level_min_in_first_block)[j] = 0;
                }
                if (bb->min_in_both_blocks_) {
                    (*current_level_min_in_both_blocks)[j] = 1;
                } else {
                    (*current_level_min_in_both_blocks)[j] = 0;
                }
                if (bb->max_in_first_block_) {
                    (*current_level_max_in_first_block)[j] = 1;
                } else {
                    (*current_level_max_in_first_block)[j] = 0;
                }
                if (bb->max_in_both_blocks_) {
                    (*current_level_max_in_both_blocks)[j] = 1;
                } else {
                    (*current_level_max_in_both_blocks)[j] = 0;
                }

                if (! bb->min_in_both_blocks_  && ! bb->min_in_first_block_) {
                    (*current_level_min_count_back_block)[j] = bb->prefix_min_first_block_count_;
                } else {
                    (*current_level_min_count_back_block)[j] = bb->prefix_min_second_block_count_;
                }

                if (! bb->min_in_both_blocks_) {
                    if (bb->min_in_first_block_) {
                        (*current_level_prefix_min_excess_back_block)[j] = 1-(bb->prefix_second_block_min_excess_);
                    } else {
                        (*current_level_prefix_min_excess_back_block)[j] = 1-(bb->prefix_first_block_min_excess_);
                    }
                } else {
                    (*current_level_prefix_min_excess_back_block)[j] = 1-(bb->prefix_second_block_min_excess_);
                }
                if (! bb->max_in_both_blocks_) {
                    if (bb->max_in_first_block_) {
                        (*current_level_prefix_max_excess_back_block)[j] = 1+(bb->prefix_second_block_max_excess_);
                    } else {
                        (*current_level_prefix_max_excess_back_block)[j] = 1 + (bb->prefix_first_block_max_excess_);
                    }
                } else {
                    (*current_level_prefix_max_excess_back_block)[j] = 1+(bb->prefix_second_block_max_excess_);
                }
/*
                if (! bb->min_in_both_blocks_) {
                    ++cmin;
                }
                if (! bb->max_in_both_blocks_) {
                    ++cmax;
                }
*/
                // ESTOP
                (*current_level_min_count_first_block)[j] = bb->prefix_min_first_block_count_;
                (*current_level_min_count_second_block)[j] = bb->prefix_min_second_block_count_;

                (*current_level_prefix_min_first_block)[j] = 1-(bb->prefix_first_block_min_excess_);
                (*current_level_prefix_min_second_block)[j] = 1-(bb->prefix_second_block_min_excess_);

                (*current_level_prefix_max_first_block)[j] = 1+(bb->prefix_first_block_max_excess_);
                (*current_level_prefix_max_second_block)[j] = 1+(bb->prefix_second_block_max_excess_);


                int first_block_excess = 0;
                for (auto pair : bb->second_ranks_) {
                    if (pair.first == open_) first_block_excess += pair.second;
                    else first_block_excess -= pair.second;
                }
                (*current_level_excess_first_block)[j] = encoded_excess(first_block_excess);

                int second_block_excess = 0;
                for (auto pair : bb->ranks_) {
                    if (pair.first == open_) second_block_excess += pair.second;
                    else second_block_excess -= pair.second;
                }
                for (auto pair : bb->second_ranks_) {
                    if (pair.first == open_) second_block_excess -= pair.second;
                    else second_block_excess += pair.second;
                }
                (*current_level_excess_second_block)[j] = encoded_excess(second_block_excess);

                ++j;
            }
        }




        // ESTOP
        sdsl::util::bit_compress(*(current_level_min_count_first_block));
        (bt_min_count_first_block_).push_back(current_level_min_count_first_block);
        (c_bt_min_count_first_block_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*current_level_min_count_first_block)));
        sdsl::util::bit_compress(*(current_level_min_count_second_block));
        (bt_min_count_second_block_).push_back(current_level_min_count_second_block);
        (c_bt_min_count_second_block_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*current_level_min_count_second_block)));


        sdsl::util::bit_compress(*(current_level_excess_first_block));
        (bt_excess_first_block_).push_back(current_level_excess_first_block);
        (c_bt_excess_first_block_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*current_level_excess_first_block)));
        sdsl::util::bit_compress(*(current_level_excess_second_block));
        (bt_excess_second_block_).push_back(current_level_excess_second_block);
        (c_bt_excess_second_block_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*current_level_excess_second_block)));

        sdsl::util::bit_compress(*(current_level_prefix_min_first_block));
        (bt_prefix_min_excess_first_block_).push_back(current_level_prefix_min_first_block);
        (c_bt_prefix_min_excess_first_block_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*current_level_prefix_min_first_block)));
        sdsl::util::bit_compress(*(current_level_prefix_min_second_block));
        (bt_prefix_min_excess_second_block_).push_back(current_level_prefix_min_second_block);
        (c_bt_prefix_min_excess_second_block_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*current_level_prefix_min_second_block)));


        sdsl::util::bit_compress(*(current_level_prefix_max_first_block));
        (bt_prefix_max_excess_first_block_).push_back(current_level_prefix_max_first_block);
        (c_bt_prefix_max_excess_first_block_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*current_level_prefix_max_first_block)));
        sdsl::util::bit_compress(*(current_level_prefix_max_second_block));
        (bt_prefix_max_excess_second_block_).push_back(current_level_prefix_max_second_block);
        (c_bt_prefix_max_excess_second_block_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*current_level_prefix_max_second_block)));



        sdsl::util::bit_compress(*(next_level_leaf_ranks));
        (bt_leaf_ranks_).push_back(next_level_leaf_ranks);
        (c_bt_leaf_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*next_level_leaf_ranks)));


        sdsl::util::bit_compress(*(next_level_prefix_leaf_ranks));
        (bt_prefix_leaf_ranks_).push_back(next_level_prefix_leaf_ranks);
        (c_bt_prefix_leaf_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*next_level_prefix_leaf_ranks)));


        sdsl::util::bit_compress(*(next_level_cumulated_leaf_ranks));
        (bt_cumulated_leaf_ranks_).push_back(next_level_cumulated_leaf_ranks);
        (c_bt_cumulated_leaf_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*next_level_cumulated_leaf_ranks)));

        sdsl::util::bit_compress(*(next_level_zeros_ranks));
        (bt_zeros_ranks_).push_back(next_level_zeros_ranks);
        (c_bt_zeros_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*next_level_zeros_ranks)));


        sdsl::util::bit_compress(*(next_level_prefix_zeros_ranks));
        (bt_prefix_zeros_ranks_).push_back(next_level_prefix_zeros_ranks);
        (c_bt_prefix_zeros_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*next_level_prefix_zeros_ranks)));


        sdsl::util::bit_compress(*(next_level_cumulated_zeros_ranks));
        (bt_cumulated_zeros_ranks_).push_back(next_level_cumulated_zeros_ranks);
        (c_bt_cumulated_zeros_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*next_level_cumulated_zeros_ranks)));


        //JEA15 encoding
        sdsl::util::bit_compress(*(jea15_next_level_leaf_ranks));
        (bt_jea15_leaf_ranks_).push_back(jea15_next_level_leaf_ranks);
        (c_bt_jea15_leaf_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*jea15_next_level_leaf_ranks)));


        //ESTO
        sdsl::util::bit_compress(*(next_level_excess));
        (bt_excess_).push_back(next_level_excess);
        (c_bt_excess_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*next_level_excess)));

        sdsl::util::bit_compress(*(next_level_prefix_min_excess));
        (bt_prefix_min_excess_).push_back(next_level_prefix_min_excess);
        (c_bt_prefix_min_excess_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*next_level_prefix_min_excess)));

        sdsl::util::bit_compress(*(next_level_prefix_max_excess));
        (bt_prefix_max_excess_).push_back(next_level_prefix_max_excess);
        (c_bt_prefix_max_excess_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*next_level_prefix_max_excess)));

        sdsl::util::bit_compress(*(next_level_min_count));
        (bt_min_count_).push_back(next_level_min_count);
        (c_bt_min_count_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*next_level_min_count)));


        sdsl::util::bit_compress(*(current_level_first_leaf_ranks));
        (bt_first_leaf_ranks_).push_back(current_level_first_leaf_ranks);
        (c_bt_first_leaf_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*current_level_first_leaf_ranks)));


        sdsl::util::bit_compress(*(current_level_second_leaf_ranks));
        (bt_second_leaf_ranks_).push_back(current_level_second_leaf_ranks);
        (c_bt_second_leaf_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*current_level_second_leaf_ranks)));


        sdsl::util::bit_compress(*(current_level_first_zeros_ranks));
        (bt_first_zeros_ranks_).push_back(current_level_first_zeros_ranks);
        (c_bt_first_zeros_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*current_level_first_zeros_ranks)));


        sdsl::util::bit_compress(*(current_level_second_zeros_ranks));
        (bt_second_zeros_ranks_).push_back(current_level_second_zeros_ranks);
        (c_bt_second_zeros_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*current_level_second_zeros_ranks)));

        // JEA15 encoding
        sdsl::util::bit_compress(*(jea15_current_level_first_leaf_ranks));
        (bt_jea15_first_leaf_ranks_).push_back(jea15_current_level_first_leaf_ranks);
        (c_bt_jea15_first_leaf_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*jea15_current_level_first_leaf_ranks)));

        // JEA15 encoding
        sdsl::util::bit_compress(*(jea15_current_level_second_leaf_ranks));
        (bt_jea15_second_leaf_ranks_).push_back(jea15_current_level_second_leaf_ranks);
        (c_bt_jea15_second_leaf_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*jea15_current_level_second_leaf_ranks)));




        bt_starts_with_end_leaf_.push_back(current_level_starts_with_end_leaf);
        c_bt_starts_with_end_leaf_.push_back(new sdsl::rrr_vector<63>(*bt_starts_with_end_leaf_.back()));

        bt_suffix_start_with_end_leaf_.push_back(current_level_suffix_start_with_end_leaf);
        c_bt_suffix_start_with_end_leaf_.push_back(new sdsl::rrr_vector<63>(*bt_suffix_start_with_end_leaf_.back()));

        bt_prefix_start_with_end_leaf_.push_back(current_level_prefix_start_with_end_leaf);
        c_bt_prefix_start_with_end_leaf_.push_back(new sdsl::rrr_vector<63>(*bt_prefix_start_with_end_leaf_.back()));


        bt_min_in_first_block_.push_back(current_level_min_in_first_block);
        c_bt_min_in_first_block_.push_back(new sdsl::rrr_vector<63>(*current_level_min_in_first_block));
        bt_min_in_both_blocks_.push_back(current_level_min_in_both_blocks);
        c_bt_min_in_both_blocks_.push_back(new sdsl::rrr_vector<63>(*current_level_min_in_both_blocks));

        bt_min_in_both_blocks_rank_.push_back(new sdsl::rank_support_v<1>(current_level_min_in_both_blocks));
        c_bt_min_in_both_blocks_rank_.push_back(new sdsl::rrr_vector<63>::rank_1_type(c_bt_min_in_both_blocks_.back()));



        bt_max_in_first_block_.push_back(current_level_max_in_first_block);
        c_bt_max_in_first_block_.push_back(new sdsl::rrr_vector<63>(*current_level_max_in_first_block));
        bt_max_in_both_blocks_.push_back(current_level_max_in_both_blocks);
        c_bt_max_in_both_blocks_.push_back(new sdsl::rrr_vector<63>(*current_level_max_in_both_blocks));

        bt_max_in_both_blocks_rank_.push_back(new sdsl::rank_support_v<1>(current_level_max_in_both_blocks));
        c_bt_max_in_both_blocks_rank_.push_back(new sdsl::rrr_vector<63>::rank_1_type(c_bt_max_in_both_blocks_.back()));

        sdsl::util::bit_compress(*current_level_min_count_back_block);
        bt_min_count_back_block_.push_back(current_level_min_count_back_block);
        c_bt_min_count_back_block_.push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*current_level_min_count_back_block));


/*
        sdsl::int_vector<>* current_level_prefix_max_excess_back_block = new sdsl::int_vector<>(cmax);
        sdsl::int_vector<>* current_level_prefix_min_excess_back_block = new sdsl::int_vector<>(cmin);
        int imin = 0;
        int imax = 0;
        for (int i = 0; i < current_level.size(); ++i) {
            if (dynamic_cast<PBackBlock*>(current_level[i])) {
                PBackBlock* bb  = dynamic_cast<PBackBlock*>(current_level[i]);
                if (! bb->min_in_both_blocks_) {
                    if (bb->min_in_first_block_) {
                        (*current_level_prefix_min_excess_back_block)[imin] = 1-(bb->prefix_second_block_min_excess_);
                    } else {
                        (*current_level_prefix_min_excess_back_block)[imin] = 1-(bb->prefix_first_block_min_excess_);
                    }
                    ++imin;
                }
                if (! bb->max_in_both_blocks_) {
                    if (bb->max_in_first_block_) {
                        (*current_level_prefix_max_excess_back_block)[imax] = 1+(bb->prefix_second_block_max_excess_);
                    } else {
                        (*current_level_prefix_max_excess_back_block)[imax] = 1+(bb->prefix_first_block_max_excess_);
                    }
                    ++imax;
                }

                ///COMMENT THIS
                (*current_level_prefix_min_first_block)[j] = 1-(bb->prefix_first_block_min_excess_);
                (*current_level_prefix_min_second_block)[j] = 1-(bb->prefix_second_block_min_excess_);

                (*current_level_prefix_max_first_block)[j] = 1+(bb->prefix_first_block_max_excess_);
                (*current_level_prefix_max_second_block)[j] = 1+(bb->prefix_second_block_max_excess_);
                //COMMENT THIS
            }
        }
*/
        sdsl::util::bit_compress(*current_level_prefix_min_excess_back_block);
        bt_prefix_min_excess_back_block_.push_back(current_level_prefix_min_excess_back_block);
        c_bt_prefix_min_excess_back_block_.push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*current_level_prefix_min_excess_back_block));

        sdsl::util::bit_compress(*current_level_prefix_max_excess_back_block);
        bt_prefix_max_excess_back_block_.push_back(current_level_prefix_max_excess_back_block);
        c_bt_prefix_max_excess_back_block_.push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*current_level_prefix_max_excess_back_block));

        current_level = next_level;
        next_level = bt->next_level(current_level);
        level_length /= r_;
    }


    std::vector<PBlock*> last_level = current_level;
    sdsl::bit_vector* last_level_starts_with_end_leaf = new sdsl::bit_vector(last_level.size() ,0);

    for (int i = 0; i < last_level.size(); ++i) {
        if (last_level[i]->starts_with_end_leaf_) {
            (*last_level_starts_with_end_leaf)[i] = 1;
        } else {
            (*last_level_starts_with_end_leaf)[i] = 0;
        }
    }

    bt_starts_with_end_leaf_.push_back(last_level_starts_with_end_leaf);
    c_bt_starts_with_end_leaf_.push_back(new sdsl::rrr_vector<63>(*bt_starts_with_end_leaf_.back()));


    int n = number_of_blocks_first_level_;
    int i = 0;
    int r_power_i = 1;
    while (n/r_ > 0) {
        r_power_i *= r_;
        n /= r_;
        ++i;
    }

    int p = (number_of_blocks_first_level_-r_power_i)/(r_-1);
    int m = (number_of_blocks_first_level_-r_power_i)% (r_-1);
    if (m>0) ++p;

    int n_pre_last_level = r_power_i - p;
    int n_last_level = number_of_blocks_first_level_+p-r_power_i;
    n_last_level_ = n_last_level;
    n_pre_last_level_ = n_pre_last_level;
    int nodes = n_last_level + (r_power_i*r_-1)/(r_-1);
    n_internal_nodes_ = nodes-n_last_level-n_pre_last_level;

    int* top_excess = new int[nodes];
    int* top_min_excess = new int[nodes];
    int* top_max_excess = new int[nodes];
    int* top_min_count = new int[nodes];

    for (int i = 0; i < n_last_level; ++i) {
        top_excess[nodes-n_last_level+i] = 0;
        for (auto pair : bt_ranks_) {
            if (pair.first == open_) {
                top_excess[nodes-n_last_level+i] += (*pair.second[0])[i];
            } else {
                top_excess[nodes-n_last_level+i] -= (*pair.second[0])[i];
            }
        }
        top_min_excess[nodes-n_last_level+i] = 1-(*bt_prefix_min_excess_[0])[i];
        top_max_excess[nodes-n_last_level+i] = -1+(*bt_prefix_max_excess_[0])[i];
        top_min_count[nodes-n_last_level+i] = (*bt_min_count_[0])[i];
    }

    for (int i = 0; i < n_pre_last_level; ++i) {
        top_excess[nodes-n_last_level-n_pre_last_level+i] = 0;
        for (auto pair : bt_ranks_) {
            if (pair.first == open_) {
                top_excess[nodes-n_last_level-n_pre_last_level+i] += (*pair.second[0])[n_last_level+i];
            } else {
                top_excess[nodes-n_last_level-n_pre_last_level+i] -= (*pair.second[0])[n_last_level+i];
            }
        }
        top_min_excess[nodes-n_last_level-n_pre_last_level+i] = 1-(*bt_prefix_min_excess_[0])[n_last_level+i];
        top_max_excess[nodes-n_last_level-n_pre_last_level+i] = -1+(*bt_prefix_max_excess_[0])[n_last_level+i];
        top_min_count[nodes-n_last_level-n_pre_last_level+i] = (*bt_min_count_[0])[n_last_level+i];
    }

    for (int i = nodes-n_last_level-n_pre_last_level-1; i >=0; --i) {
        top_excess[i] = 0;
        int min_excess = 1;
        int max_excess = -1;
        int min_count = 0;
        for (int j = 0; j < r_; ++j) {
            if (i*r_+j+1 < nodes) {
                if (top_excess[i] + top_min_excess[i*r_+j+1] < min_excess) {
                    min_excess = top_excess[i] + top_min_excess[i*r_+j+1];
                    min_count = top_min_count[i*r_+j+1];
                } else if (top_excess[i] + top_min_excess[i*r_+j+1] == min_excess) {
                    min_count += top_min_count[i*r_+j+1];
                }
                if (top_excess[i] + top_max_excess[i*r_+j+1] > max_excess) {
                    max_excess = top_excess[i] + top_max_excess[i*r_+j+1];
                }
                top_excess[i] +=  top_excess[i*r_+j+1];
            }
        }
        top_min_excess[i] = min_excess;
        top_max_excess[i] = max_excess;
        top_min_count[i] = min_count;
    }

    top_excess_ = new sdsl::int_vector<>(nodes-n_last_level-n_pre_last_level);
    top_min_excess_ = new sdsl::int_vector<>(nodes-n_last_level-n_pre_last_level);
    top_max_excess_ = new sdsl::int_vector<>(nodes-n_last_level-n_pre_last_level);
    top_min_count_ = new sdsl::int_vector<>(nodes-n_last_level-n_pre_last_level);
    for (int i = 0; i < nodes-n_last_level-n_pre_last_level; ++i) {
        (*top_excess_)[i] = encoded_excess(top_excess[i]);
        (*top_min_excess_)[i] = 1-top_min_excess[i];
        (*top_max_excess_)[i] = 1+top_max_excess[i];
        (*top_min_count_)[i] = top_min_count[i];
    }

    c_top_excess_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*top_excess_);
    c_top_min_excess_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*top_min_excess_);
    c_top_max_excess_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*top_max_excess_);
    c_top_min_count_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*top_min_count_);


    for (sdsl::bit_vector* bv_level : bt_bv_) {
        bt_bv_select_1_.push_back(new sdsl::bit_vector::select_1_type(bv_level));
    }
    for (sdsl::rrr_vector<63>* c_bv_level : c_bt_bv_) {
        c_bt_bv_select_1_.push_back(new sdsl::rrr_vector<63>::select_1_type(c_bv_level));
    }


    delete[] top_excess;
    delete[] top_min_excess;
    delete[] top_max_excess;
    delete[] top_min_count;

    delete bt_first_level_prefix_ranks_[close];
    bt_first_level_prefix_ranks_.erase(close);
    delete bt_first_level_cumulated_ranks_[close];
    bt_first_level_cumulated_ranks_.erase(close);
    for (auto vec : bt_prefix_ranks_[close])
        delete vec;
    bt_prefix_ranks_.erase(close);
    for (auto vec : bt_cumulated_ranks_[close])
        delete vec;
    bt_cumulated_ranks_.erase(close);
    for (auto vec : bt_ranks_[close])
        delete vec;
    bt_ranks_.erase(close);
    for (auto vec : bt_first_ranks_[close])
        delete vec;
    bt_first_ranks_.erase(close);
    for (auto vec : bt_second_ranks_[close])
        delete vec;
    bt_second_ranks_.erase(close);

    delete c_bt_first_level_prefix_ranks_[close];
    c_bt_first_level_prefix_ranks_.erase(close);
    delete c_bt_first_level_cumulated_ranks_[close];
    c_bt_first_level_cumulated_ranks_.erase(close);
    for (auto vec : c_bt_prefix_ranks_[close])
        delete vec;
    c_bt_prefix_ranks_.erase(close);
    for (auto vec : c_bt_cumulated_ranks_[close])
        delete vec;
    c_bt_cumulated_ranks_.erase(close);
    for (auto vec : c_bt_ranks_[close])
        delete vec;
    c_bt_ranks_.erase(close);
    for (auto vec : c_bt_first_ranks_[close])
        delete vec;
    c_bt_first_ranks_.erase(close);
    for (auto vec : c_bt_second_ranks_[close])
        delete vec;
    c_bt_second_ranks_.erase(close);


    //HARCODED in little-endian
    bottom_bit_size_ = 16;
    bottom_excess_ = new sdsl::int_vector<>(1 << bottom_bit_size_);
    bottom_min_excess_ = new sdsl::int_vector<>(1 << bottom_bit_size_);
    bottom_max_excess_ = new sdsl::int_vector<>(1 << bottom_bit_size_);
    bottom_min_count_ = new sdsl::int_vector<>(1 << bottom_bit_size_);
    for (unsigned int i = 0; i < (1 << bottom_bit_size_); ++i) {
        int min_excess = 1;
        int max_excess = -1;
        int excess = 0;
        int min_count = 0;
        int num = i;
        for (int j = 0; j < bottom_bit_size_;++j) {
            excess += ((i>>j)%2)?1:-1;
            if (excess == min_excess) ++min_count;
            if (excess < min_excess) {
                min_count = 1;
                min_excess = excess;
            }
            if (excess > max_excess) max_excess = excess;
        }
        bottom_excess_2_.push_back(excess);
        bottom_min_excess_2_.push_back(min_excess);
        bottom_max_excess_2_.push_back(max_excess);
        bottom_min_count_2_.push_back(min_count);
        (*bottom_excess_)[i] = encoded_excess(excess);
        (*bottom_min_excess_)[i] = 1-min_excess;
        (*bottom_max_excess_)[i] = 1+max_excess;
        (*bottom_min_count_)[i] = min_count;
    }

    c_bottom_excess_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*bottom_excess_);
    c_bottom_min_excess_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*bottom_min_excess_);
    c_bottom_max_excess_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*bottom_max_excess_);
    c_bottom_min_count_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*bottom_min_count_);
    //HARCODED



}

BPSPCBlockTree::~BPSPCBlockTree() {

    for (sdsl::bit_vector* bv : bt_min_in_first_block_) {
        delete bv;
    }
    for (sdsl::bit_vector* bv : bt_min_in_both_blocks_) {
        delete bv;
    }
    for (sdsl::bit_vector* bv : bt_max_in_first_block_) {
        delete bv;
    }
    for (sdsl::bit_vector* bv : bt_max_in_both_blocks_) {
        delete bv;
    }
    for (sdsl::rank_support_v<1>* r : bt_min_in_both_blocks_rank_) {
        delete r;
    }
    for (sdsl::rank_support_v<1>* r : bt_max_in_both_blocks_rank_) {
        delete r;
    }
    for (sdsl::int_vector<>* vector : bt_prefix_min_excess_back_block_) {
        delete vector;
    }
    for (sdsl::int_vector<>* vector : bt_prefix_max_excess_back_block_) {
        delete vector;
    }
    for (sdsl::int_vector<>* vector : bt_min_count_back_block_) {
        delete vector;
    }




    for (sdsl::bit_vector::select_1_type* select : bt_bv_select_1_)
        delete select;
    for (sdsl::rrr_vector<63>::select_1_type* select : c_bt_bv_select_1_)
        delete select;


    delete bt_first_level_prefix_leaf_ranks_;
    delete bt_first_level_cumulated_leaf_ranks_;

    delete bt_first_level_prefix_zeros_ranks_;
    delete bt_first_level_cumulated_zeros_ranks_;

    //delete bt_jea15_first_level_prefix_leaf_ranks_; //Jea15 encoding
    //delete bt_jea15_first_level_cumulated_leaf_ranks_; // Jea15 encoding

    //ESTO
    delete top_excess_;
    delete top_min_excess_;
    delete top_max_excess_;
    delete top_min_count_;

    delete bottom_excess_;//HARCODED
    delete bottom_min_excess_;//HARCODED
    delete bottom_max_excess_;//HARCODED
    delete bottom_min_count_;//HARCODED
    delete c_bottom_excess_;//HARCODED
    delete c_bottom_min_excess_;//HARCODED
    delete c_bottom_max_excess_;//HARCODED
    delete c_bottom_min_count_;//HARCODED
    /*
    delete bt_deleted_excess_;
    delete bt_deleted_min_excess_;
    delete bt_deleted_max_excess_;
    delete bt_deleted_min_count_;
*/

    for (auto vector : bt_excess_)
        delete vector;
    for (auto vector : bt_excess_first_block_)
        delete vector;
    for (auto vector : bt_excess_second_block_)
        delete vector;

    //delete bt_first_level_prefix_min_excess_;
    for (auto vector : bt_prefix_min_excess_)
        delete vector;
    for (auto vector : bt_prefix_min_excess_first_block_)
        delete vector;
    for (auto vector : bt_prefix_min_excess_second_block_)
        delete vector;

    //delete bt_first_level_prefix_max_excess_;
    for (auto vector : bt_prefix_max_excess_)
        delete vector;
    for (auto vector : bt_prefix_max_excess_first_block_)
        delete vector;
    for (auto vector : bt_prefix_max_excess_second_block_)
        delete vector;

    for (auto vector : bt_min_count_)
        delete vector;
    for (auto vector : bt_min_count_first_block_)
        delete vector;
    for (auto vector : bt_min_count_second_block_)
        delete vector;


    for (auto vector : bt_prefix_zeros_ranks_)
        delete vector;
    for (auto vector : bt_cumulated_zeros_ranks_)
        delete vector;
    for (auto vector : bt_zeros_ranks_)
        delete vector;
    for (auto vector : bt_first_zeros_ranks_)
        delete vector;
    for (auto vector : bt_second_zeros_ranks_)
        delete vector;

    for (auto vector : bt_prefix_leaf_ranks_)
        delete vector;
    for (auto vector : bt_cumulated_leaf_ranks_)
        delete vector;
    for (auto vector : bt_leaf_ranks_)
        delete vector;
    for (auto vector : bt_first_leaf_ranks_)
        delete vector;
    for (auto vector : bt_second_leaf_ranks_)
        delete vector;
    for (auto vector : bt_starts_with_end_leaf_)
        delete vector;
    for (auto vector : bt_suffix_start_with_end_leaf_)
        delete vector;
    for (auto vector : bt_prefix_start_with_end_leaf_)
        delete vector;

    /*
    for (auto vector : bt_jea15_prefix_leaf_ranks_) //Jea15 encoding
        delete vector;
    for (auto vector : bt_jea15_cumulated_leaf_ranks_) //Jea15 encoding
        delete vector;
        */
    for (auto vector : bt_jea15_leaf_ranks_) //Jea15 encoding
        delete vector;
    for (auto vector : bt_jea15_first_leaf_ranks_) //Jea15 encoding
        delete vector;
    for (auto vector : bt_jea15_second_leaf_ranks_) //Jea15 encoding
        delete vector;





    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector : c_bt_prefix_min_excess_back_block_) {
        delete vector;
    }
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector : c_bt_prefix_max_excess_back_block_) {
        delete vector;
    }
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector : c_bt_min_count_back_block_) {
        delete vector;
    }
    for (sdsl::rrr_vector<63>* bv : c_bt_min_in_first_block_) {
        delete bv;
    }
    for (sdsl::rrr_vector<63>* bv : c_bt_min_in_both_blocks_) {
        delete bv;
    }
    for (sdsl::rrr_vector<63>* bv : c_bt_max_in_first_block_) {
        delete bv;
    }
    for (sdsl::rrr_vector<63>* bv : c_bt_max_in_both_blocks_) {
        delete bv;
    }
    for (sdsl::rrr_vector<63>::rank_1_type* r : c_bt_min_in_both_blocks_rank_) {
        delete r;
    }
    for (sdsl::rrr_vector<63>::rank_1_type* r : c_bt_max_in_both_blocks_rank_) {
        delete r;
    }

    delete c_bt_first_level_prefix_zeros_ranks_;
    delete c_bt_first_level_cumulated_zeros_ranks_;

    delete c_bt_first_level_prefix_leaf_ranks_;
    delete c_bt_first_level_cumulated_leaf_ranks_;

    //delete c_bt_jea15_first_level_prefix_leaf_ranks_; //Jea15 encoding
    //delete c_bt_jea15_first_level_cumulated_leaf_ranks_; // Jea15 encoding


    //ESTO
    delete c_top_excess_;
    delete c_top_min_excess_;
    delete c_top_max_excess_;
    delete c_top_min_count_;
    /*
    delete c_bt_deleted_excess_;
    delete c_bt_deleted_min_excess_;
    delete c_bt_deleted_max_excess_;
    delete c_bt_deleted_min_count_;
*/

    //delete c_bt_first_level_prefix_min_excess_;

    for (auto vector : c_bt_excess_)
        delete vector;
    for (auto vector : c_bt_excess_first_block_)
        delete vector;
    for (auto vector : c_bt_excess_second_block_)
        delete vector;

    for (auto vector : c_bt_prefix_min_excess_)
        delete vector;
    for (auto vector : c_bt_prefix_min_excess_first_block_)
        delete vector;
    for (auto vector : c_bt_prefix_min_excess_second_block_)
        delete vector;

    //delete c_bt_first_level_prefix_max_excess_;
    for (auto vector : c_bt_prefix_max_excess_)
        delete vector;
    for (auto vector : c_bt_prefix_max_excess_first_block_)
        delete vector;
    for (auto vector : c_bt_prefix_max_excess_second_block_)
        delete vector;

    for (auto vector : c_bt_min_count_)
        delete vector;
    for (auto vector : c_bt_min_count_first_block_)
        delete vector;
    for (auto vector : c_bt_min_count_second_block_)
        delete vector;

    for (auto vector : c_bt_prefix_zeros_ranks_)
        delete vector;
    for (auto vector : c_bt_cumulated_zeros_ranks_)
        delete vector;
    for (auto vector : c_bt_zeros_ranks_)
        delete vector;
    for (auto vector : c_bt_first_zeros_ranks_)
        delete vector;
    for (auto vector : c_bt_second_zeros_ranks_)
        delete vector;

    for (auto vector : c_bt_prefix_leaf_ranks_)
        delete vector;
    for (auto vector : c_bt_cumulated_leaf_ranks_)
        delete vector;
    for (auto vector : c_bt_leaf_ranks_)
        delete vector;
    for (auto vector : c_bt_first_leaf_ranks_)
        delete vector;
    for (auto vector : c_bt_second_leaf_ranks_)
        delete vector;
    for (auto vector : c_bt_starts_with_end_leaf_)
        delete vector;
    for (auto vector : c_bt_suffix_start_with_end_leaf_)
        delete vector;
    for (auto vector : c_bt_prefix_start_with_end_leaf_)
        delete vector;

/*
    for (auto vector : c_bt_jea15_prefix_leaf_ranks_) // Jea15 encoding
        delete vector;
    for (auto vector : c_bt_jea15_cumulated_leaf_ranks_) // Jea15 encoding
        delete vector;
        */
    for (auto vector : c_bt_jea15_leaf_ranks_) // Jea15 encoding
        delete vector;
    for (auto vector : c_bt_jea15_first_leaf_ranks_) // Jea15 encoding
        delete vector;
    for (auto vector : c_bt_jea15_second_leaf_ranks_) // Jea15 encoding
        delete vector;

}

int BPSPCBlockTree::excess(int i) {
    return 2*rank(open_, i) - i - 1;
}

int BPSPCBlockTree::excess_alternative(int i) {
    return 2*rank_alternative(open_, i) - i - 1;
}

int BPSPCBlockTree::excess_alternative_noback(int i) {
    return 2*noback_rank_alternative(open_, i) - i - 1;
}

int BPSPCBlockTree::better_excess(int i) {
    return 2*better_rank(open_, i) - i - 1;
}


int BPSPCBlockTree::rank_1(int i) {
    return rank(open_, i);
}

int BPSPCBlockTree::rank_0(int i) {
    return i-rank(open_, i)+1;
}

int BPSPCBlockTree::rank_1_alternative(int i) {
    return rank_alternative(open_, i);
}

int BPSPCBlockTree::rank_1_alternative_noback(int i) {
    return noback_rank_alternative(open_, i);
}

int BPSPCBlockTree::rank_0_alternative(int i) {
    return i-rank_alternative(open_, i)+1;
}

int BPSPCBlockTree::rank_0_alternative_noback(int i) {
    return i-noback_rank_alternative(open_, i)+1;
}

int BPSPCBlockTree::better_rank_1(int i) {
    return better_rank(open_, i);
}

int BPSPCBlockTree::better_rank_0(int i) {
    return i-better_rank(open_, i)+1;
}

int BPSPCBlockTree::select_1(int j) {
    return select(open_, j);
}

int BPSPCBlockTree::select_1_alternative(int j) {
    return select_alternative(open_, j);
}

int BPSPCBlockTree::select_1_alternative_noback(int j) {
    return noback_select_alternative(open_, j);
}

int BPSPCBlockTree::better_select_1(int j) {
    return better_select(open_, j);
}

int BPSPCBlockTree::select_0(int k) {

    auto& prefix_ranks = bt_prefix_ranks_[open_];
    auto& second_ranks = bt_second_ranks_[open_];
    auto& first_ranks = bt_first_ranks_[open_];
    auto& first_level_prefix_ranks = bt_first_level_prefix_ranks_[open_];

    int current_block = (k-1)/first_level_length_;

    int end_block = number_of_blocks_first_level_-1;
    while (current_block != end_block) {
        int m = current_block + (end_block-current_block)/2;
        int f = m*first_level_length_ - (*first_level_prefix_ranks)[m];
        if (f < k) {
            if (end_block - current_block == 1) {
                if (((m+1)*first_level_length_ - (*first_level_prefix_ranks)[m+1]) < k) {
                    current_block = m+1;
                }
                break;
            }
            current_block = m;
        } else {
            end_block = m-1;
        }
    }
    /* // Change for first_level_prefix_ranks
    while (initial_block < bt_bv_[0]->size()-1 && k > (*bt_prefix_ranks_[c][0])[initial_block+1]) {
        ++initial_block;
    }
     */

    int s = current_block*first_level_length_;
    k -= s - (*first_level_prefix_ranks)[current_block];
    int current_length = first_level_length_;
    int level = 0;
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            int child = firstChild;
            int child_length = current_length/r_;
            /* // Fix this to counter - 1, because of firstlevelprefixranks_ , and factorized map
            int first = firstChild;
            int last = firstChild + r_ - 1;

            while (true) {
                int m = first + (last-first)/2;
                if (m == firstChild+r_ - 1) {
                    child = m;
                    break;
                }
                int f = (*bt_prefix_ranks_[c][counter+1])[m];
                int s = (*bt_prefix_ranks_[c][counter+1])[m+1];
                if (f<k && s >= k) {
                    child = m;
                    break;
                }

                if (f >= k) {
                    last = m-1;
                } else {
                    first = m+1;
                }
            }
            */
            /*
            int end_child = firstChild + r_-1;
            while (child != end_child) {
                int m = child + (end_child-child)/2;
                int f = (*prefix_ranks[counter+1-1])[m];
                if (f < k) {
                    if (end_child - child == 1) {
                        if ((*prefix_ranks[counter+1-1])[m+1] < k) {
                            child = m+1;
                        }
                        break;
                    }
                    child = m;
                } else {
                    end_child = m-1;
                }
            }
            */
            // Sequential search behaves better
            int last_possible_child = (firstChild + r_-1 > (*prefix_ranks[level+1-1]).size()-1) ?  (*prefix_ranks[level+1-1]).size()-1 : firstChild + r_-1;
            while ( child <  last_possible_child && k > ((child+1-firstChild)*child_length -  (*prefix_ranks[level+1-1])[child+1])) {
                ++child;
            }
            k -= (child-firstChild)*child_length - (*prefix_ranks[level+1-1])[child];
            current_length = child_length;
            s += (child-firstChild)*current_length;
            current_block = child;
            ++level;
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;
            s -= encoded_offset%current_length;
            int second_rank = (current_length-(encoded_offset%current_length)) - (*second_ranks[level])[index];
            if (k > second_rank) {
                s += current_length;
                k-= second_rank;
                ++current_block;
            } else {
                k += (encoded_offset%current_length) - (*first_ranks[level])[index];
            }
        }
    }


    int d = mapping_[open_];
    for (int j = current_block*current_length;  ; ++j) {
        if ((*leaf_string_)[j] != d) --k;
        if (!k) return s + j - current_block*current_length;
    }
/*
    int qq = (onesRank*max_length_level + zerosRank*(max_length_level-1));
    return s+leaf_string_wt_->select(k + leaf_string_wt_->rank(qq, c), c)-qq;
*/
    return -1;
}

int BPSPCBlockTree::select_0_alternative(int k) {

    auto& ranks = bt_ranks_[open_];
    auto& second_ranks = bt_second_ranks_[open_];
    auto& first_level_prefix_ranks = bt_first_level_prefix_ranks_[open_];


    int current_block = (k-1)/first_level_length_;

    int end_block = number_of_blocks_first_level_-1;
    while (current_block != end_block) {
        int m = current_block + (end_block-current_block)/2;
        int f = first_level_length_*m - (*first_level_prefix_ranks)[m];
        if (f < k) {
            if (end_block - current_block == 1) {
                if ((first_level_length_*(m+1) - (*first_level_prefix_ranks)[m+1]) < k) {
                    current_block = m+1;
                }
                break;
            }
            current_block = m;
        } else {
            end_block = m-1;
        }
    }
    /* // Factorize
    while (initial_block < bt_bv_[0]->size()-1 && k > (*bt_prefix_ranks_[c][0])[initial_block+1]) {
        ++initial_block;
    }
     */
    int current_length = first_level_length_;
    int s = current_block*current_length;
    k -= s - (*first_level_prefix_ranks)[current_block];
    int level = 0;
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            int child = firstChild;
            int child_length = current_length/r_;
            int r = child_length - (*ranks[level+1])[child];
            int last_possible_child = (firstChild + r_-1 > (*ranks[level+1]).size()-1) ?  (*ranks[level+1]).size()-1 : firstChild + r_-1;
            while ( child < last_possible_child && k > r) { //Border conditions?
                ++child;
                r+= child_length - (*ranks[level+1])[child];
            }
            k -= r - (child_length - (*ranks[level+1])[child]);
            current_length = child_length;
            s += (child-firstChild)*current_length;
            current_block = child;
            ++level;
        } else { // Case BackBlock
            int index = current_block -  (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;

            k -= (current_length - encoded_offset%current_length) - (*second_ranks[level])[index];
            s -= encoded_offset%current_length;
            if (k > 0) {
                s += current_length;
                ++current_block;
            } else {
                k += current_length - (*ranks[level])[current_block];
            }

        }
    }

    int d = mapping_[open_];
    for (int j = current_block*current_length;  ; ++j) {
        if ((*leaf_string_)[j] != d) --k;
        if (!k) return s + j - current_block*current_length;
    }

    return -1;
}


int BPSPCBlockTree::select_0_alternative_noback(int k) {

    auto& ranks = bt_ranks_[open_];
    auto& first_level_prefix_ranks = bt_first_level_prefix_ranks_[open_];


    int current_block = (k-1)/first_level_length_;

    int end_block = number_of_blocks_first_level_-1;
    while (current_block != end_block) {
        int m = current_block + (end_block-current_block)/2;
        int f = first_level_length_*m - (*first_level_prefix_ranks)[m];
        if (f < k) {
            if (end_block - current_block == 1) {
                if ((first_level_length_*(m+1) - (*first_level_prefix_ranks)[m+1]) < k) {
                    current_block = m+1;
                }
                break;
            }
            current_block = m;
        } else {
            end_block = m-1;
        }
    }
    /* // Factorize
    while (initial_block < bt_bv_[0]->size()-1 && k > (*bt_prefix_ranks_[c][0])[initial_block+1]) {
        ++initial_block;
    }
     */
    int current_length = first_level_length_;
    int s = current_block*current_length;
    k -= s - (*first_level_prefix_ranks)[current_block];
    int level = 0;
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            int child = firstChild;
            int child_length = current_length/r_;
            int r = child_length - (*ranks[level+1])[child];
            int last_possible_child = (firstChild + r_-1 > (*ranks[level+1]).size()-1) ?  (*ranks[level+1]).size()-1 : firstChild + r_-1;
            while ( child < last_possible_child && k > r) { //Border conditions?
                ++child;
                r+= child_length - (*ranks[level+1])[child];
            }
            k -= r - (child_length - (*ranks[level+1])[child]);
            current_length = child_length;
            s += (child-firstChild)*current_length;
            current_block = child;
            ++level;
        } else { // Case BackBlock
            int index = current_block -  (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;

            k -= (current_length - encoded_offset%current_length) - ((*ranks[level])[current_block] - recursive_noback_rank_alternative(open_, encoded_offset%current_length - 1, level, current_block, current_length));
            s -= encoded_offset%current_length;
            if (k > 0) {
                s += current_length;
                ++current_block;
            } else {
                k += current_length - (*ranks[level])[current_block];
            }

        }
    }

    int d = mapping_[open_];
    for (int j = current_block*current_length;  ; ++j) {
        if ((*leaf_string_)[j] != d) --k;
        if (!k) return s + j - current_block*current_length;
    }

    return -1;
}

int BPSPCBlockTree::better_select_0(int k) {

    auto& cumulated_ranks = bt_cumulated_ranks_[open_];
    auto& second_ranks = bt_second_ranks_[open_];
    //auto first_ranks = bt_first_ranks_[c];
    auto& first_level_cumulated_ranks = bt_first_level_cumulated_ranks_[open_];

    int current_block = (k-1)/first_level_length_;

    int end_block = number_of_blocks_first_level_-1;
    while (current_block != end_block) {
        int m = current_block + (end_block-current_block)/2;
        int f = first_level_length_*m - (*first_level_cumulated_ranks)[m];
        if (f < k) {
            /*
            if (end_block - initial_block == 1) {
                if ((*first_level_prefix_ranks)[m+1] < k) {
                    initial_block = m+1;
                }
                break;
            }
            initial_block = m;
             */
            current_block = m+1;
        } else {
            end_block = m;

        }
    }
    /* // Change for first_level_prefix_ranks
    while (initial_block < bt_bv_[0]->size()-1 && k > (*bt_prefix_ranks_[c][0])[initial_block+1]) {
        ++initial_block;
    }
     */

    int s = current_block*first_level_length_;
    k -= (current_block == 0) ? 0 : (s- (*first_level_cumulated_ranks)[current_block-1]);


    int current_length = first_level_length_;
    int level = 0;
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            int child = firstChild;
            int child_length = current_length/r_;
            /* // Fix this to counter - 1, because of firstlevelprefixranks_ , and factorized map
            int first = firstChild;
            int last = firstChild + r_ - 1;

            while (true) {
                int m = first + (last-first)/2;
                if (m == firstChild+r_ - 1) {
                    child = m;
                    break;
                }
                int f = (*bt_prefix_ranks_[c][counter+1])[m];
                int s = (*bt_prefix_ranks_[c][counter+1])[m+1];
                if (f<k && s >= k) {
                    child = m;
                    break;
                }

                if (f >= k) {
                    last = m-1;
                } else {
                    first = m+1;
                }
            }
            */

            /* //Fix this
            int end_child = firstChild + r_-1;
            while (child != end_child) {
                int m = child + (end_child-child)/2;
                int f = (*prefix_ranks[counter+1-1])[m];
                if (f < k) {
                    if (end_child - child == 1) {
                        if ((*prefix_ranks[counter+1-1])[m+1] < k) {
                            child = m+1;
                        }
                        break;
                    }
                    child = m;
                } else {
                    end_child = m-1;
                }
            }
            */
            // Sequential search behaves better
            int last_possible_child = (firstChild + r_-1 > (*cumulated_ranks[level+1-1]).size()-1) ?  (*cumulated_ranks[level+1-1]).size()-1 : firstChild + r_-1;
            while ( child < last_possible_child && k > (child_length*(child-firstChild+1) - (*cumulated_ranks[level+1-1])[child])) {
                ++child;
            }

            k -= (child%r_ == 0) ? 0 : ((child-firstChild)*child_length - (*cumulated_ranks[level+1-1])[child-1]);
            current_length = child_length;
            s += (child-firstChild)*current_length;
            current_block = child;
            ++level;
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;
            s -= encoded_offset%current_length;
            k -= (current_length - encoded_offset%current_length) - (*second_ranks[level])[index];
            if (k > 0) {
                s += current_length;
                ++current_block;
            } else {
                if (level != 0 )k += current_length - ((current_block%r_ == 0) ?  ((*cumulated_ranks[level-1])[current_block]) :  (((*cumulated_ranks[level-1])[current_block]) - ((*cumulated_ranks[level-1])[current_block-1])));
                else k += current_length - ((current_block == 0) ?  ((*first_level_cumulated_ranks)[current_block]) :  (((*first_level_cumulated_ranks)[current_block]) - ((*first_level_cumulated_ranks)[current_block-1])));
            }

        }
    }

    int d = mapping_[open_];
    for (int j = current_block*current_length;  ; ++j) {
        if ((*leaf_string_)[j] != d) --k;
        if (!k) return s + j - current_block*current_length;
    }
/*
    int qq = (onesRank*max_length_level + zerosRank*(max_length_level-1));
    return s+leaf_string_wt_->select(k + leaf_string_wt_->rank(qq, c), c)-qq;
*/
    return -1;
}


int BPSPCBlockTree::rank_10(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i -= current_block*current_length;
    int level = 0;
    int r = (*bt_first_level_prefix_leaf_ranks_)[current_block];

    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;
            current_block = (*bt_bv_rank_[level])(current_block)*r_ + child_number;
            ++level;
            r += (*bt_prefix_leaf_ranks_[level-1])[current_block];
        } else { // Case BackBlock
            int index = current_block-(*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            int f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
            int s_condition = (*bt_suffix_start_with_end_leaf_[level])[index];
            if (f_condition && !s_condition) ++r;
            if (!f_condition && s_condition) --r;
            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            if (i >= current_length) {
                r += (*bt_second_leaf_ranks_[level])[index];
                ++current_block;
                i -= current_length;
                f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                if (f_condition && !s_condition) --r;
                if (!f_condition && s_condition) ++r;

            } else {
                r -= (*bt_first_leaf_ranks_[level])[index];
            }
        }
    }

    i  += current_block*current_length;

    bool one_seen = (*bt_starts_with_end_leaf_[level])[current_block];
    for (int j = current_block*current_length; j <= i; ++j) {
        if ((*leaf_string_)[j] == mapping_[open_]) {
            one_seen = true;
        } else {
            if (one_seen) ++r;
            one_seen = false;
        }
    }

    return r;
}


int BPSPCBlockTree::rank_10_alternative_noback(int i) {

    int current_block = i/first_level_length_;
    return (*bt_first_level_prefix_leaf_ranks_)[current_block] + recursive_rank_10_alternative_noback(i-current_block*first_level_length_, 0, current_block, first_level_length_);

}

int BPSPCBlockTree::recursive_rank_10_alternative_noback(int i, int level, int level_index, int level_length) {
    if (i == -1) return 0;
    int r = 0;
    if (level == number_of_levels_-1) {

        i  += level_index*level_length;
        bool one_seen = (*bt_starts_with_end_leaf_[level])[level_index];
        for (int j = level_index*level_length; j <= i; ++j) {
            if ((*leaf_string_)[j] == mapping_[open_]) {
                one_seen = true;
            } else {
                if (one_seen) ++r;
                one_seen = false;
            }
        }

        return r;
    }

    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        level_length /= r_;
        int child_number = i/level_length;
        i -= child_number*level_length;

        int firstChild = (*bt_bv_rank_[level])(level_index)*r_;
        for (int child = firstChild; child < firstChild + child_number; ++child)
            r += (*bt_leaf_ranks_[level+1])[child];
        level_index = firstChild + child_number;
        ++level;
    } else { // Case BackBlock
        /*
        int index = level_index - (*bt_bv_rank_[level])(level_index+1);
        int encoded_offset = (*bt_offsets_[level])[index];
        level_index = encoded_offset/level_length;
        i += encoded_offset%level_length;
        r -= recursive_noback_rank_alternative(c, encoded_offset%level_length - 1, level, level_index, level_length);
        if (i >= level_length) {
            r += (*ranks[level])[level_index];
            ++level_index;
            i -= level_length;
        }
         */
        int index = level_index - (*bt_bv_rank_[level])(level_index+1);
        int encoded_offset = (*bt_offsets_[level])[index];
        int f_condition = (*bt_starts_with_end_leaf_[level])[level_index];
        int s_condition = (*bt_suffix_start_with_end_leaf_[level])[index];
        if (f_condition && !s_condition) ++r;
        if (!f_condition && s_condition) --r;

        level_index = encoded_offset/level_length;
        i += encoded_offset%level_length;
        r -= recursive_rank_10_alternative_noback(encoded_offset%level_length - 1, level, level_index, level_length);
        if (i >= level_length) {
            r += (*bt_leaf_ranks_[level])[level_index];
            ++level_index;
            i -= level_length;
            f_condition = (*bt_starts_with_end_leaf_[level])[level_index];
            s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
            if (f_condition && !s_condition) --r;
            if (!f_condition && s_condition) ++r;
        }
    }

    return r + recursive_rank_10_alternative_noback(i, level, level_index, level_length);
}



int BPSPCBlockTree::rank_10_alternative(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i = i-current_block*current_length;
    int level = 0;

    int r = (*bt_first_level_prefix_leaf_ranks_)[current_block];
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;

            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            for (int child = firstChild; child < firstChild + child_number; ++child)
                r += (*bt_leaf_ranks_[level+1])[child];
            current_block = firstChild + child_number;
            ++level;
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            int f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
            int s_condition = (*bt_suffix_start_with_end_leaf_[level])[index];
            if (f_condition && !s_condition) ++r;
            if (!f_condition && s_condition) --r;

            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            r += (*bt_second_leaf_ranks_[level])[index];
            if (i >= current_length) {
                ++current_block;
                i -= current_length;
                f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                if (f_condition && !s_condition) --r;
                if (!f_condition && s_condition) ++r;
            } else {
                r -= (*bt_leaf_ranks_[level])[current_block];
            }

        }
    }

    i  += current_block*current_length;

    bool one_seen = (*bt_starts_with_end_leaf_[level])[current_block];
    for (int j = current_block*current_length; j <= i; ++j) {
        if ((*leaf_string_)[j] == mapping_[open_]) {
            one_seen = true;
        } else {
            if (one_seen) ++r;
            one_seen = false;
        }
    }

    return r;
}

bool BPSPCBlockTree::is_leaf_rank_alternative(int i, int& leaf_rank) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i = i-current_block*current_length;
    int level = 0;

    leaf_rank = (*bt_first_level_prefix_leaf_ranks_)[current_block];
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;

            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            for (int child = firstChild; child < firstChild + child_number; ++child)
                leaf_rank += (*bt_leaf_ranks_[level+1])[child];
            current_block = firstChild + child_number;
            ++level;
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            int f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
            int s_condition = (*bt_suffix_start_with_end_leaf_[level])[index];
            if (f_condition && !s_condition) ++leaf_rank;
            if (!f_condition && s_condition) --leaf_rank;

            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            leaf_rank += (*bt_second_leaf_ranks_[level])[index];
            if (i >= current_length) {
                ++current_block;
                i -= current_length;
                f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                if (f_condition && !s_condition) --leaf_rank;
                if (!f_condition && s_condition) ++leaf_rank;
            } else {
                leaf_rank -= (*bt_leaf_ranks_[level])[current_block];
            }

        }
    }

    i  += current_block*current_length;

    bool one_seen = (*bt_starts_with_end_leaf_[level])[current_block];
    for (int j = current_block*current_length; j <= i; ++j) {
        if ((*leaf_string_)[j] == mapping_[open_]) {
            one_seen = true;
        } else {
            if (one_seen) ++leaf_rank;
            one_seen = false;
        }
    }

    return (i < leaf_string_->size() && (*leaf_string_)[i] != mapping_[open_]);
}

bool BPSPCBlockTree::is_leaf_rank_alternative_noback(int i, int& leaf_rank) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i = i-current_block*current_length;
    int level = 0;

    leaf_rank = (*bt_first_level_prefix_leaf_ranks_)[current_block];
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;

            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            for (int child = firstChild; child < firstChild + child_number; ++child)
                leaf_rank += (*bt_leaf_ranks_[level+1])[child];
            current_block = firstChild + child_number;
            ++level;
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            int f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
            int s_condition = (*bt_suffix_start_with_end_leaf_[level])[index];
            if (f_condition && !s_condition) ++leaf_rank;
            if (!f_condition && s_condition) --leaf_rank;

            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            leaf_rank -= recursive_rank_10_alternative_noback(encoded_offset%current_length - 1, level, current_block, current_length);
            if (i >= current_length) {
                leaf_rank += (*bt_leaf_ranks_[level])[current_block];
                ++current_block;
                i -= current_length;
                f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                if (f_condition && !s_condition) --leaf_rank;
                if (!f_condition && s_condition) ++leaf_rank;
            }

        }
    }

    i  += current_block*current_length;

    bool one_seen = (*bt_starts_with_end_leaf_[level])[current_block];
    for (int j = current_block*current_length; j <= i; ++j) {
        if ((*leaf_string_)[j] == mapping_[open_]) {
            one_seen = true;
        } else {
            if (one_seen) ++leaf_rank;
            one_seen = false;
        }
    }

    return (i < leaf_string_->size() && (*leaf_string_)[i] != mapping_[open_]);
}


int BPSPCBlockTree::better_rank_10(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;

    i -= current_block*current_length;
    int level = 0;

    int r = (current_block == 0) ? 0 : (*bt_first_level_cumulated_leaf_ranks_)[current_block-1];
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;
            current_block = (*bt_bv_rank_[level])(current_block)*r_ + child_number;
            ++level;
            r += (current_block%r_ == 0) ? 0 : (*bt_cumulated_leaf_ranks_[level-1])[current_block-1];
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            int f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
            int s_condition = (*bt_suffix_start_with_end_leaf_[level])[index];
            if (f_condition && !s_condition) ++r;
            if (!f_condition && s_condition) --r;
            current_block = encoded_offset/current_length;

            i += encoded_offset%current_length;
            r += (*bt_second_leaf_ranks_[level])[index];
            if (i  >= current_length) {
                ++current_block;
                i -= current_length;
                f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                if (f_condition && !s_condition) --r;
                if (!f_condition && s_condition) ++r;
            } else {
                if (level != 0 )r -= ((current_block%r_ == 0) ?  ((*bt_cumulated_leaf_ranks_[level-1])[current_block]) :  (((*bt_cumulated_leaf_ranks_[level-1])[current_block]) - ((*bt_cumulated_leaf_ranks_[level-1])[current_block-1])) );
                else r -= ((current_block == 0) ?  ((*bt_first_level_cumulated_leaf_ranks_)[current_block]) :  (((*bt_first_level_cumulated_leaf_ranks_)[current_block]) - ((*bt_first_level_cumulated_leaf_ranks_)[current_block-1])) );
            }

        }
    }

    i  += current_block*current_length;

    bool one_seen = (*bt_starts_with_end_leaf_[level])[current_block];
    for (int j = current_block*current_length; j <= i; ++j) {
        if ((*leaf_string_)[j] == mapping_[open_]) {
            one_seen = true;
        } else {
            if (one_seen) ++r;
            one_seen = false;
        }
    }

    return r;
}

int BPSPCBlockTree::select_10(int k) {

    int current_block = (k-1)/first_level_length_;

    int end_block = number_of_blocks_first_level_-1;
    while (current_block != end_block) {
        int m = current_block + (end_block-current_block)/2;
        int f = (*bt_first_level_prefix_leaf_ranks_)[m];
        if (f < k) {
            if (end_block - current_block == 1) {
                if ((*bt_first_level_prefix_leaf_ranks_)[m+1] < k) {
                    current_block = m+1;
                }
                break;
            }
            current_block = m;
        } else {
            end_block = m-1;
        }
    }
    /* // Change for first_level_prefix_ranks
    while (initial_block < bt_bv_[0]->size()-1 && k > (*bt_prefix_ranks_[c][0])[initial_block+1]) {
        ++initial_block;
    }
     */

    int s = current_block*first_level_length_;
    k -= (*bt_first_level_prefix_leaf_ranks_)[current_block];
    int current_length = first_level_length_;
    int level = 0;
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            int child = firstChild;
            /* // Fix this to counter - 1, because of firstlevelprefixranks_ , and factorized map
            int first = firstChild;
            int last = firstChild + r_ - 1;

            while (true) {
                int m = first + (last-first)/2;
                if (m == firstChild+r_ - 1) {
                    child = m;
                    break;
                }
                int f = (*bt_prefix_ranks_[c][counter+1])[m];
                int s = (*bt_prefix_ranks_[c][counter+1])[m+1];
                if (f<k && s >= k) {
                    child = m;
                    break;
                }

                if (f >= k) {
                    last = m-1;
                } else {
                    first = m+1;
                }
            }
            */
            /*
            int end_child = firstChild + r_-1;
            while (child != end_child) {
                int m = child + (end_child-child)/2;
                int f = (*prefix_ranks[counter+1-1])[m];
                if (f < k) {
                    if (end_child - child == 1) {
                        if ((*prefix_ranks[counter+1-1])[m+1] < k) {
                            child = m+1;
                        }
                        break;
                    }
                    child = m;
                } else {
                    end_child = m-1;
                }
            }
            */
            // Sequential search behaves better
            int last_possible_child = (firstChild + r_-1 > (*bt_prefix_leaf_ranks_[level+1-1]).size()-1) ?  (*bt_prefix_leaf_ranks_[level+1-1]).size()-1 : firstChild + r_-1;
            while ( child <  last_possible_child && k > (*bt_prefix_leaf_ranks_[level+1-1])[child+1]) {
                ++child;
            }
            k -= (*bt_prefix_leaf_ranks_[level+1-1])[child];
            current_length /= r_;
            s += (child-firstChild)*current_length;
            current_block = child;
            ++level;
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            int f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
            int s_condition = (*bt_suffix_start_with_end_leaf_[level])[index];
            if (f_condition && !s_condition) --k;
            if (!f_condition && s_condition) ++k;
            current_block = encoded_offset/current_length;
            int second_rank = (*bt_second_leaf_ranks_[level])[index];
            if (k > second_rank) {
                s += current_length - encoded_offset%current_length;
                ++current_block;
                f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                if (f_condition && !s_condition) ++k;
                if (!f_condition && s_condition) --k;
                k-= second_rank;
                if (k == 0) return s-1;
            } else {
                if (k == 0) return s-1;
                s -= encoded_offset%current_length;
                k += (*bt_first_leaf_ranks_[level])[index];
            }
        }
    }

    bool one_seen = (*bt_starts_with_end_leaf_[level])[current_block];
    if (one_seen && k == 1) return s-1;
    for (int j = current_block*current_length; ; ++j) {
        if ((*leaf_string_)[j] == mapping_[open_]) {
            one_seen = true;
        } else {
            if (one_seen) {
                --k;
            }
            one_seen = false;
        }
        if (!k) return s + j - current_block*current_length-1;
    }
    return -1;

/*
    int qq = (onesRank*max_length_level + zerosRank*(max_length_level-1));
    return s+leaf_string_wt_->select(k + leaf_string_wt_->rank(qq, c), c)-qq;
*/
    return -1;
}


int BPSPCBlockTree::select_10_alternative(int k) {

    int current_block = (k-1)/first_level_length_;

    int end_block = number_of_blocks_first_level_-1;
    while (current_block != end_block) {
        int m = current_block + (end_block-current_block)/2;
        int f = (*bt_first_level_prefix_leaf_ranks_)[m];
        if (f < k) {
            if (end_block - current_block == 1) {
                if ((*bt_first_level_prefix_leaf_ranks_)[m+1] < k) {
                    current_block = m+1;
                }
                break;
            }
            current_block = m;
        } else {
            end_block = m-1;
        }
    }
    /* // Factorize
    while (initial_block < bt_bv_[0]->size()-1 && k > (*bt_prefix_ranks_[c][0])[initial_block+1]) {
        ++initial_block;
    }
     */
    int current_length = first_level_length_;
    int s = current_block*current_length;
    k -= (*bt_first_level_prefix_leaf_ranks_)[current_block];
    int level = 0;
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            int child = firstChild;
            int r = (*bt_leaf_ranks_[level+1])[child];
            int last_possible_child = (firstChild + r_-1 > (*bt_leaf_ranks_[level+1]).size()-1) ?  (*bt_leaf_ranks_[level+1]).size()-1 : firstChild + r_-1;
            while ( child < last_possible_child && k > r) { //Border conditions?
                ++child;
                r+= (*bt_leaf_ranks_[level+1])[child];
            }
            k -= r - (*bt_leaf_ranks_[level+1])[child];
            current_length /= r_;
            s += (child-firstChild)*current_length;
            current_block = child;
            ++level;
        } else { // Case BackBlock
            int index = current_block -  (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            int f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
            int s_condition = (*bt_suffix_start_with_end_leaf_[level])[index];
            if (f_condition && !s_condition) --k;
            if (!f_condition && s_condition) ++k;
            current_block = encoded_offset/current_length;

            int second_rank = (*bt_second_leaf_ranks_[level])[index];

            if (k > second_rank) {
                k -= second_rank;
                s += current_length - encoded_offset%current_length;
                ++current_block;
                f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                if (f_condition && !s_condition) ++k;
                if (!f_condition && s_condition) --k;
                if (k == 0) return s-1;
            } else {
                if (k == 0) return s-1;
                s -= encoded_offset%current_length;
                k += (*bt_leaf_ranks_[level])[current_block] - second_rank;;
            }

        }
    }

    bool one_seen = (*bt_starts_with_end_leaf_[level])[current_block];
    if (one_seen && k == 1) return s-1;
    for (int j = current_block*current_length; ; ++j) {
        if ((*leaf_string_)[j] == mapping_[open_]) {
            one_seen = true;
        } else {
            if (one_seen) {
                --k;
            }
            one_seen = false;
        }
        if (!k) return s + j - current_block*current_length-1;
    }
    return -1;
}



int BPSPCBlockTree::select_10_alternative_noback(int k) {

    int current_block = (k-1)/first_level_length_;

    int end_block = number_of_blocks_first_level_-1;
    while (current_block != end_block) {
        int m = current_block + (end_block-current_block)/2;
        int f = (*bt_first_level_prefix_leaf_ranks_)[m];
        if (f < k) {
            if (end_block - current_block == 1) {
                if ((*bt_first_level_prefix_leaf_ranks_)[m+1] < k) {
                    current_block = m+1;
                }
                break;
            }
            current_block = m;
        } else {
            end_block = m-1;
        }
    }
    /* // Factorize
    while (initial_block < bt_bv_[0]->size()-1 && k > (*bt_prefix_ranks_[c][0])[initial_block+1]) {
        ++initial_block;
    }
     */
    int current_length = first_level_length_;
    int s = current_block*current_length;
    k -= (*bt_first_level_prefix_leaf_ranks_)[current_block];
    int level = 0;
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            int child = firstChild;
            int r = (*bt_leaf_ranks_[level+1])[child];
            int last_possible_child = (firstChild + r_-1 > (*bt_leaf_ranks_[level+1]).size()-1) ?  (*bt_leaf_ranks_[level+1]).size()-1 : firstChild + r_-1;
            while ( child < last_possible_child && k > r) { //Border conditions?
                ++child;
                r+= (*bt_leaf_ranks_[level+1])[child];
            }
            k -= r - (*bt_leaf_ranks_[level+1])[child];
            current_length /= r_;
            s += (child-firstChild)*current_length;
            current_block = child;
            ++level;
        } else { // Case BackBlock
            int index = current_block -  (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            int f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
            int s_condition = (*bt_suffix_start_with_end_leaf_[level])[index];
            if (f_condition && !s_condition) --k;
            if (!f_condition && s_condition) ++k;
            current_block = encoded_offset/current_length;

            int second_rank = (*bt_leaf_ranks_[level])[current_block] - recursive_rank_10_alternative_noback(encoded_offset%current_length - 1, level, current_block, current_length);

            if (k > second_rank) {
                k -= second_rank;
                s += current_length - encoded_offset%current_length;
                ++current_block;
                f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                if (f_condition && !s_condition) ++k;
                if (!f_condition && s_condition) --k;
                if (k == 0) return s-1;
            } else {
                if (k == 0) return s-1;
                s -= encoded_offset%current_length;
                k += (*bt_leaf_ranks_[level])[current_block] - second_rank;;
            }

        }
    }

    bool one_seen = (*bt_starts_with_end_leaf_[level])[current_block];
    if (one_seen && k == 1) return s-1;
    for (int j = current_block*current_length; ; ++j) {
        if ((*leaf_string_)[j] == mapping_[open_]) {
            one_seen = true;
        } else {
            if (one_seen) {
                --k;
            }
            one_seen = false;
        }
        if (!k) return s + j - current_block*current_length-1;
    }
    return -1;
}

int BPSPCBlockTree::better_select_10(int k) {

    int current_block = (k-1)/first_level_length_;

    int end_block = number_of_blocks_first_level_-1;
    while (current_block != end_block) {
        int m = current_block + (end_block-current_block)/2;
        int f = (*bt_first_level_cumulated_leaf_ranks_)[m];
        if (f < k) {
            /*
            if (end_block - initial_block == 1) {
                if ((*first_level_prefix_ranks)[m+1] < k) {
                    initial_block = m+1;
                }
                break;
            }
            initial_block = m;
             */
            current_block = m+1;
        } else {
            end_block = m;

        }
    }
    /* // Change for first_level_prefix_ranks
    while (initial_block < bt_bv_[0]->size()-1 && k > (*bt_prefix_ranks_[c][0])[initial_block+1]) {
        ++initial_block;
    }
     */

    int s = current_block*first_level_length_;
    k -= (current_block == 0) ? 0 : (*bt_first_level_cumulated_leaf_ranks_)[current_block-1];


    int current_length = first_level_length_;
    int level = 0;
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            int child = firstChild;
            /* // Fix this to counter - 1, because of firstlevelprefixranks_ , and factorized map
            int first = firstChild;
            int last = firstChild + r_ - 1;

            while (true) {
                int m = first + (last-first)/2;
                if (m == firstChild+r_ - 1) {
                    child = m;
                    break;
                }
                int f = (*bt_prefix_ranks_[c][counter+1])[m];
                int s = (*bt_prefix_ranks_[c][counter+1])[m+1];
                if (f<k && s >= k) {
                    child = m;
                    break;
                }

                if (f >= k) {
                    last = m-1;
                } else {
                    first = m+1;
                }
            }
            */

            /* //Fix this
            int end_child = firstChild + r_-1;
            while (child != end_child) {
                int m = child + (end_child-child)/2;
                int f = (*prefix_ranks[counter+1-1])[m];
                if (f < k) {
                    if (end_child - child == 1) {
                        if ((*prefix_ranks[counter+1-1])[m+1] < k) {
                            child = m+1;
                        }
                        break;
                    }
                    child = m;
                } else {
                    end_child = m-1;
                }
            }
            */
            // Sequential search behaves better
            int last_possible_child = (firstChild + r_-1 > (*bt_cumulated_leaf_ranks_[level+1-1]).size()-1) ?  (*bt_cumulated_leaf_ranks_[level+1-1]).size()-1 : firstChild + r_-1;
            while ( child < last_possible_child && k > (*bt_cumulated_leaf_ranks_[level+1-1])[child]) {
                ++child;
            }

            k -= (child%r_ == 0) ? 0 : (*bt_cumulated_leaf_ranks_[level+1-1])[child-1];
            current_length /= r_;
            s += (child-firstChild)*current_length;
            current_block = child;
            ++level;
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            int f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
            int s_condition = (*bt_suffix_start_with_end_leaf_[level])[index];
            if (f_condition && !s_condition) --k;
            if (!f_condition && s_condition) ++k;
            current_block = encoded_offset/current_length;
            int second_rank = (*bt_second_leaf_ranks_[level])[index];
            if (k > second_rank) {
                k -= second_rank;
                s += current_length - encoded_offset%current_length;
                ++current_block;
                f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                if (f_condition && !s_condition) ++k;
                if (!f_condition && s_condition) --k;
                if (k == 0) return s-1;
            } else {
                if (k == 0) return s-1;
                s -= encoded_offset%current_length;
                k -= second_rank;
                if (level != 0 )k += ((current_block%r_ == 0) ?  ((*bt_cumulated_leaf_ranks_[level-1])[current_block]) :  (((*bt_cumulated_leaf_ranks_[level-1])[current_block]) - ((*bt_cumulated_leaf_ranks_[level-1])[current_block-1])));
                else k += ((current_block == 0) ?  ((*bt_first_level_cumulated_leaf_ranks_)[current_block]) :  (((*bt_first_level_cumulated_leaf_ranks_)[current_block]) - ((*bt_first_level_cumulated_leaf_ranks_)[current_block-1])));
            }

        }
    }

    bool one_seen = (*bt_starts_with_end_leaf_[level])[current_block];
    if (one_seen && k == 1) return s-1;
    for (int j = current_block*current_length; ; ++j) {
        if ((*leaf_string_)[j] == mapping_[open_]) {
            one_seen = true;
        } else {
            if (one_seen) {
                --k;
            }
            one_seen = false;
        }
        if (!k) return s + j - current_block*current_length-1;
    }
    return -1;
}

int BPSPCBlockTree::min_excess(int i, int j) {
    int e = 0;
    if (i/first_level_length_ == j/first_level_length_) return min_excess(i%first_level_length_,j%first_level_length_,0,i/first_level_length_,first_level_length_,e);
    int m = min_excess(i%first_level_length_,first_level_length_-1,0,i/first_level_length_,first_level_length_,e);
    int be = e;
    //int m2 = next_block_min_excess(i/first_level_length_, j/first_level_length_ , e);
    int m2 = faster_next_block_min_excess(i/first_level_length_, j/first_level_length_ , e);
    if (m2+be < m) m = m2+be;
    be = e;
    int m3 = min_excess(0,j%first_level_length_,0,j/first_level_length_,first_level_length_,e);
    if (m3+be< m) m = m3+be;
    return m;
}



int BPSPCBlockTree::max_excess(int i, int j) {
    int e = 0;
    if (i/first_level_length_ == j/first_level_length_) return max_excess(i%first_level_length_,j%first_level_length_,0,i/first_level_length_,first_level_length_,e);
    int M = max_excess(i%first_level_length_,first_level_length_-1,0,i/first_level_length_,first_level_length_,e);
    int be = e;
    //int M2 = next_block_max_excess(i/first_level_length_, j/first_level_length_ , e);
    int M2 = faster_next_block_max_excess(i/first_level_length_, j/first_level_length_ , e);
    if (M2+be > M) M = M2+be;
    be = e;
    int M3 = max_excess(0,j%first_level_length_,0,j/first_level_length_,first_level_length_,e);
    if (M3+be> M) M = M3+be;
    return M;
}


int BPSPCBlockTree::my_max_excess(int i, int j) {
    int e = 0;
    if (i/first_level_length_ == j/first_level_length_) return my_max_excess(i%first_level_length_,j%first_level_length_,0,i/first_level_length_,first_level_length_,e);
    int M = my_max_excess(i%first_level_length_,first_level_length_-1,0,i/first_level_length_,first_level_length_,e);
    int be = e;
    //int M2 = my_next_block_max_excess(i/first_level_length_, j/first_level_length_ , e);
    int M2 = my_faster_next_block_max_excess(i/first_level_length_, j/first_level_length_ , e);
    if (M2+be > M) M = M2+be;
    be = e;
    int M3 = my_max_excess(0,j%first_level_length_,0,j/first_level_length_,first_level_length_,e);
    if (M3+be> M) M = M3+be;
    return M;
}


int BPSPCBlockTree::my_max_excess_nobackmax(int i, int j) {
    int e = 0;
    if (i/first_level_length_ == j/first_level_length_) return my_max_excess_nobackmax(i%first_level_length_,j%first_level_length_,0,i/first_level_length_,first_level_length_,e);
    int M = my_max_excess_nobackmax(i%first_level_length_,first_level_length_-1,0,i/first_level_length_,first_level_length_,e);
    int be = e;
    //int M2 = my_next_block_max_excess(i/first_level_length_, j/first_level_length_ , e);
    int M2 = my_faster_next_block_max_excess(i/first_level_length_, j/first_level_length_ , e);
    if (M2+be > M) M = M2+be;
    be = e;
    int M3 = my_max_excess_nobackmax(0,j%first_level_length_,0,j/first_level_length_,first_level_length_,e);
    if (M3+be> M) M = M3+be;
    return M;
}


int BPSPCBlockTree::my_min_excess(int i, int j) {
    int e = 0;
    if (i/first_level_length_ == j/first_level_length_) return my_min_excess(i%first_level_length_,j%first_level_length_,0,i/first_level_length_,first_level_length_,e);
    int m = my_min_excess(i%first_level_length_,first_level_length_-1,0,i/first_level_length_,first_level_length_,e);
    int be = e;
    //int m2 = my_next_block_min_excess(i/first_level_length_, j/first_level_length_ , e);
    int m2 = my_faster_next_block_min_excess(i/first_level_length_, j/first_level_length_ , e);
    if (m2+be < m) m = m2+be;
    be = e;
    int m3 = my_min_excess(0,j%first_level_length_,0,j/first_level_length_,first_level_length_,e);
    if (m3+be< m) m = m3+be;
    return m;
}


int BPSPCBlockTree::my_min_excess_cback(int i, int j) {
    int e = 0;
    if (i/first_level_length_ == j/first_level_length_) return my_min_excess_cback(i%first_level_length_,j%first_level_length_,0,i/first_level_length_,first_level_length_,e);
    int m = my_min_excess_cback(i%first_level_length_,first_level_length_-1,0,i/first_level_length_,first_level_length_,e);
    int be = e;
    //int m2 = my_next_block_min_excess(i/first_level_length_, j/first_level_length_ , e);
    int m2 = my_faster_next_block_min_excess(i/first_level_length_, j/first_level_length_ , e);
    if (m2+be < m) m = m2+be;
    be = e;
    int m3 = my_min_excess_cback(0,j%first_level_length_,0,j/first_level_length_,first_level_length_,e);
    if (m3+be< m) m = m3+be;
    return m;
}

int BPSPCBlockTree::my_min_excess_nobackmin(int i, int j) {
    int e = 0;
    if (i/first_level_length_ == j/first_level_length_) return my_min_excess_nobackmin(i%first_level_length_,j%first_level_length_,0,i/first_level_length_,first_level_length_,e);
    int m = my_min_excess_nobackmin(i%first_level_length_,first_level_length_-1,0,i/first_level_length_,first_level_length_,e);
    int be = e;
    //int m2 = my_next_block_min_excess(i/first_level_length_, j/first_level_length_ , e);
    int m2 = my_faster_next_block_min_excess(i/first_level_length_, j/first_level_length_ , e);
    if (m2+be < m) m = m2+be;
    be = e;
    int m3 = my_min_excess_nobackmin(0,j%first_level_length_,0,j/first_level_length_,first_level_length_,e);
    if (m3+be< m) m = m3+be;
    return m;
}

int BPSPCBlockTree::min_count(int i, int j) {
    int e = 0;
    int m = 1;
    if (i/first_level_length_ == j/first_level_length_) return min_count(i%first_level_length_,j%first_level_length_,0,i/first_level_length_,first_level_length_,e,m);
    int c = min_count(i%first_level_length_,first_level_length_-1,0,i/first_level_length_,first_level_length_,e,m);
    int be = e;
    int min = 1;
    //int c2 = next_block_min_count(i/first_level_length_, j/first_level_length_ , e,min);
    int c2 = faster_next_block_min_count(i/first_level_length_, j/first_level_length_ , e,min);
    if (min+be == m) c +=c2;
    if (min+be < m) {
        c = c2;
        m = min+be;
    }

    be = e;
    min = 1;
    int c3 = min_count(0,j%first_level_length_,0,j/first_level_length_,first_level_length_,e,min);
    if (min+be == m) c +=c3;
    if (min+be < m) {
        c = c3;
        m = min+be;
    }
    return c;
}


int BPSPCBlockTree::my_min_count(int i, int j) {
    int e = 0;
    int m = 1;
    if (i/first_level_length_ == j/first_level_length_) return my_min_count(i%first_level_length_,j%first_level_length_,0,i/first_level_length_,first_level_length_,e,m);
    int c = my_min_count(i%first_level_length_,first_level_length_-1,0,i/first_level_length_,first_level_length_,e,m);
    int be = e;
    int min = 1;
    //int c2 = my_next_block_min_count(i/first_level_length_, j/first_level_length_ , e,min);
    int c2 = my_faster_next_block_min_count(i/first_level_length_, j/first_level_length_ , e,min);
    if (min+be == m) c +=c2;
    if (min+be < m) {
        c = c2;
        m = min+be;
    }

    be = e;
    min = 1;
    int c3 = my_min_count(0,j%first_level_length_,0,j/first_level_length_,first_level_length_,e,min);
    if (min+be == m) c +=c3;
    if (min+be < m) {
        c = c3;
        m = min+be;
    }
    return c;
}


int BPSPCBlockTree::my_min_count_nobackmin(int i, int j) {
    int e = 0;
    int m = 1;
    if (i/first_level_length_ == j/first_level_length_) return my_min_count_nobackmin(i%first_level_length_,j%first_level_length_,0,i/first_level_length_,first_level_length_,e,m);
    int c = my_min_count_nobackmin(i%first_level_length_,first_level_length_-1,0,i/first_level_length_,first_level_length_,e,m);
    int be = e;
    int min = 1;
    //int c2 = my_next_block_min_count(i/first_level_length_, j/first_level_length_ , e,min);
    int c2 = my_faster_next_block_min_count(i/first_level_length_, j/first_level_length_ , e,min);
    if (min+be == m) c +=c2;
    if (min+be < m) {
        c = c2;
        m = min+be;
    }

    be = e;
    min = 1;
    int c3 = my_min_count_nobackmin(0,j%first_level_length_,0,j/first_level_length_,first_level_length_,e,min);
    if (min+be == m) c +=c3;
    if (min+be < m) {
        c = c3;
        m = min+be;
    }
    return c;
}


int BPSPCBlockTree::next_block_min_count(int initial_block, int end_block, int& e, int& m) {
    int c = 0;
    int excess = 0;
    m = 1;
    for (int i = initial_block+1; i < end_block; ++i) {
        int local_min = excess + 1-(int)((*(bt_prefix_min_excess_[0]))[i]);
        if (local_min < m) {
            m = local_min;
            c = ((*(bt_min_count_[0]))[i]);
        } else if (local_min == m) {
            c += ((*(bt_min_count_[0]))[i]);
        }

        int rank_ones = (i == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[i-1])));
        excess += 2*rank_ones - first_level_length_ ;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess += (*pair.second[0])[i];
            } else {
                excess -= (*pair.second[0])[i];
            }
        }
         */
    }
    e += excess;
    return c;
}


int BPSPCBlockTree::my_next_block_min_count(int initial_block, int end_block, int& e, int& m) {
    int c = 0;
    int excess = 0;
    m = 1;
    for (int i = initial_block+1; i < end_block; ++i) {
        int local_min = excess + 1-(int)((*(bt_prefix_min_excess_[0]))[i]);
        if (local_min < m) {
            m = local_min;
            c = ((*(bt_min_count_[0]))[i]);
        } else if (local_min == m) {
            c += ((*(bt_min_count_[0]))[i]);
        }

        int rank_ones = ((int)((*bt_ranks_[open_][0])[i]));
        excess += 2*rank_ones - first_level_length_ ;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess += (*pair.second[0])[i];
            } else {
                excess -= (*pair.second[0])[i];
            }
        }
         */
    }
    e += excess;
    return c;
}

int BPSPCBlockTree::faster_next_block_min_count(int initial_block, int end_block, int& e, int& m) {
    m = 1;
    int c = 0;
    if (end_block > initial_block + 1) {
        int r_initial_block = 0;
        if (initial_block < n_last_level_)
            r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
        else
            r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;

        int r_end_block = 0;
        if (end_block < n_last_level_)
            r_end_block = end_block + n_pre_last_level_ + n_internal_nodes_;
        else
            r_end_block = end_block - n_last_level_ + n_internal_nodes_;
        bool going_up = true;
        int excess = 0;
        std::stack<int> down_path;
        bool different_height =  initial_block < n_last_level_ && end_block >= n_last_level_;
        while (true) {
            if (going_up) {
                while (r_initial_block % r_ != 0) {
                    ++r_initial_block;
                    if (r_initial_block >= number_of_blocks_first_level_ + n_internal_nodes_) break;
                    if (r_initial_block == r_end_block) {
                        going_up = false;
                        break;
                    }
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                       r_initial_block -
                                                                                                       n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_ - n_internal_nodes_;;
                        if (initial_block >= number_of_blocks_first_level_) break;

                        int local_min = excess + 1-(int) ((*(bt_prefix_min_excess_[0]))[initial_block]);
                        if (local_min == m) c += (*(bt_min_count_[0]))[initial_block];
                        if (local_min < m) {
                            m = local_min;
                            c = (*(bt_min_count_[0]))[initial_block];
                        }
                        int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                        excess += 2*rank_ones - first_level_length_;
                        /*
                        for (auto& pair: bt_ranks_) {
                            if (pair.first == open_) {
                                excess += (*pair.second[0])[initial_block];
                            } else {
                                excess -= (*pair.second[0])[initial_block];
                            }
                        }
                         */

                    } else {
                        int local_min = excess + 1-(int) ((*(top_min_excess_))[r_initial_block]);
                        if (local_min == m) c+= (*(top_min_count_))[r_initial_block];
                        if (local_min < m) {
                            m = local_min;
                            c= (*(top_min_count_))[r_initial_block];
                        }
                        excess += decoded_excess((*top_excess_)[r_initial_block]);
                    }
                }

                //LOOKUP
                if (going_up && r_initial_block +1 == r_end_block) {
                    down_path.push(r_end_block);
                    ++ r_initial_block;
                    going_up = false;
                }

                if (going_up) {
                    r_initial_block = (r_initial_block - 1) / r_;
                    if (different_height) different_height = !different_height;
                    else {
                        down_path.push(r_end_block);
                        r_end_block = (r_end_block - 1) / r_;
                    }
                }
            } else {
                while (!down_path.empty()) {
                    int next_r_index = down_path.top();
                    down_path.pop();
                    for (int r_sibling = ((next_r_index-1)/r_)*r_ + 1; r_sibling < next_r_index; ++r_sibling) {
                        if (r_sibling >= n_internal_nodes_) {
                            if (r_sibling - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                     r_sibling -
                                                                                                     n_internal_nodes_;
                            else initial_block = r_sibling - n_pre_last_level_ - n_internal_nodes_;
                            //if (initial_block >= number_of_blocks_first_level_) break;

                            int local_min = excess + 1-(int) ((*(bt_prefix_min_excess_[0]))[initial_block]);
                            if (local_min == m) c += (*(bt_min_count_[0]))[initial_block];
                            if (local_min < m) {
                                m = local_min;
                                c = (*(bt_min_count_[0]))[initial_block];
                            }
                            int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                            excess += 2*rank_ones - first_level_length_;
                            /*
                            for (auto& pair: bt_ranks_) {
                                if (pair.first == open_) {
                                    excess += (*pair.second[0])[initial_block];
                                } else {
                                    excess -= (*pair.second[0])[initial_block];
                                }
                            }
                             */

                        } else {
                            int local_min = excess + 1-(int) ((*(top_min_excess_))[r_sibling]);
                            if (local_min == m) c+= (*(top_min_count_))[r_sibling];
                            if (local_min < m) {
                                m = local_min;
                                c= (*(top_min_count_))[r_sibling];
                            }
                            excess += decoded_excess((*top_excess_)[r_sibling]);
                        }
                    }

                }
                break;
            }
        }
        e += excess;
    }
    return c;
}



int BPSPCBlockTree::my_faster_next_block_min_count(int initial_block, int end_block, int& e, int& m) {
    m = 1;
    int c = 0;
    if (end_block > initial_block + 1) {
        int r_initial_block = 0;
        if (initial_block < n_last_level_)
            r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
        else
            r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;

        int r_end_block = 0;
        if (end_block < n_last_level_)
            r_end_block = end_block + n_pre_last_level_ + n_internal_nodes_;
        else
            r_end_block = end_block - n_last_level_ + n_internal_nodes_;
        bool going_up = true;
        int excess = 0;
        std::stack<int> down_path;
        bool different_height =  initial_block < n_last_level_ && end_block >= n_last_level_;
        while (true) {
            if (going_up) {
                while (r_initial_block % r_ != 0) {
                    ++r_initial_block;
                    if (r_initial_block >= number_of_blocks_first_level_ + n_internal_nodes_) break;
                    if (r_initial_block == r_end_block) {
                        going_up = false;
                        break;
                    }
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                     r_initial_block -
                                                                                                     n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_ - n_internal_nodes_;;
                        if (initial_block >= number_of_blocks_first_level_) break;

                        int local_min = excess + 1-(int) ((*(bt_prefix_min_excess_[0]))[initial_block]);
                        if (local_min == m) c += (*(bt_min_count_[0]))[initial_block];
                        if (local_min < m) {
                            m = local_min;
                            c = (*(bt_min_count_[0]))[initial_block];
                        }
                        int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                        excess += 2*rank_ones - first_level_length_;
                        /*
                        for (auto& pair: bt_ranks_) {
                            if (pair.first == open_) {
                                excess += (*pair.second[0])[initial_block];
                            } else {
                                excess -= (*pair.second[0])[initial_block];
                            }
                        }
                         */

                    } else {
                        int local_min = excess + 1-(int) ((*(top_min_excess_))[r_initial_block]);
                        if (local_min == m) c+= (*(top_min_count_))[r_initial_block];
                        if (local_min < m) {
                            m = local_min;
                            c= (*(top_min_count_))[r_initial_block];
                        }
                        excess += decoded_excess((*top_excess_)[r_initial_block]);
                    }
                }

                //LOOKUP
                if (going_up && r_initial_block +1 == r_end_block) {
                    down_path.push(r_end_block);
                    ++ r_initial_block;
                    going_up = false;
                }

                if (going_up) {
                    r_initial_block = (r_initial_block - 1) / r_;
                    if (different_height) different_height = !different_height;
                    else {
                        down_path.push(r_end_block);
                        r_end_block = (r_end_block - 1) / r_;
                    }
                }
            } else {
                while (!down_path.empty()) {
                    int next_r_index = down_path.top();
                    down_path.pop();
                    for (int r_sibling = ((next_r_index-1)/r_)*r_ + 1; r_sibling < next_r_index; ++r_sibling) {
                        if (r_sibling >= n_internal_nodes_) {
                            if (r_sibling - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                   r_sibling -
                                                                                                   n_internal_nodes_;
                            else initial_block = r_sibling - n_pre_last_level_ - n_internal_nodes_;
                            //if (initial_block >= number_of_blocks_first_level_) break;

                            int local_min = excess + 1-(int) ((*(bt_prefix_min_excess_[0]))[initial_block]);
                            if (local_min == m) c += (*(bt_min_count_[0]))[initial_block];
                            if (local_min < m) {
                                m = local_min;
                                c = (*(bt_min_count_[0]))[initial_block];
                            }
                            int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                            excess += 2*rank_ones - first_level_length_;
                            /*
                            for (auto& pair: bt_ranks_) {
                                if (pair.first == open_) {
                                    excess += (*pair.second[0])[initial_block];
                                } else {
                                    excess -= (*pair.second[0])[initial_block];
                                }
                            }
                             */

                        } else {
                            int local_min = excess + 1-(int) ((*(top_min_excess_))[r_sibling]);
                            if (local_min == m) c+= (*(top_min_count_))[r_sibling];
                            if (local_min < m) {
                                m = local_min;
                                c= (*(top_min_count_))[r_sibling];
                            }
                            excess += decoded_excess((*top_excess_)[r_sibling]);
                        }
                    }

                }
                break;
            }
        }
        e += excess;
    }
    return c;
}

int BPSPCBlockTree::min_count(int i, int j, int level, int level_index, int level_length, int& e, int& m) {
    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) { // Not looking at padding border j condition!
        int rank_ones = 0;
        if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
        else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
        e += 2*rank_ones - (j+1);
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[level_index];
            } else {
                e -= (*pair.second[level])[level_index];
            }
        }
         */
        m = 1-(*bt_prefix_min_excess_[level])[level_index];
        return (*bt_min_count_[level])[level_index];
    }

   m = 1;
   int c = 0;
   if (level == number_of_levels_-1) { // Case LeafBlock
       int excess = 0;
       auto& leaf_string = *leaf_string_;
       int leaf_string_size = leaf_string.size();
       int k = i;
       int chunk = (level_length*level_index + k)/64;
       uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
       if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
           while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
               int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
               if (index < 0) index += 1<<bottom_bit_size_;

               if (excess + bottom_min_excess_2_[index] < m) {
                   m = excess + bottom_min_excess_2_[index];
                   c = bottom_min_count_2_[index];
               } else if (excess + bottom_min_excess_2_[index] == m) c += bottom_min_count_2_[index];
               excess += bottom_excess_2_[index];;
               k += bottom_bit_size_;
           }
       }
*/
       for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
           excess += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
           //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
           if (excess == m) ++c;
           if (excess < m) {
               m = excess;
               c = 1;
           }
           if ((level_length*level_index + k + 1)%64 == 0) {
               ++chunk;
               chunk_info = *(leaf_string.m_data+chunk);
           }
       }
       e += excess;
       return c;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int excess = 0;
        int child_length = level_length/r_;
        int initial_block = i/child_length;
        int end_block = j/child_length;

        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
         */
        if (initial_block == end_block) {
            c = min_count(i%child_length, j%child_length, level+1, rank*r_ + initial_block,child_length, excess,m);
        } else {
            c = min_count(i%child_length, child_length-1, level+1, rank*r_ + initial_block,child_length, excess,m);
            for (int block = initial_block+1; block<end_block; ++block) {
                int be = excess;
                int min = 1;
                int c2 = min_count(0, child_length - 1, level+1, rank*r_ + block, child_length, excess,min);
                if (min + be == m) c += c2;
                if (min + be < m) {
                    m = min + be;
                    c = c2;
                }
            }
            int be = excess;
            int min = 1;
            int c2 = min_count(0, j%child_length, level+1, rank*r_ + end_block, child_length, excess,min);
            if (min + be == m) c += c2;
            if (min + be < m) {
                m = min + be;
                c = c2;
            }
        }
        e += excess;
        return c;
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                m = 1-(*bt_prefix_min_excess_first_block_[level])[back_index];
                return (*bt_min_count_first_block_[level])[back_index];
            }
            c = min_count(i + offset, j + offset, level, first_block, level_length, e,m);
            return c;
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                int rank_ones = 0;
                if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                m = 1-(*bt_prefix_min_excess_second_block_[level])[back_index];
                return (*bt_min_count_second_block_[level])[back_index];
            }
            c = min_count(offset + i - level_length, offset + j - level_length, level, first_block + 1, level_length,
                           e,m);
            return c;
        } else {
            int excess = 0;
            if (i == 0) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[back_index];
                    } else {
                        excess -= (*pair.second[level])[back_index];
                    }
                }
                 */
                m = 1-(*bt_prefix_min_excess_first_block_[level])[back_index];
                c = (*bt_min_count_first_block_[level])[back_index];
            } else {
                c = min_count(i + offset, level_length - 1, level, first_block, level_length, excess,m);
            }

            if (j == level_length - 1) {
                int be = excess;
                int rank_ones = 0;
                if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                excess += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[level_index];
                    } else {
                        excess -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess -= (*pair.second[level])[back_index];
                    } else {
                        excess += (*pair.second[level])[back_index];
                    }
                }
                 */
                int mm = 1-(*bt_prefix_min_excess_second_block_[level])[back_index];
                int c2 = (*bt_min_count_second_block_[level])[back_index];
                if (mm + be == m) c += c2;
                if (mm + be < m) {
                    m = mm + be;
                    c = c2;
                }
            } else {
                int be = excess;
                int mm = 1;
                int c2 = min_count(0, offset + j - level_length, level, first_block + 1, level_length, excess,mm);
                if (mm + be == m) c += c2;
                if (mm + be < m) {
                    m = mm + be;
                    c = c2;
                }
            }
            e += excess;
            return c;
        }
    }
}



int BPSPCBlockTree::my_min_count(int i, int j, int level, int level_index, int level_length, int& e, int& m) {
    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) { // Not looking at padding border j condition!
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        e += 2*rank_ones - (j+1);
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[level_index];
            } else {
                e -= (*pair.second[level])[level_index];
            }
        }
         */
        m = 1-(*bt_prefix_min_excess_[level])[level_index];
        return (*bt_min_count_[level])[level_index];
    }

    m = 1;
    int c = 0;
    if (level == number_of_levels_-1) { // Case LeafBlock
        int excess = 0;
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;

                if (excess + bottom_min_excess_2_[index] < m) {
                    m = excess + bottom_min_excess_2_[index];
                    c = bottom_min_count_2_[index];
                } else if (excess + bottom_min_excess_2_[index] == m) c += bottom_min_count_2_[index];
                excess += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
*/
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            excess += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (excess == m) ++c;
            if (excess < m) {
                m = excess;
                c = 1;
            }
            if ((level_length*level_index + k + 1)%64 == 0) {
                ++chunk;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        e += excess;
        return c;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int excess = 0;
        int child_length = level_length/r_;
        int initial_block = i/child_length;
        int end_block = j/child_length;

        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
         */
        if (initial_block == end_block) {
            c = my_min_count(i%child_length, j%child_length, level+1, rank*r_ + initial_block,child_length, excess,m);
        } else {
            c = my_min_count(i%child_length, child_length-1, level+1, rank*r_ + initial_block,child_length, excess,m);
            for (int block = initial_block+1; block<end_block; ++block) {
                int be = excess;
                int min = 1;
                int c2 = my_min_count(0, child_length - 1, level+1, rank*r_ + block, child_length, excess,min);
                if (min + be == m) c += c2;
                if (min + be < m) {
                    m = min + be;
                    c = c2;
                }
            }
            int be = excess;
            int min = 1;
            int c2 = my_min_count(0, j%child_length, level+1, rank*r_ + end_block, child_length, excess,min);
            if (min + be == m) c += c2;
            if (min + be < m) {
                m = min + be;
                c = c2;
            }
        }
        e += excess;
        return c;
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                m = 1-(*bt_prefix_min_excess_first_block_[level])[back_index];
                return (*bt_min_count_first_block_[level])[back_index];
            }
            c = my_min_count(i + offset, j + offset, level, first_block, level_length, e,m);
            return c;
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                m = 1-(*bt_prefix_min_excess_second_block_[level])[back_index];
                return (*bt_min_count_second_block_[level])[back_index];
            }
            c = my_min_count(offset + i - level_length, offset + j - level_length, level, first_block + 1, level_length,
                          e,m);
            return c;
        } else {
            int excess = 0;
            if (i == 0) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[back_index];
                    } else {
                        excess -= (*pair.second[level])[back_index];
                    }
                }
                 */
                m = 1-(*bt_prefix_min_excess_first_block_[level])[back_index];
                c = (*bt_min_count_first_block_[level])[back_index];
            } else {
                c = my_min_count(i + offset, level_length - 1, level, first_block, level_length, excess,m);
            }

            if (j == level_length - 1) {
                int be = excess;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                excess += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[level_index];
                    } else {
                        excess -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess -= (*pair.second[level])[back_index];
                    } else {
                        excess += (*pair.second[level])[back_index];
                    }
                }
                 */
                int mm = 1-(*bt_prefix_min_excess_second_block_[level])[back_index];
                int c2 = (*bt_min_count_second_block_[level])[back_index];
                if (mm + be == m) c += c2;
                if (mm + be < m) {
                    m = mm + be;
                    c = c2;
                }
            } else {
                int be = excess;
                int mm = 1;
                int c2 = my_min_count(0, offset + j - level_length, level, first_block + 1, level_length, excess,mm);
                if (mm + be == m) c += c2;
                if (mm + be < m) {
                    m = mm + be;
                    c = c2;
                }
            }
            e += excess;
            return c;
        }
    }
}

int BPSPCBlockTree::my_min_count_nobackmin(int i, int j, int level, int level_index, int level_length, int& e, int& m) {
    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) { // Not looking at padding border j condition!
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        e += 2*rank_ones - (j+1);
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[level_index];
            } else {
                e -= (*pair.second[level])[level_index];
            }
        }
         */
        m = 1-(*bt_prefix_min_excess_[level])[level_index];
        return (*bt_min_count_[level])[level_index];
    }

    m = 1;
    int c = 0;
    if (level == number_of_levels_-1) { // Case LeafBlock
        int excess = 0;
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;

                if (excess + bottom_min_excess_2_[index] < m) {
                    m = excess + bottom_min_excess_2_[index];
                    c = bottom_min_count_2_[index];
                } else if (excess + bottom_min_excess_2_[index] == m) c += bottom_min_count_2_[index];
                excess += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
*/
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            excess += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (excess == m) ++c;
            if (excess < m) {
                m = excess;
                c = 1;
            }
            if ((level_length*level_index + k + 1)%64 == 0) {
                ++chunk;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        e += excess;
        return c;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int excess = 0;
        int child_length = level_length/r_;
        int initial_block = i/child_length;
        int end_block = j/child_length;

        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
         */
        if (initial_block == end_block) {
            c = my_min_count_nobackmin(i%child_length, j%child_length, level+1, rank*r_ + initial_block,child_length, excess,m);
        } else {
            c = my_min_count_nobackmin(i%child_length, child_length-1, level+1, rank*r_ + initial_block,child_length, excess,m);
            for (int block = initial_block+1; block<end_block; ++block) {
                int be = excess;
                int min = 1;
                int c2 = my_min_count_nobackmin(0, child_length - 1, level+1, rank*r_ + block, child_length, excess,min);
                if (min + be == m) c += c2;
                if (min + be < m) {
                    m = min + be;
                    c = c2;
                }
            }
            int be = excess;
            int min = 1;
            int c2 = my_min_count_nobackmin(0, j%child_length, level+1, rank*r_ + end_block, child_length, excess,min);
            if (min + be == m) c += c2;
            if (min + be < m) {
                m = min + be;
                c = c2;
            }
        }
        e += excess;
        return c;
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            /*
            if (i == 0 && j + offset == level_length - 1) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                */
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                /*
                m = 1-(*bt_prefix_min_excess_first_block_[level])[back_index];
                return (*bt_min_count_first_block_[level])[back_index];
            }
                 */
            c = my_min_count_nobackmin(i + offset, j + offset, level, first_block, level_length, e,m);
            return c;
        } else if (i + offset >= level_length && j + offset >= level_length) {
            /*
            if (i + offset == level_length && j == level_length - 1) {
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                e += 2*rank_ones - level_length;
                */
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                /*
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                 */
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                /*
                m = 1-(*bt_prefix_min_excess_second_block_[level])[back_index];
                return (*bt_min_count_second_block_[level])[back_index];
            }
                 */
            c = my_min_count_nobackmin(offset + i - level_length, offset + j - level_length, level, first_block + 1, level_length,
                             e,m);
            return c;
        } else {
            int excess = 0;
            /*
            if (i == 0) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess += 2*rank_ones - (level_length-offset);
                */
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[back_index];
                    } else {
                        excess -= (*pair.second[level])[back_index];
                    }
                }
                 */
                /*
                m = 1-(*bt_prefix_min_excess_first_block_[level])[back_index];
                c = (*bt_min_count_first_block_[level])[back_index];
            } else {
                 */
                c = my_min_count_nobackmin(i + offset, level_length - 1, level, first_block, level_length, excess,m);
            /*}*/
            /*
            if (j == level_length - 1) {
                int be = excess;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                excess += 2*rank_ones - level_length;
                */
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[level_index];
                    } else {
                        excess -= (*pair.second[level])[level_index];
                    }
                }
                 */
                /*
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess -= 2*rank_ones - (level_length-offset);
                 */
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess -= (*pair.second[level])[back_index];
                    } else {
                        excess += (*pair.second[level])[back_index];
                    }
                }
                 */
                /*
                int mm = 1-(*bt_prefix_min_excess_second_block_[level])[back_index];
                int c2 = (*bt_min_count_second_block_[level])[back_index];
                if (mm + be == m) c += c2;
                if (mm + be < m) {
                    m = mm + be;
                    c = c2;
                }
            } else {*/
                int be = excess;
                int mm = 1;
                int c2 = my_min_count_nobackmin(0, offset + j - level_length, level, first_block + 1, level_length, excess,mm);
                if (mm + be == m) c += c2;
                if (mm + be < m) {
                    m = mm + be;
                    c = c2;
               /* }*/
            }
            e += excess;
            return c;
        }
    }
}


int BPSPCBlockTree::next_block_min_excess(int initial_block, int end_block, int& e) {
    int m = 1;
    int excess = 0;
    for (int i = initial_block+1; i < end_block; ++i) {
        int local_min = excess + 1-(int)((*(bt_prefix_min_excess_[0]))[i]);
        if (local_min < m)
            m = local_min;
        int rank_ones = (i == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[i-1])));
        excess += 2*rank_ones - first_level_length_ ;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess += (*pair.second[0])[i];
            } else {
                excess -= (*pair.second[0])[i];
            }
        }
         */
    }
    e += excess;
    return m;
}

int BPSPCBlockTree::next_block_max_excess(int initial_block, int end_block, int& e) {
    int M = -1;
    int excess = 0;
    for (int i = initial_block+1; i < end_block; ++i) {
        int local_max = excess - 1+(int)((*(bt_prefix_max_excess_[0]))[i]);
        if (local_max > M)
            M = local_max;
        int rank_ones = (i == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[i-1])));
        excess += 2*rank_ones - first_level_length_ ;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess += (*pair.second[0])[i];
            } else {
                excess -= (*pair.second[0])[i];
            }
        }
         */
    }
    e += excess;
    return M;
}


int BPSPCBlockTree::my_next_block_max_excess(int initial_block, int end_block, int& e) {
    int M = -1;
    int excess = 0;
    for (int i = initial_block+1; i < end_block; ++i) {
        int local_max = excess - 1+(int)((*(bt_prefix_max_excess_[0]))[i]);
        if (local_max > M)
            M = local_max;
        int rank_ones = ((int)((*bt_ranks_[open_][0])[i]));
        excess += 2*rank_ones - first_level_length_ ;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess += (*pair.second[0])[i];
            } else {
                excess -= (*pair.second[0])[i];
            }
        }
         */
    }
    e += excess;
    return M;
}


int BPSPCBlockTree::my_next_block_min_excess(int initial_block, int end_block, int& e) {
    int m = 1;
    int excess = 0;
    for (int i = initial_block+1; i < end_block; ++i) {
        int local_min = excess + 1-(int)((*(bt_prefix_min_excess_[0]))[i]);
        if (local_min < m)
            m = local_min;
        int rank_ones = ((int)((*bt_ranks_[open_][0])[i]));
        excess += 2*rank_ones - first_level_length_ ;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess += (*pair.second[0])[i];
            } else {
                excess -= (*pair.second[0])[i];
            }
        }
         */
    }
    e += excess;
    return m;
}

int BPSPCBlockTree::faster_next_block_min_excess(int initial_block, int end_block, int& e) {
    int m = 1;
    if (end_block > initial_block + 1) {
        int r_initial_block = 0;
        if (initial_block < n_last_level_)
            r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
        else
            r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;

        int r_end_block = 0;
        if (end_block < n_last_level_)
            r_end_block = end_block + n_pre_last_level_ + n_internal_nodes_;
        else
            r_end_block = end_block - n_last_level_ + n_internal_nodes_;
        bool going_up = true;
        int excess = 0;
        std::stack<int> down_path;
        bool different_height =  initial_block < n_last_level_ && end_block >= n_last_level_;
        while (true) {
            if (going_up) {
                while (r_initial_block % r_ != 0) {
                    ++r_initial_block;
                    if (r_initial_block >= number_of_blocks_first_level_ + n_internal_nodes_) break;
                    if (r_initial_block == r_end_block) {
                        going_up = false;
                        break;
                    }
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                       r_initial_block -
                                                                                                       n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_ - n_internal_nodes_;;
                        if (initial_block >= number_of_blocks_first_level_) break;

                        int local_min = excess + 1-(int) ((*(bt_prefix_min_excess_[0]))[initial_block]);
                        if (local_min < m) {
                            m = local_min;
                        }
                        int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                        excess += 2*rank_ones - first_level_length_;
                        /*
                        for (auto& pair: bt_ranks_) {
                            if (pair.first == open_) {
                                excess += (*pair.second[0])[initial_block];
                            } else {
                                excess -= (*pair.second[0])[initial_block];
                            }
                        }
                         */

                    } else {
                        int local_min = excess + 1-(int) ((*(top_min_excess_))[r_initial_block]);
                        if (local_min < m) {
                            m = local_min;
                        }
                        excess += decoded_excess((*top_excess_)[r_initial_block]);
                    }
                }

                //LOOKUP
                if (going_up && r_initial_block +1 == r_end_block) {
                    down_path.push(r_end_block);
                    ++ r_initial_block;
                    going_up = false;
                }
                if (going_up) {
                    r_initial_block = (r_initial_block - 1) / r_;
                    if (different_height) different_height = !different_height;
                    else {
                        down_path.push(r_end_block);
                        r_end_block = (r_end_block - 1) / r_;
                    }
                }
            } else {
                while (!down_path.empty()) {
                    int next_r_index = down_path.top();
                    down_path.pop();
                    for (int r_sibling = ((next_r_index-1)/r_)*r_ + 1; r_sibling < next_r_index; ++r_sibling) {
                        if (r_sibling >= n_internal_nodes_) {
                            if (r_sibling - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                     r_sibling -
                                                                                                     n_internal_nodes_;
                            else initial_block = r_sibling - n_pre_last_level_ - n_internal_nodes_;
                            //if (initial_block >= number_of_blocks_first_level_) break;

                            int local_min = excess + 1-(int) ((*(bt_prefix_min_excess_[0]))[initial_block]);
                            if (local_min < m) {
                                m = local_min;
                            }
                            int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                            excess += 2*rank_ones - first_level_length_;
                            /*
                            for (auto& pair: bt_ranks_) {
                                if (pair.first == open_) {
                                    excess += (*pair.second[0])[initial_block];
                                } else {
                                    excess -= (*pair.second[0])[initial_block];
                                }
                            }
                                */
                        } else {
                            int local_min = excess + 1-(int) ((*(top_min_excess_))[r_sibling]);
                            if (local_min < m) {
                                m = local_min;
                            }
                            excess += decoded_excess((*top_excess_)[r_sibling]);
                        }
                    }

                }
                break;
            }
        }
        e += excess;
    }
    return m;
}



int BPSPCBlockTree::faster_next_block_max_excess(int initial_block, int end_block, int& e) {
    int M = -1;
    if (end_block > initial_block + 1) {
        int r_initial_block = 0;
        if (initial_block < n_last_level_)
            r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
        else
            r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;

        int r_end_block = 0;
        if (end_block < n_last_level_)
            r_end_block = end_block + n_pre_last_level_ + n_internal_nodes_;
        else
            r_end_block = end_block - n_last_level_ + n_internal_nodes_;
        bool going_up = true;
        int excess = 0;
        std::stack<int> down_path;
        bool different_height =  initial_block < n_last_level_ && end_block >= n_last_level_;
        while (true) {
            if (going_up) {
                while (r_initial_block % r_ != 0) {
                    ++r_initial_block;
                    if (r_initial_block >= number_of_blocks_first_level_ + n_internal_nodes_) break;
                    if (r_initial_block == r_end_block) {
                        going_up = false;
                        break;
                    }
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                     r_initial_block -
                                                                                                     n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_ - n_internal_nodes_;;
                        if (initial_block >= number_of_blocks_first_level_) break;

                        int local_max = excess - 1+(int) ((*(bt_prefix_max_excess_[0]))[initial_block]);
                        if (local_max > M) {
                            M = local_max;
                        }
                        int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                        excess += 2*rank_ones - first_level_length_;
                        /*
                        for (auto& pair: bt_ranks_) {
                            if (pair.first == open_) {
                                excess += (*pair.second[0])[initial_block];
                            } else {
                                excess -= (*pair.second[0])[initial_block];
                            }
                        }
                         */

                    } else {
                        int local_max = excess - 1+(int) ((*(top_max_excess_))[r_initial_block]);
                        if (local_max > M) {
                            M = local_max;
                        }
                        excess += decoded_excess((*top_excess_)[r_initial_block]);
                    }
                }

                //LOOKUP
                if (going_up && r_initial_block +1 == r_end_block) {
                    down_path.push(r_end_block);
                    ++ r_initial_block;
                    going_up = false;
                }
                if (going_up) {
                    r_initial_block = (r_initial_block - 1) / r_;
                    if (different_height) different_height = !different_height;
                    else {
                        down_path.push(r_end_block);
                        r_end_block = (r_end_block - 1) / r_;
                    }
                }
            } else {
                while (!down_path.empty()) {
                    int next_r_index = down_path.top();
                    down_path.pop();
                    for (int r_sibling = ((next_r_index-1)/r_)*r_ + 1; r_sibling < next_r_index; ++r_sibling) {
                        if (r_sibling >= n_internal_nodes_) {
                            if (r_sibling - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                   r_sibling -
                                                                                                   n_internal_nodes_;
                            else initial_block = r_sibling - n_pre_last_level_ - n_internal_nodes_;
                            //if (initial_block >= number_of_blocks_first_level_) break;

                            int local_max = excess - 1+(int) ((*(bt_prefix_max_excess_[0]))[initial_block]);
                            if (local_max > M) {
                                M = local_max;
                            }
                            int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                            excess += 2*rank_ones - first_level_length_;
                            /*
                            for (auto& pair: bt_ranks_) {
                                if (pair.first == open_) {
                                    excess += (*pair.second[0])[initial_block];
                                } else {
                                    excess -= (*pair.second[0])[initial_block];
                                }
                            }
                                */
                        } else {
                            int local_max = excess - 1+(int) ((*(top_max_excess_))[r_sibling]);
                            if (local_max > M) {
                                M = local_max;
                            }
                            excess += decoded_excess((*top_excess_)[r_sibling]);
                        }
                    }

                }
                break;
            }
        }
        e += excess;
    }
    return M;
}

int BPSPCBlockTree::my_faster_next_block_max_excess(int initial_block, int end_block, int& e) {
    int M = -1;
    if (end_block > initial_block + 1) {
        int r_initial_block = 0;
        if (initial_block < n_last_level_)
            r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
        else
            r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;

        int r_end_block = 0;
        if (end_block < n_last_level_)
            r_end_block = end_block + n_pre_last_level_ + n_internal_nodes_;
        else
            r_end_block = end_block - n_last_level_ + n_internal_nodes_;
        bool going_up = true;
        int excess = 0;
        std::stack<int> down_path;
        bool different_height =  initial_block < n_last_level_ && end_block >= n_last_level_;
        while (true) {
            if (going_up) {
                while (r_initial_block % r_ != 0) {
                    ++r_initial_block;
                    if (r_initial_block >= number_of_blocks_first_level_ + n_internal_nodes_) break;
                    if (r_initial_block == r_end_block) {
                        going_up = false;
                        break;
                    }
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                     r_initial_block -
                                                                                                     n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_ - n_internal_nodes_;;
                        if (initial_block >= number_of_blocks_first_level_) break;

                        int local_max = excess - 1+(int) ((*(bt_prefix_max_excess_[0]))[initial_block]);
                        if (local_max > M) {
                            M = local_max;
                        }
                        int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                        excess += 2*rank_ones - first_level_length_;
                        /*
                        for (auto& pair: bt_ranks_) {
                            if (pair.first == open_) {
                                excess += (*pair.second[0])[initial_block];
                            } else {
                                excess -= (*pair.second[0])[initial_block];
                            }
                        }
                         */

                    } else {
                        int local_max = excess - 1+(int) ((*(top_max_excess_))[r_initial_block]);
                        if (local_max > M) {
                            M = local_max;
                        }
                        excess += decoded_excess((*top_excess_)[r_initial_block]);
                    }
                }

                //LOOKUP
                if (going_up && r_initial_block +1 == r_end_block) {
                    down_path.push(r_end_block);
                    ++ r_initial_block;
                    going_up = false;
                }
                if (going_up) {
                    r_initial_block = (r_initial_block - 1) / r_;
                    if (different_height) different_height = !different_height;
                    else {
                        down_path.push(r_end_block);
                        r_end_block = (r_end_block - 1) / r_;
                    }
                }
            } else {
                while (!down_path.empty()) {
                    int next_r_index = down_path.top();
                    down_path.pop();
                    for (int r_sibling = ((next_r_index-1)/r_)*r_ + 1; r_sibling < next_r_index; ++r_sibling) {
                        if (r_sibling >= n_internal_nodes_) {
                            if (r_sibling - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                   r_sibling -
                                                                                                   n_internal_nodes_;
                            else initial_block = r_sibling - n_pre_last_level_ - n_internal_nodes_;
                            //if (initial_block >= number_of_blocks_first_level_) break;

                            int local_max = excess - 1+(int) ((*(bt_prefix_max_excess_[0]))[initial_block]);
                            if (local_max > M) {
                                M = local_max;
                            }
                            int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                            excess += 2*rank_ones - first_level_length_;
                            /*
                            for (auto& pair: bt_ranks_) {
                                if (pair.first == open_) {
                                    excess += (*pair.second[0])[initial_block];
                                } else {
                                    excess -= (*pair.second[0])[initial_block];
                                }
                            }
                                */
                        } else {
                            int local_max = excess - 1+(int) ((*(top_max_excess_))[r_sibling]);
                            if (local_max > M) {
                                M = local_max;
                            }
                            excess += decoded_excess((*top_excess_)[r_sibling]);
                        }
                    }

                }
                break;
            }
        }
        e += excess;
    }
    return M;
}


int BPSPCBlockTree::my_faster_next_block_min_excess(int initial_block, int end_block, int& e) {
    int m = 1;
    if (end_block > initial_block + 1) {
        int r_initial_block = 0;
        if (initial_block < n_last_level_)
            r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
        else
            r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;

        int r_end_block = 0;
        if (end_block < n_last_level_)
            r_end_block = end_block + n_pre_last_level_ + n_internal_nodes_;
        else
            r_end_block = end_block - n_last_level_ + n_internal_nodes_;
        bool going_up = true;
        int excess = 0;
        std::stack<int> down_path;
        bool different_height =  initial_block < n_last_level_ && end_block >= n_last_level_;
        while (true) {
            if (going_up) {
                while (r_initial_block % r_ != 0) {
                    ++r_initial_block;
                    if (r_initial_block >= number_of_blocks_first_level_ + n_internal_nodes_) break;
                    if (r_initial_block == r_end_block) {
                        going_up = false;
                        break;
                    }
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                     r_initial_block -
                                                                                                     n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_ - n_internal_nodes_;;
                        if (initial_block >= number_of_blocks_first_level_) break;

                        int local_min = excess + 1-(int) ((*(bt_prefix_min_excess_[0]))[initial_block]);
                        if (local_min < m) {
                            m = local_min;
                        }
                        int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                        excess += 2*rank_ones - first_level_length_;
                        /*
                        for (auto& pair: bt_ranks_) {
                            if (pair.first == open_) {
                                excess += (*pair.second[0])[initial_block];
                            } else {
                                excess -= (*pair.second[0])[initial_block];
                            }
                        }
                         */

                    } else {
                        int local_min = excess + 1-(int) ((*(top_min_excess_))[r_initial_block]);
                        if (local_min < m) {
                            m = local_min;
                        }
                        excess += decoded_excess((*top_excess_)[r_initial_block]);
                    }
                }

                //LOOKUP
                if (going_up && r_initial_block +1 == r_end_block) {
                    down_path.push(r_end_block);
                    ++ r_initial_block;
                    going_up = false;
                }
                if (going_up) {
                    r_initial_block = (r_initial_block - 1) / r_;
                    if (different_height) different_height = !different_height;
                    else {
                        down_path.push(r_end_block);
                        r_end_block = (r_end_block - 1) / r_;
                    }
                }
            } else {
                while (!down_path.empty()) {
                    int next_r_index = down_path.top();
                    down_path.pop();
                    for (int r_sibling = ((next_r_index-1)/r_)*r_ + 1; r_sibling < next_r_index; ++r_sibling) {
                        if (r_sibling >= n_internal_nodes_) {
                            if (r_sibling - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                   r_sibling -
                                                                                                   n_internal_nodes_;
                            else initial_block = r_sibling - n_pre_last_level_ - n_internal_nodes_;
                            //if (initial_block >= number_of_blocks_first_level_) break;

                            int local_min = excess + 1-(int) ((*(bt_prefix_min_excess_[0]))[initial_block]);
                            if (local_min < m) {
                                m = local_min;
                            }
                            int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                            excess += 2*rank_ones - first_level_length_;
                            /*
                            for (auto& pair: bt_ranks_) {
                                if (pair.first == open_) {
                                    excess += (*pair.second[0])[initial_block];
                                } else {
                                    excess -= (*pair.second[0])[initial_block];
                                }
                            }
                                */
                        } else {
                            int local_min = excess + 1-(int) ((*(top_min_excess_))[r_sibling]);
                            if (local_min < m) {
                                m = local_min;
                            }
                            excess += decoded_excess((*top_excess_)[r_sibling]);
                        }
                    }

                }
                break;
            }
        }
        e += excess;
    }
    return m;
}

int BPSPCBlockTree::min_excess(int i, int j, int level, int level_index, int level_length, int& e) {
    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) { // Not looking at padding border j condition!
        int rank_ones = 0;
        if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
        else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
        e += 2*rank_ones - (j+1);
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[level_index];
            } else {
                e -= (*pair.second[level])[level_index];
            }
        }
         */
        return 1-(*bt_prefix_min_excess_[level])[level_index];
    }

    int m = 1;
    if (level == number_of_levels_-1) { // Case LeafBlock
        int excess = 0;
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;

                if (excess + bottom_min_excess_2_[index] < m) {
                    m = excess + bottom_min_excess_2_[index];
                }
                excess += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
*/
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            excess += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (excess < m) m = excess;
            if ((level_length*level_index + k + 1)%64 == 0) {
                chunk++;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        e += excess;
        return m;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int excess = 0;
        int child_length = level_length/r_;
        int initial_block = i/child_length;
        int end_block = j/child_length;

        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
         */
        if (initial_block == end_block) {
            m = min_excess(i%child_length, j%child_length, level+1, rank*r_ + initial_block,child_length, excess);
        } else {
            m = min_excess(i%child_length, child_length-1, level+1, rank*r_ + initial_block,child_length, excess);
            for (int block = initial_block+1; block<end_block; ++block) {
                int be = excess;
                int mm = min_excess(0, child_length - 1, level+1, rank*r_ + block, child_length, excess);
                if (mm + be < m) m = mm + be;
            }
            int be = excess;
            int mm = min_excess(0, j%child_length, level+1, rank*r_ + end_block, child_length, excess);
            if (mm + be< m) m = mm + be;
        }
        e += excess;
        return m;
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                return 1-(*bt_prefix_min_excess_first_block_[level])[back_index];
            }
            m = min_excess(i + offset, j + offset, level, first_block, level_length, e);
            return m;
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                int rank_ones = 0;
                if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                return 1-(*bt_prefix_min_excess_second_block_[level])[back_index];
            }
            m = min_excess(offset + i - level_length, offset + j - level_length, level, first_block + 1, level_length,
                          e);
            return m;
        } else {
            int excess = 0;
            if (i == 0) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[back_index];
                    } else {
                        excess -= (*pair.second[level])[back_index];
                    }
                }
                 */
                m = 1-(*bt_prefix_min_excess_first_block_[level])[back_index];
            } else {
                m = min_excess(i + offset, level_length - 1, level, first_block, level_length, excess);
            }

            if (j == level_length - 1) {
                int be = excess;
                int rank_ones = 0;
                if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                excess += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[level_index];
                    } else {
                        excess -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess -= (*pair.second[level])[back_index];
                    } else {
                        excess += (*pair.second[level])[back_index];
                    }
                }
                 */
                int mm = 1-(*bt_prefix_min_excess_second_block_[level])[back_index];
                if (mm + be < m) m = mm + be;
            } else {
                int be = excess;
                int mm = min_excess(0, offset + j - level_length, level, first_block + 1, level_length, excess);
                if (mm + be < m) m = mm + be;
            }
            e += excess;
            return m;
        }
    }
}


int BPSPCBlockTree::max_excess(int i, int j, int level, int level_index, int level_length, int& e) {
    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_max_excess_[level]).size()-1)))) { // Not looking at padding border j condition!
        int rank_ones = 0;
        if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
        else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
        e += 2*rank_ones - (j+1);
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[level_index];
            } else {
                e -= (*pair.second[level])[level_index];
            }
        }
         */
        return -1+(*bt_prefix_max_excess_[level])[level_index];
    }

    int M = -1;
    if (level == number_of_levels_-1) { // Case LeafBlock
        int excess = 0;
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;

                if (excess + bottom_max_excess_2_[index] > M) {
                    M = excess + bottom_max_excess_2_[index];
                }
                excess += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
*/
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            excess += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (excess > M) M = excess;
            if ((level_length*level_index + k + 1)%64 == 0) {
                ++chunk;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        e += excess;
        return M;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int excess = 0;
        int child_length = level_length/r_;
        int initial_block = i/child_length;
        int end_block = j/child_length;

        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
         */
        if (initial_block == end_block) {
            M = max_excess(i%child_length, j%child_length, level+1, rank*r_ + initial_block,child_length, excess);
        } else {
            M = max_excess(i%child_length, child_length-1, level+1, rank*r_ + initial_block,child_length, excess);
            for (int block = initial_block+1; block<end_block; ++block) {
                int be = excess;
                int MM = max_excess(0, child_length - 1, level+1, rank*r_ + block, child_length, excess);
                if (MM + be > M) M = MM + be;
            }
            int be = excess;
            int MM = max_excess(0, j%child_length, level+1, rank*r_ + end_block, child_length, excess);
            if (MM + be > M) M = MM + be;
        }
        e += excess;
        return M;
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                return -1+(*bt_prefix_max_excess_first_block_[level])[back_index];
            }
            M = max_excess(i + offset, j + offset, level, first_block, level_length, e);
            return M;
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                int rank_ones = 0;
                if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                return -1+(*bt_prefix_max_excess_second_block_[level])[back_index];
            }
            M = max_excess(offset + i - level_length, offset + j - level_length, level, first_block + 1, level_length,
                           e);
            return M;
        } else {
            int excess = 0;
            if (i == 0) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[back_index];
                    } else {
                        excess -= (*pair.second[level])[back_index];
                    }
                }
                 */
                M = -1+(*bt_prefix_max_excess_first_block_[level])[back_index];
            } else {
                M = max_excess(i + offset, level_length - 1, level, first_block, level_length, excess);
            }

            if (j == level_length - 1) {
                int be = excess;
                int rank_ones = 0;
                if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                excess += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[level_index];
                    } else {
                        excess -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess -= (*pair.second[level])[back_index];
                    } else {
                        excess += (*pair.second[level])[back_index];
                    }
                }
                 */
                int MM = -1+(*bt_prefix_max_excess_second_block_[level])[back_index];
                if (MM + be > M) M = MM + be;
            } else {
                int be = excess;
                int MM = max_excess(0, offset + j - level_length, level, first_block + 1, level_length, excess);
                if (MM + be > M) M = MM + be;
            }
            e += excess;
            return M;
        }
    }
}



int BPSPCBlockTree::my_max_excess(int i, int j, int level, int level_index, int level_length, int& e) {
    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_max_excess_[level]).size()-1)))) { // Not looking at padding border j condition!
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        e += 2*rank_ones - (j+1);
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[level_index];
            } else {
                e -= (*pair.second[level])[level_index];
            }
        }
         */
        return -1+(*bt_prefix_max_excess_[level])[level_index];
    }

    int M = -1;
    if (level == number_of_levels_-1) { // Case LeafBlock
        int excess = 0;
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;

                if (excess + bottom_max_excess_2_[index] > M) {
                    M = excess + bottom_max_excess_2_[index];
                }
                excess += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
*/
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            excess += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (excess > M) M = excess;
            if ((level_length*level_index + k + 1)%64 == 0) {
                chunk++;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        e += excess;
        return M;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int excess = 0;
        int child_length = level_length/r_;
        int initial_block = i/child_length;
        int end_block = j/child_length;

        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
         */
        if (initial_block == end_block) {
            M = my_max_excess(i%child_length, j%child_length, level+1, rank*r_ + initial_block,child_length, excess);
        } else {
            M = my_max_excess(i%child_length, child_length-1, level+1, rank*r_ + initial_block,child_length, excess);
            for (int block = initial_block+1; block<end_block; ++block) {
                int be = excess;
                int MM = my_max_excess(0, child_length - 1, level+1, rank*r_ + block, child_length, excess);
                if (MM + be > M) M = MM + be;
            }
            int be = excess;
            int MM = my_max_excess(0, j%child_length, level+1, rank*r_ + end_block, child_length, excess);
            if (MM + be > M) M = MM + be;
        }
        e += excess;
        return M;
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                return -1+(*bt_prefix_max_excess_first_block_[level])[back_index];
            }
            M = my_max_excess(i + offset, j + offset, level, first_block, level_length, e);
            return M;
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                return -1+(*bt_prefix_max_excess_second_block_[level])[back_index];
            }
            M = my_max_excess(offset + i - level_length, offset + j - level_length, level, first_block + 1, level_length,
                           e);
            return M;
        } else {
            int excess = 0;
            if (i == 0) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[back_index];
                    } else {
                        excess -= (*pair.second[level])[back_index];
                    }
                }
                 */
                M = -1+(*bt_prefix_max_excess_first_block_[level])[back_index];
            } else {
                M = my_max_excess(i + offset, level_length - 1, level, first_block, level_length, excess);
            }

            if (j == level_length - 1) {
                int be = excess;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                excess += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[level_index];
                    } else {
                        excess -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess -= (*pair.second[level])[back_index];
                    } else {
                        excess += (*pair.second[level])[back_index];
                    }
                }
                 */
                int MM = -1+(*bt_prefix_max_excess_second_block_[level])[back_index];
                if (MM + be > M) M = MM + be;
            } else {
                int be = excess;
                int MM = my_max_excess(0, offset + j - level_length, level, first_block + 1, level_length, excess);
                if (MM + be > M) M = MM + be;
            }
            e += excess;
            return M;
        }
    }
}

int BPSPCBlockTree::my_max_excess_nobackmax(int i, int j, int level, int level_index, int level_length, int& e) {
    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_max_excess_[level]).size()-1)))) { // Not looking at padding border j condition!
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        e += 2*rank_ones - (j+1);
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[level_index];
            } else {
                e -= (*pair.second[level])[level_index];
            }
        }
         */
        return -1+(*bt_prefix_max_excess_[level])[level_index];
    }

    int M = -1;
    if (level == number_of_levels_-1) { // Case LeafBlock
        int excess = 0;
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;

                if (excess + bottom_max_excess_2_[index] > M) {
                    M = excess + bottom_max_excess_2_[index];
                }
                excess += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
*/
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            excess += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (excess > M) M = excess;
            if ((level_length*level_index + k + 1)%64 == 0) {
                chunk++;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        e += excess;
        return M;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int excess = 0;
        int child_length = level_length/r_;
        int initial_block = i/child_length;
        int end_block = j/child_length;

        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
         */
        if (initial_block == end_block) {
            M = my_max_excess_nobackmax(i%child_length, j%child_length, level+1, rank*r_ + initial_block,child_length, excess);
        } else {
            M = my_max_excess_nobackmax(i%child_length, child_length-1, level+1, rank*r_ + initial_block,child_length, excess);
            for (int block = initial_block+1; block<end_block; ++block) {
                int be = excess;
                int MM = my_max_excess_nobackmax(0, child_length - 1, level+1, rank*r_ + block, child_length, excess);
                if (MM + be > M) M = MM + be;
            }
            int be = excess;
            int MM = my_max_excess_nobackmax(0, j%child_length, level+1, rank*r_ + end_block, child_length, excess);
            if (MM + be > M) M = MM + be;
        }
        e += excess;
        return M;
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            /*
            if (i == 0 && j + offset == level_length - 1) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                */
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                /*
                return -1+(*bt_prefix_max_excess_first_block_[level])[back_index];
            }
                 */
            M = my_max_excess_nobackmax(i + offset, j + offset, level, first_block, level_length, e);
            return M;
        } else if (i + offset >= level_length && j + offset >= level_length) {
            /*
            if (i + offset == level_length && j == level_length - 1) {
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                e += 2*rank_ones - level_length;
                */
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                /*
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                 */
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                /*
                return -1+(*bt_prefix_max_excess_second_block_[level])[back_index];
            }
                 */
            M = my_max_excess_nobackmax(offset + i - level_length, offset + j - level_length, level, first_block + 1, level_length,
                              e);
            return M;
        } else {
            int excess = 0;
            /*
            if (i == 0) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess += 2*rank_ones - (level_length-offset);
                */
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[back_index];
                    } else {
                        excess -= (*pair.second[level])[back_index];
                    }
                }
                 */
                /*
                M = -1+(*bt_prefix_max_excess_first_block_[level])[back_index];
            } else {
                 */
                M = my_max_excess_nobackmax(i + offset, level_length - 1, level, first_block, level_length, excess);
            /*}*/

            /*
            if (j == level_length - 1) {
                int be = excess;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                excess += 2*rank_ones - level_length;
                */
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[level_index];
                    } else {
                        excess -= (*pair.second[level])[level_index];
                    }
                }
                 */
                /*
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess -= 2*rank_ones - (level_length-offset);
                 */
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess -= (*pair.second[level])[back_index];
                    } else {
                        excess += (*pair.second[level])[back_index];
                    }
                }
                 */
                /*
                int MM = -1+(*bt_prefix_max_excess_second_block_[level])[back_index];
                if (MM + be > M) M = MM + be;
            } else {
                 */
                int be = excess;
                int MM = my_max_excess_nobackmax(0, offset + j - level_length, level, first_block + 1, level_length, excess);
                if (MM + be > M) M = MM + be;
            /*}*/
            e += excess;
            return M;
        }
    }
}


int BPSPCBlockTree::my_min_excess(int i, int j, int level, int level_index, int level_length, int& e) {
    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) { // Not looking at padding border j condition!
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        e += 2*rank_ones - (j+1);
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[level_index];
            } else {
                e -= (*pair.second[level])[level_index];
            }
        }
         */
        return 1-(*bt_prefix_min_excess_[level])[level_index];
    }

    int m = 1;
    if (level == number_of_levels_-1) { // Case LeafBlock
        int excess = 0;
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;

                if (excess + bottom_min_excess_2_[index] < m) {
                    m = excess + bottom_min_excess_2_[index];
                }
                excess += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
*/
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            excess += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (excess < m) m = excess;
            if ((level_length*level_index + k + 1)%64 == 0) {
                ++chunk;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        e += excess;
        return m;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int excess = 0;
        int child_length = level_length/r_;
        int initial_block = i/child_length;
        int end_block = j/child_length;

        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
         */
        if (initial_block == end_block) {
            m = my_min_excess(i%child_length, j%child_length, level+1, rank*r_ + initial_block,child_length, excess);
        } else {
            m = my_min_excess(i%child_length, child_length-1, level+1, rank*r_ + initial_block,child_length, excess);
            for (int block = initial_block+1; block<end_block; ++block) {
                int be = excess;
                int mm = my_min_excess(0, child_length - 1, level+1, rank*r_ + block, child_length, excess);
                if (mm + be < m) m = mm + be;
            }
            int be = excess;
            int mm = my_min_excess(0, j%child_length, level+1, rank*r_ + end_block, child_length, excess);
            if (mm + be< m) m = mm + be;
        }
        e += excess;
        return m;
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                return 1-(*bt_prefix_min_excess_first_block_[level])[back_index];
            }
            m = my_min_excess(i + offset, j + offset, level, first_block, level_length, e);
            return m;
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                return 1-(*bt_prefix_min_excess_second_block_[level])[back_index];
            }
            m = my_min_excess(offset + i - level_length, offset + j - level_length, level, first_block + 1, level_length,
                           e);
            return m;
        } else {
            int excess = 0;
            if (i == 0) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[back_index];
                    } else {
                        excess -= (*pair.second[level])[back_index];
                    }
                }
                 */
                m = 1-(*bt_prefix_min_excess_first_block_[level])[back_index];
            } else {
                m = my_min_excess(i + offset, level_length - 1, level, first_block, level_length, excess);
            }

            if (j == level_length - 1) {
                int be = excess;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                excess += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[level_index];
                    } else {
                        excess -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess -= (*pair.second[level])[back_index];
                    } else {
                        excess += (*pair.second[level])[back_index];
                    }
                }
                 */
                int mm = 1-(*bt_prefix_min_excess_second_block_[level])[back_index];
                if (mm + be < m) m = mm + be;
            } else {
                int be = excess;
                int mm = my_min_excess(0, offset + j - level_length, level, first_block + 1, level_length, excess);
                if (mm + be < m) m = mm + be;
            }
            e += excess;
            return m;
        }
    }
}


int BPSPCBlockTree::my_min_excess_cback(int i, int j, int level, int level_index, int level_length, int& e) {
    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) { // Not looking at padding border j condition!
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        e += 2*rank_ones - (j+1);
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[level_index];
            } else {
                e -= (*pair.second[level])[level_index];
            }
        }
         */
        return 1-(*bt_prefix_min_excess_[level])[level_index];
    }

    int m = 1;
    if (level == number_of_levels_-1) { // Case LeafBlock
        int excess = 0;
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;

                if (excess + bottom_min_excess_2_[index] < m) {
                    m = excess + bottom_min_excess_2_[index];
                }
                excess += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
*/
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            excess += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (excess < m) m = excess;
            if ((level_length*level_index + k + 1)%64 == 0) {
                ++chunk;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        e += excess;
        return m;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int excess = 0;
        int child_length = level_length/r_;
        int initial_block = i/child_length;
        int end_block = j/child_length;

        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
         */
        if (initial_block == end_block) {
            m = my_min_excess_cback(i%child_length, j%child_length, level+1, rank*r_ + initial_block,child_length, excess);
        } else {
            m = my_min_excess_cback(i%child_length, child_length-1, level+1, rank*r_ + initial_block,child_length, excess);
            for (int block = initial_block+1; block<end_block; ++block) {
                int be = excess;
                int mm = my_min_excess_cback(0, child_length - 1, level+1, rank*r_ + block, child_length, excess);
                if (mm + be < m) m = mm + be;
            }
            int be = excess;
            int mm = my_min_excess_cback(0, j%child_length, level+1, rank*r_ + end_block, child_length, excess);
            if (mm + be< m) m = mm + be;
        }
        e += excess;
        return m;
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                int min_prefix_first_block = 1-((*(bt_prefix_min_excess_back_block_[level]))[back_index]);
                if (((*(bt_min_in_first_block_[level]))[back_index]) || ((*(bt_min_in_both_blocks_[level]))[back_index])) {
                    min_prefix_first_block = (1-((*(bt_prefix_min_excess_[level]))[level_index]));
                }
                return min_prefix_first_block;
            }
            m = my_min_excess_cback(i + offset, j + offset, level, first_block, level_length, e);
            return m;
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                int min_prefix_second_block = 1-((*(bt_prefix_min_excess_back_block_[level]))[back_index]);
                if (!((*(bt_min_in_first_block_[level]))[back_index]) && !((*(bt_min_in_both_blocks_[level]))[back_index])) {
                    int first_block_excess = 0;
                    //int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    first_block_excess += 2*rank_ones - (level_length-offset);
                    min_prefix_second_block = (1-((*(bt_prefix_min_excess_[level]))[level_index])) - first_block_excess;
                }
                return min_prefix_second_block;
            }
            m = my_min_excess_cback(offset + i - level_length, offset + j - level_length, level, first_block + 1, level_length,
                              e);
            return m;
        } else {
            int excess = 0;
            if (i == 0) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[back_index];
                    } else {
                        excess -= (*pair.second[level])[back_index];
                    }
                }
                 */
                int min_prefix_first_block = 1-((*(bt_prefix_min_excess_back_block_[level]))[back_index]);
                if (((*(bt_min_in_first_block_[level]))[back_index]) || ((*(bt_min_in_both_blocks_[level]))[back_index])) {
                    min_prefix_first_block = (1-((*(bt_prefix_min_excess_[level]))[level_index]));
                }
                m = min_prefix_first_block;
            } else {
                m = my_min_excess_cback(i + offset, level_length - 1, level, first_block, level_length, excess);
            }

            if (j == level_length - 1) {
                int be = excess;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                excess += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[level_index];
                    } else {
                        excess -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess -= (*pair.second[level])[back_index];
                    } else {
                        excess += (*pair.second[level])[back_index];
                    }
                }
                 */
                int min_prefix_second_block = 1-((*(bt_prefix_min_excess_back_block_[level]))[back_index]);
                if (!((*(bt_min_in_first_block_[level]))[back_index]) && !((*(bt_min_in_both_blocks_[level]))[back_index])) {
                    int first_block_excess = 0;
                    //int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    first_block_excess += 2*rank_ones - (level_length-offset);
                    min_prefix_second_block = (1-((*(bt_prefix_min_excess_[level]))[level_index])) - first_block_excess;
                }
                int mm = min_prefix_second_block;
                if (mm + be < m) m = mm + be;
            } else {
                int be = excess;
                int mm = my_min_excess_cback(0, offset + j - level_length, level, first_block + 1, level_length, excess);
                if (mm + be < m) m = mm + be;
            }
            e += excess;
            return m;
        }
    }
}


int BPSPCBlockTree::my_min_excess_nobackmin(int i, int j, int level, int level_index, int level_length, int& e) {
    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) { // Not looking at padding border j condition!
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        e += 2*rank_ones - (j+1);
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[level_index];
            } else {
                e -= (*pair.second[level])[level_index];
            }
        }
         */
        return 1-(*bt_prefix_min_excess_[level])[level_index];
    }

    int m = 1;
    if (level == number_of_levels_-1) { // Case LeafBlock
        int excess = 0;
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;

                if (excess + bottom_min_excess_2_[index] < m) {
                    m = excess + bottom_min_excess_2_[index];
                }
                excess += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
*/
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            excess += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (excess < m) m = excess;
            if ((level_length*level_index + k + 1)%64 == 0) {
                ++chunk;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        e += excess;
        return m;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int excess = 0;
        int child_length = level_length/r_;
        int initial_block = i/child_length;
        int end_block = j/child_length;

        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
         */
        if (initial_block == end_block) {
            m = my_min_excess_nobackmin(i%child_length, j%child_length, level+1, rank*r_ + initial_block,child_length, excess);
        } else {
            m = my_min_excess_nobackmin(i%child_length, child_length-1, level+1, rank*r_ + initial_block,child_length, excess);
            for (int block = initial_block+1; block<end_block; ++block) {
                int be = excess;
                int mm = my_min_excess_nobackmin(0, child_length - 1, level+1, rank*r_ + block, child_length, excess);
                if (mm + be < m) m = mm + be;
            }
            int be = excess;
            int mm = my_min_excess_nobackmin(0, j%child_length, level+1, rank*r_ + end_block, child_length, excess);
            if (mm + be< m) m = mm + be;
        }
        e += excess;
        return m;
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            /*
            if (i == 0 && j + offset == level_length - 1) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                */
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                /*
                return 1-(*bt_prefix_min_excess_first_block_[level])[back_index];
            }
                 */
            m = my_min_excess_nobackmin(i + offset, j + offset, level, first_block, level_length, e);
            return m;
        } else if (i + offset >= level_length && j + offset >= level_length) {
            /*
            if (i + offset == level_length && j == level_length - 1) {
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                e += 2*rank_ones - level_length;
                */
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                /*
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                 */
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                /*
                return 1-(*bt_prefix_min_excess_second_block_[level])[back_index];
            }*/
            m = my_min_excess_nobackmin(offset + i - level_length, offset + j - level_length, level, first_block + 1, level_length,
                              e);
            return m;
        } else {
            int excess = 0;
            /*
            if (i == 0) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess += 2*rank_ones - (level_length-offset);
                */
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[back_index];
                    } else {
                        excess -= (*pair.second[level])[back_index];
                    }
                }
                 */
                /*
                m = 1-(*bt_prefix_min_excess_first_block_[level])[back_index];
            } else {*/
                m = my_min_excess_nobackmin(i + offset, level_length - 1, level, first_block, level_length, excess);
            /*}*/

            /*
            if (j == level_length - 1) {
                int be = excess;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                excess += 2*rank_ones - level_length;
                */
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        excess += (*pair.second[level])[level_index];
                    } else {
                        excess -= (*pair.second[level])[level_index];
                    }
                }
                 */
                /*
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                excess -= 2*rank_ones - (level_length-offset);
                 */
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        excess -= (*pair.second[level])[back_index];
                    } else {
                        excess += (*pair.second[level])[back_index];
                    }
                }
                 */
                /*
                int mm = 1-(*bt_prefix_min_excess_second_block_[level])[back_index];
                if (mm + be < m) m = mm + be;
            } else {
                 */
                int be = excess;
                int mm = my_min_excess_nobackmin(0, offset + j - level_length, level, first_block + 1, level_length, excess);
                if (mm + be < m) m = mm + be;
            /*}*/
            e += excess;
            return m;
        }
    }
}



int BPSPCBlockTree::my_min_select(int i, int j, int t, int min) {
    int m;
    if (min != 3) m = min;
    else m = my_min_excess(i,j);
    int e = 0;
    if (i/first_level_length_ == j/first_level_length_) {
        int a =  my_min_select(i%first_level_length_,j%first_level_length_,t,0,i/first_level_length_,first_level_length_,e,m);
        if (a != -1) return a + (i/first_level_length_)*first_level_length_;
        return -1;
    }
    int a = my_min_select(i%first_level_length_,first_level_length_-1,t,0, i/first_level_length_, first_level_length_, e,m);
    if (a != -1) return a + (i/first_level_length_)*first_level_length_;

    //int index = my_next_block_min_select(i/first_level_length_, j/first_level_length_,t , e,m-e);
    int index = my_faster_next_block_min_select(i/first_level_length_, j/first_level_length_, t , e,m-e);

    a = my_min_select(0,(j/first_level_length_ == index) ? (j%first_level_length_): (first_level_length_-1),t,0,index,first_level_length_,e,m-e);
    if (a != -1) return a + index*first_level_length_;
    return -1;
}

int BPSPCBlockTree::my_min_select_nobackmin(int i, int j, int t) {
    int m = my_min_excess_nobackmin(i,j);
    int e = 0;
    if (i/first_level_length_ == j/first_level_length_) {
        int a =  my_min_select_nobackmin(i%first_level_length_,j%first_level_length_,t,0,i/first_level_length_,first_level_length_,e,m);
        if (a != -1) return a + (i/first_level_length_)*first_level_length_;
        return -1;
    }
    int a = my_min_select_nobackmin(i%first_level_length_,first_level_length_-1,t,0, i/first_level_length_, first_level_length_, e,m);
    if (a != -1) return a + (i/first_level_length_)*first_level_length_;

    //int index = my_next_block_min_select(i/first_level_length_, j/first_level_length_,t , e,m-e);
    int index = my_faster_next_block_min_select(i/first_level_length_, j/first_level_length_, t , e,m-e);

    a = my_min_select_nobackmin(0,(j/first_level_length_ == index) ? (j%first_level_length_): (first_level_length_-1),t,0,index,first_level_length_,e,m-e);
    if (a != -1) return a + index*first_level_length_;
    return -1;
}

int BPSPCBlockTree::min_select(int i, int j, int t) {
    int m = min_excess(i,j);
    int e = 0;
    if (i/first_level_length_ == j/first_level_length_) {
        int a =  min_select(i%first_level_length_,j%first_level_length_,t,0,i/first_level_length_,first_level_length_,e,m);
        if (a != -1) return a + (i/first_level_length_)*first_level_length_;
        return -1;
    }
    int a = min_select(i%first_level_length_,first_level_length_-1,t,0, i/first_level_length_, first_level_length_, e,m);
    if (a != -1) return a + (i/first_level_length_)*first_level_length_;

    //int index = next_block_min_select(i/first_level_length_, j/first_level_length_,t , e,m-e);
    int index = faster_next_block_min_select(i/first_level_length_, j/first_level_length_, t , e,m-e);

    a = min_select(0,(j/first_level_length_ == index) ? (j%first_level_length_): (first_level_length_-1),t,0,index,first_level_length_,e,m-e);
    if (a != -1) return a + index*first_level_length_;
    return -1;
}


int BPSPCBlockTree::my_fwdsearch(int i, int d) {
    int e = 0;
    int a = my_fwdsearch(((i+1)%first_level_length_) - 1,first_level_length_-1,d,0, (i+1)/first_level_length_, first_level_length_, e);
    if (a != -1) return a + ((i+1)/first_level_length_)*first_level_length_;
    int index = my_faster_next_block_fwdsearch((i+1)/first_level_length_, d , e);
    if (index == -1) return input_.size();
    a = my_fwdsearch(-1,first_level_length_-1,d,0, index, first_level_length_, e);
    if (a != -1) return a + index*first_level_length_;
    return input_.size();
}


int BPSPCBlockTree::my_fwdsearch_cback(int i, int d) {
    int e = 0;
    int a = my_fwdsearch_cback(((i+1)%first_level_length_) - 1,first_level_length_-1,d,0, (i+1)/first_level_length_, first_level_length_, e);
    if (a != -1) return a + ((i+1)/first_level_length_)*first_level_length_;
    int index = my_faster_next_block_fwdsearch((i+1)/first_level_length_, d , e);
    if (index == -1) return input_.size();
    a = my_fwdsearch_cback(-1,first_level_length_-1,d,0, index, first_level_length_, e);
    if (a != -1) return a + index*first_level_length_;
    return input_.size();
}


int BPSPCBlockTree::my_fwdsearch_nobackmin(int i, int d) {
    int e = 0;
    int a = my_fwdsearch_nobackmin(((i+1)%first_level_length_) - 1,first_level_length_-1,d,0, (i+1)/first_level_length_, first_level_length_, e);
    if (a != -1) return a + ((i+1)/first_level_length_)*first_level_length_;
    int index = my_faster_next_block_fwdsearch((i+1)/first_level_length_, d , e);
    if (index == -1) return input_.size();
    a = my_fwdsearch_nobackmin(-1,first_level_length_-1,d,0, index, first_level_length_, e);
    if (a != -1) return a + index*first_level_length_;
    return input_.size();
}



int BPSPCBlockTree::fwdsearch(int i, int d) {
    int e = 0;
    int a = fwdsearch(((i+1)%first_level_length_) - 1,first_level_length_-1,d,0, (i+1)/first_level_length_, first_level_length_, e);
    if (a != -1) return a + ((i+1)/first_level_length_)*first_level_length_;
    int index = faster_next_block_fwdsearch((i+1)/first_level_length_, d , e);
    if (index == -1) return input_.size();
    a = fwdsearch(-1,first_level_length_-1,d,0, index, first_level_length_, e);
    if (a != -1) return a + index*first_level_length_;
    return input_.size();
}


int BPSPCBlockTree::min_select(int i, int j, int& t, int level, int level_index, int level_length, int& e, int m) {
    int excess = 0;

    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) {
        int local_min = 1-(int)((*(bt_prefix_min_excess_[level]))[level_index]);
        if (excess + local_min > m) {
            int rank_ones = 0;
            if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
            else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
            excess += 2*rank_ones - (j+1);
            /*
            for (auto& pair: bt_ranks_) {
                if (pair.first == open_) {
                    excess += (*pair.second[level])[level_index];
                } else {
                    excess -= (*pair.second[level])[level_index];
                }
            }
             */
            e += excess;
            return -1;
        }
        int min_count =  ((int)((*(bt_min_count_[level]))[level_index]));
        if (excess + local_min == m &&
            min_count < t) {
            int rank_ones = 0;
            if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
            else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
            excess += 2*rank_ones - (j+1);
            /*
            for (auto& pair: bt_ranks_) {
                if (pair.first == open_) {
                    excess += (*pair.second[level])[level_index];
                } else {
                    excess -= (*pair.second[level])[level_index];
                }
            }
             */
            t -= min_count;
            e += excess;
            return -1;
        }
    }


    int a = -1;
    if (level == number_of_levels_-1) { // Case LeafBlock

        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;
                int min_count = bottom_min_count_2_[index];
                int min_excess = bottom_min_excess_2_[index];
                if (excess + min_excess == m && min_count >= t) {
                    break;
                } else if (excess + min_excess == m ) {
                    t -= min_count;
                }
                excess += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
*/
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            excess += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (excess == m) --t;
            if (t == 0) {
                e += excess;
                return k;
            }
            if ((level_length*level_index + k + 1)%64 == 0) {
                ++chunk;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        e += excess;
        return -1;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = i/child_length;
        int end_block = j/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        /*
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) {
            e += excess;
            return -1;
        }
         */
        if (initial_block == end_block) {
            a = min_select(i%child_length , j%child_length, t, level+1, rank*r_ + initial_block,child_length, excess, m-excess);
            e += excess;
            if (a != -1) return a + initial_block*child_length;
            return -1;
        } else {
            a = min_select(i%child_length, child_length-1, t, level+1, rank*r_ + initial_block,child_length, excess,m-excess);
            if (a != -1) {
                e += excess;
                return a + initial_block*child_length;
            }
            for (int block = initial_block+1; block<end_block; ++block) {
                a = min_select(0, child_length - 1, t, level+1, rank*r_ + block, child_length, excess,m-excess);
                if (a != -1) {
                    e += excess;
                    return a + block*child_length;
                }
            }
            a = min_select(0, j%child_length, t, level+1, rank*r_ + end_block, child_length, excess, m-excess);
            e += excess;
            if (a != -1) return a + end_block*child_length;
            return -1;
        }
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int local_min = excess+ 1-(int)((*bt_prefix_min_excess_first_block_[level])[back_index]);
                if (local_min > m) {
                    int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess += 2*rank_ones - (level_length-offset);
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[back_index];
                        } else {
                            excess -= (*pair.second[level])[back_index];
                        }
                    }
                     */
                    e += excess;
                    return -1;
                }
                int min_count = ((int)((*(bt_min_count_first_block_[level]))[back_index]));
                if (local_min == m &&
                 min_count < t) {
                    int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess += 2*rank_ones - (level_length-offset);
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[back_index];
                        } else {
                            excess -= (*pair.second[level])[back_index];
                        }
                    }
                     */
                    e += excess;
                    t -= min_count;
                    return -1;
                }
            }

            a = min_select(i + offset, j + offset, t, level, first_block, level_length, excess, m-excess);
            e += excess;
            if (a != -1) return a - offset;
            return -1;
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                int local_min = excess+1-(int)((*bt_prefix_min_excess_second_block_[level])[back_index]);
                if (local_min > m) {
                    int rank_ones = 0;
                    if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                    else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                    excess += 2*rank_ones - level_length;
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[level_index];
                        } else {
                            excess -= (*pair.second[level])[level_index];
                        }
                    }
                     */
                    rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess -= 2*rank_ones - (level_length-offset);
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess -= (*pair.second[level])[back_index];
                        } else {
                            excess += (*pair.second[level])[back_index];
                        }
                    }
                     */
                    e += excess;
                    return -1;
                }
                int min_count = ((int)((*(bt_min_count_second_block_[level]))[back_index]));
                if (local_min == m && min_count < t) {
                    int rank_ones = 0;
                    if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                    else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                    excess += 2*rank_ones - level_length;
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[level_index];
                        } else {
                            excess -= (*pair.second[level])[level_index];
                        }
                    }
                     */
                    rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess -= 2*rank_ones - (level_length-offset);
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess -= (*pair.second[level])[back_index];
                        } else {
                            excess += (*pair.second[level])[back_index];
                        }
                    }
                     */
                    t -= min_count;
                    e += excess;
                    return -1;
                }
            }

            a = min_select(offset + i - level_length, offset + j - level_length, t, level, first_block + 1, level_length,
                          excess, m-excess);
            e += excess;
            if (a != -1) return a + level_length - offset;
            return -1;
        } else {
            if (i == 0) {
                int local_min = excess+ 1-(int)((*bt_prefix_min_excess_first_block_[level])[back_index]);
                if (local_min > m) {
                    int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess += 2*rank_ones - (level_length-offset);
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[back_index];
                        } else {
                            excess -= (*pair.second[level])[back_index];
                        }
                    }
                     */
                } else {
                    int min_count = ((int) ((*(bt_min_count_first_block_[level]))[back_index]));
                    if (local_min == m && min_count < t) {
                        int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                        excess += 2*rank_ones - (level_length-offset);
                        /*
                        for (auto& pair: bt_second_ranks_) {
                            if (pair.first == open_) {
                                excess += (*pair.second[level])[back_index];
                            } else {
                                excess -= (*pair.second[level])[back_index];
                            }
                        }
                         */
                        t -= min_count;
                    } else {
                        a = min_select(i + offset, level_length - 1, t, level, first_block, level_length, excess, m-excess);
                        if (a != -1) {
                            e += excess;
                            return a - offset;
                        }
                    }
                }
            }
            else {
                a = min_select(i + offset, level_length - 1, t, level, first_block, level_length, excess, m-excess);
                if (a != -1) {
                    e += excess;
                    return a - offset;
                }
            }

            if (j == level_length - 1) {
                int local_min = excess + 1-(int)((*bt_prefix_min_excess_second_block_[level])[back_index]);
                if (local_min > m) {
                    int rank_ones = 0;
                    if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                    else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                    excess += 2*rank_ones - level_length;
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[level_index];
                        } else {
                            excess -= (*pair.second[level])[level_index];
                        }
                    }
                     */
                    rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess -= 2*rank_ones - (level_length-offset);
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess -= (*pair.second[level])[back_index];
                        } else {
                            excess += (*pair.second[level])[back_index];
                        }
                    }
                     */
                    e += excess;
                    return -1;
                }
                int min_count = ((int)((*(bt_min_count_second_block_[level]))[back_index]));
                if (local_min == m && min_count < t) {
                    int rank_ones = 0;
                    if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                    else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                    excess += 2*rank_ones - level_length;
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[level_index];
                        } else {
                            excess -= (*pair.second[level])[level_index];
                        }
                    }
                     */
                    rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess -= 2*rank_ones - (level_length-offset);
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess -= (*pair.second[level])[back_index];
                        } else {
                            excess += (*pair.second[level])[back_index];
                        }
                    }
                     */
                    t -= ((int)((*(bt_min_count_second_block_[level]))[back_index]));
                    e += excess;
                    return -1;
                }
            }
            a = min_select(0, offset + j - level_length, t, level, first_block + 1, level_length, excess,m-excess);
            e += excess;
            if (a != -1) return a + level_length - offset;
            return -1;
        }
    }
}




int BPSPCBlockTree::my_min_select(int i, int j, int& t, int level, int level_index, int level_length, int& e, int m) {
    int excess = 0;

    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) {
        int local_min = 1-(int)((*(bt_prefix_min_excess_[level]))[level_index]);
        if (excess + local_min > m) {
            int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
            excess += 2*rank_ones - (j+1);
            /*
            for (auto& pair: bt_ranks_) {
                if (pair.first == open_) {
                    excess += (*pair.second[level])[level_index];
                } else {
                    excess -= (*pair.second[level])[level_index];
                }
            }
             */
            e += excess;
            return -1;
        }
        int min_count =  ((int)((*(bt_min_count_[level]))[level_index]));
        if (excess + local_min == m &&
            min_count < t) {
            int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
            excess += 2*rank_ones - (j+1);
            /*
            for (auto& pair: bt_ranks_) {
                if (pair.first == open_) {
                    excess += (*pair.second[level])[level_index];
                } else {
                    excess -= (*pair.second[level])[level_index];
                }
            }
             */
            t -= min_count;
            e += excess;
            return -1;
        }
    }


    int a = -1;
    if (level == number_of_levels_-1) { // Case LeafBlock

        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;
                int min_count = bottom_min_count_2_[index];
                int min_excess = bottom_min_excess_2_[index];
                if (excess + min_excess == m && min_count >= t) {
                    break;
                } else if (excess + min_excess == m ) {
                    t -= min_count;
                }
                excess += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
*/
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            excess += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (excess == m) --t;
            if (t == 0) {
                e += excess;
                return k;
            }
            if ((level_length*level_index + k + 1)%64 == 0) {
                ++chunk;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        e += excess;
        return -1;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = i/child_length;
        int end_block = j/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        /*
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) {
            e += excess;
            return -1;
        }
         */
        if (initial_block == end_block) {
            a = my_min_select(i%child_length , j%child_length, t, level+1, rank*r_ + initial_block,child_length, excess, m-excess);
            e += excess;
            if (a != -1) return a + initial_block*child_length;
            return -1;
        } else {
            a = my_min_select(i%child_length, child_length-1, t, level+1, rank*r_ + initial_block,child_length, excess,m-excess);
            if (a != -1) {
                e += excess;
                return a + initial_block*child_length;
            }
            for (int block = initial_block+1; block<end_block; ++block) {
                a = my_min_select(0, child_length - 1, t, level+1, rank*r_ + block, child_length, excess,m-excess);
                if (a != -1) {
                    e += excess;
                    return a + block*child_length;
                }
            }
            a = my_min_select(0, j%child_length, t, level+1, rank*r_ + end_block, child_length, excess, m-excess);
            e += excess;
            if (a != -1) return a + end_block*child_length;
            return -1;
        }
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int local_min = excess+ 1-(int)((*bt_prefix_min_excess_first_block_[level])[back_index]);
                if (local_min > m) {
                    int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess += 2*rank_ones - (level_length-offset);
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[back_index];
                        } else {
                            excess -= (*pair.second[level])[back_index];
                        }
                    }
                     */
                    e += excess;
                    return -1;
                }
                int min_count = ((int)((*(bt_min_count_first_block_[level]))[back_index]));
                if (local_min == m &&
                    min_count < t) {
                    int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess += 2*rank_ones - (level_length-offset);
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[back_index];
                        } else {
                            excess -= (*pair.second[level])[back_index];
                        }
                    }
                     */
                    e += excess;
                    t -= min_count;
                    return -1;
                }
            }

            a = my_min_select(i + offset, j + offset, t, level, first_block, level_length, excess, m-excess);
            e += excess;
            if (a != -1) return a - offset;
            return -1;
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                int local_min = excess+1-(int)((*bt_prefix_min_excess_second_block_[level])[back_index]);
                if (local_min > m) {
                    int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                    excess += 2*rank_ones - level_length;
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[level_index];
                        } else {
                            excess -= (*pair.second[level])[level_index];
                        }
                    }
                     */
                    rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess -= 2*rank_ones - (level_length-offset);
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess -= (*pair.second[level])[back_index];
                        } else {
                            excess += (*pair.second[level])[back_index];
                        }
                    }
                     */
                    e += excess;
                    return -1;
                }
                int min_count = ((int)((*(bt_min_count_second_block_[level]))[back_index]));
                if (local_min == m && min_count < t) {
                    int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                    excess += 2*rank_ones - level_length;
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[level_index];
                        } else {
                            excess -= (*pair.second[level])[level_index];
                        }
                    }
                     */
                    rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess -= 2*rank_ones - (level_length-offset);
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess -= (*pair.second[level])[back_index];
                        } else {
                            excess += (*pair.second[level])[back_index];
                        }
                    }
                     */
                    t -= min_count;
                    e += excess;
                    return -1;
                }
            }

            a = my_min_select(offset + i - level_length, offset + j - level_length, t, level, first_block + 1, level_length,
                           excess, m-excess);
            e += excess;
            if (a != -1) return a + level_length - offset;
            return -1;
        } else {
            if (i == 0) {
                int local_min = excess+ 1-(int)((*bt_prefix_min_excess_first_block_[level])[back_index]);
                if (local_min > m) {
                    int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess += 2*rank_ones - (level_length-offset);
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[back_index];
                        } else {
                            excess -= (*pair.second[level])[back_index];
                        }
                    }
                     */
                } else {
                    int min_count = ((int) ((*(bt_min_count_first_block_[level]))[back_index]));
                    if (local_min == m && min_count < t) {
                        int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                        excess += 2*rank_ones - (level_length-offset);
                        /*
                        for (auto& pair: bt_second_ranks_) {
                            if (pair.first == open_) {
                                excess += (*pair.second[level])[back_index];
                            } else {
                                excess -= (*pair.second[level])[back_index];
                            }
                        }
                         */
                        t -= min_count;
                    } else {
                        a = my_min_select(i + offset, level_length - 1, t, level, first_block, level_length, excess, m-excess);
                        if (a != -1) {
                            e += excess;
                            return a - offset;
                        }
                    }
                }
            }
            else {
                a = my_min_select(i + offset, level_length - 1, t, level, first_block, level_length, excess, m-excess);
                if (a != -1) {
                    e += excess;
                    return a - offset;
                }
            }

            if (j == level_length - 1) {
                int local_min = excess + 1-(int)((*bt_prefix_min_excess_second_block_[level])[back_index]);
                if (local_min > m) {
                    int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                    excess += 2*rank_ones - level_length;
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[level_index];
                        } else {
                            excess -= (*pair.second[level])[level_index];
                        }
                    }
                     */
                    rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess -= 2*rank_ones - (level_length-offset);
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess -= (*pair.second[level])[back_index];
                        } else {
                            excess += (*pair.second[level])[back_index];
                        }
                    }
                     */
                    e += excess;
                    return -1;
                }
                int min_count = ((int)((*(bt_min_count_second_block_[level]))[back_index]));
                if (local_min == m && min_count < t) {
                    int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                    excess += 2*rank_ones - level_length;
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[level_index];
                        } else {
                            excess -= (*pair.second[level])[level_index];
                        }
                    }
                     */
                    rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess -= 2*rank_ones - (level_length-offset);
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess -= (*pair.second[level])[back_index];
                        } else {
                            excess += (*pair.second[level])[back_index];
                        }
                    }
                     */
                    t -= ((int)((*(bt_min_count_second_block_[level]))[back_index]));
                    e += excess;
                    return -1;
                }
            }
            a = my_min_select(0, offset + j - level_length, t, level, first_block + 1, level_length, excess,m-excess);
            e += excess;
            if (a != -1) return a + level_length - offset;
            return -1;
        }
    }
}

int BPSPCBlockTree::my_min_select_nobackmin(int i, int j, int& t, int level, int level_index, int level_length, int& e, int m) {
    int excess = 0;

    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) {
        int local_min = 1-(int)((*(bt_prefix_min_excess_[level]))[level_index]);
        if (excess + local_min > m) {
            int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
            excess += 2*rank_ones - (j+1);
            /*
            for (auto& pair: bt_ranks_) {
                if (pair.first == open_) {
                    excess += (*pair.second[level])[level_index];
                } else {
                    excess -= (*pair.second[level])[level_index];
                }
            }
             */
            e += excess;
            return -1;
        }
        int min_count =  ((int)((*(bt_min_count_[level]))[level_index]));
        if (excess + local_min == m &&
            min_count < t) {
            int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
            excess += 2*rank_ones - (j+1);
            /*
            for (auto& pair: bt_ranks_) {
                if (pair.first == open_) {
                    excess += (*pair.second[level])[level_index];
                } else {
                    excess -= (*pair.second[level])[level_index];
                }
            }
             */
            t -= min_count;
            e += excess;
            return -1;
        }
    }


    int a = -1;
    if (level == number_of_levels_-1) { // Case LeafBlock

        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;
                int min_count = bottom_min_count_2_[index];
                int min_excess = bottom_min_excess_2_[index];
                if (excess + min_excess == m && min_count >= t) {
                    break;
                } else if (excess + min_excess == m ) {
                    t -= min_count;
                }
                excess += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
*/
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            excess += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (excess == m) --t;
            if (t == 0) {
                e += excess;
                return k;
            }
            if ((level_length*level_index + k + 1)%64 == 0) {
                ++chunk;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        e += excess;
        return -1;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = i/child_length;
        int end_block = j/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        /*
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) {
            e += excess;
            return -1;
        }
         */
        if (initial_block == end_block) {
            a = my_min_select_nobackmin(i%child_length , j%child_length, t, level+1, rank*r_ + initial_block,child_length, excess, m-excess);
            e += excess;
            if (a != -1) return a + initial_block*child_length;
            return -1;
        } else {
            a = my_min_select_nobackmin(i%child_length, child_length-1, t, level+1, rank*r_ + initial_block,child_length, excess,m-excess);
            if (a != -1) {
                e += excess;
                return a + initial_block*child_length;
            }
            for (int block = initial_block+1; block<end_block; ++block) {
                a = my_min_select_nobackmin(0, child_length - 1, t, level+1, rank*r_ + block, child_length, excess,m-excess);
                if (a != -1) {
                    e += excess;
                    return a + block*child_length;
                }
            }
            a = my_min_select_nobackmin(0, j%child_length, t, level+1, rank*r_ + end_block, child_length, excess, m-excess);
            e += excess;
            if (a != -1) return a + end_block*child_length;
            return -1;
        }
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            /*
            if (i == 0 && j + offset == level_length - 1) {
                int local_min = excess+ 1-(int)((*bt_prefix_min_excess_first_block_[level])[back_index]);
                if (local_min > m) {
                    int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess += 2*rank_ones - (level_length-offset);
                    */
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[back_index];
                        } else {
                            excess -= (*pair.second[level])[back_index];
                        }
                    }
                     */
                    /*
                    e += excess;
                    return -1;
                }
                int min_count = ((int)((*(bt_min_count_first_block_[level]))[back_index]));
                if (local_min == m &&
                    min_count < t) {
                    int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess += 2*rank_ones - (level_length-offset);
                    */
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[back_index];
                        } else {
                            excess -= (*pair.second[level])[back_index];
                        }
                    }
                     */
                    /*
                    e += excess;
                    t -= min_count;
                    return -1;
                }
            }
            */
            a = my_min_select_nobackmin(i + offset, j + offset, t, level, first_block, level_length, excess, m-excess);
            e += excess;
            if (a != -1) return a - offset;
            return -1;
        } else if (i + offset >= level_length && j + offset >= level_length) {
            /*
            if (i + offset == level_length && j == level_length - 1) {
                int local_min = excess+1-(int)((*bt_prefix_min_excess_second_block_[level])[back_index]);
                if (local_min > m) {
                    int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                    excess += 2*rank_ones - level_length;
                    */
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[level_index];
                        } else {
                            excess -= (*pair.second[level])[level_index];
                        }
                    }
                     */
                    /*
                    rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess -= 2*rank_ones - (level_length-offset);
                     */
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess -= (*pair.second[level])[back_index];
                        } else {
                            excess += (*pair.second[level])[back_index];
                        }
                    }
                     */
                    /*
                    e += excess;
                    return -1;
                }
                int min_count = ((int)((*(bt_min_count_second_block_[level]))[back_index]));
                if (local_min == m && min_count < t) {
                    int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                    excess += 2*rank_ones - level_length;
                     */
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[level_index];
                        } else {
                            excess -= (*pair.second[level])[level_index];
                        }
                    }
                     */
                    /*
                    rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess -= 2*rank_ones - (level_length-offset);
                     */
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess -= (*pair.second[level])[back_index];
                        } else {
                            excess += (*pair.second[level])[back_index];
                        }
                    }
                     */
                    /*
                    t -= min_count;
                    e += excess;
                    return -1;
                }
            }
                     */

            a = my_min_select_nobackmin(offset + i - level_length, offset + j - level_length, t, level, first_block + 1, level_length,
                              excess, m-excess);
            e += excess;
            if (a != -1) return a + level_length - offset;
            return -1;
        } else {
            /*
            if (i == 0) {
                int local_min = excess+ 1-(int)((*bt_prefix_min_excess_first_block_[level])[back_index]);
                if (local_min > m) {
                    int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess += 2*rank_ones - (level_length-offset);
                    */
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[back_index];
                        } else {
                            excess -= (*pair.second[level])[back_index];
                        }
                    }
                     */
                    /*
                } else {
                    int min_count = ((int) ((*(bt_min_count_first_block_[level]))[back_index]));
                    if (local_min == m && min_count < t) {
                        int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                        excess += 2*rank_ones - (level_length-offset);
                        /*
                        for (auto& pair: bt_second_ranks_) {
                            if (pair.first == open_) {
                                excess += (*pair.second[level])[back_index];
                            } else {
                                excess -= (*pair.second[level])[back_index];
                            }
                        }
                         */
                    /*
                        t -= min_count;
                    } else {
                        a = my_min_select_nobackmin(i + offset, level_length - 1, t, level, first_block, level_length, excess, m-excess);
                        if (a != -1) {
                            e += excess;
                            return a - offset;
                        }
                    }
                }
            }
            else {
                */
                a = my_min_select_nobackmin(i + offset, level_length - 1, t, level, first_block, level_length, excess, m-excess);
                if (a != -1) {
                    e += excess;
                    return a - offset;
                }
            /*}*/
            /*
            if (j == level_length - 1) {
                int local_min = excess + 1-(int)((*bt_prefix_min_excess_second_block_[level])[back_index]);
                if (local_min > m) {
                    int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                    excess += 2*rank_ones - level_length;
                    */
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[level_index];
                        } else {
                            excess -= (*pair.second[level])[level_index];
                        }
                    }
                     */
                    /*
                    rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess -= 2*rank_ones - (level_length-offset);
                    */
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess -= (*pair.second[level])[back_index];
                        } else {
                            excess += (*pair.second[level])[back_index];
                        }
                    }
                     */
                    /*
                    e += excess;
                    return -1;
                }
                int min_count = ((int)((*(bt_min_count_second_block_[level]))[back_index]));
                if (local_min == m && min_count < t) {
                    int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                    excess += 2*rank_ones - level_length;
                    */
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[level])[level_index];
                        } else {
                            excess -= (*pair.second[level])[level_index];
                        }
                    }
                     */
                    /*
                    rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    excess -= 2*rank_ones - (level_length-offset);
                    */
                    /*
                    for (auto& pair: bt_second_ranks_) {
                        if (pair.first == open_) {
                            excess -= (*pair.second[level])[back_index];
                        } else {
                            excess += (*pair.second[level])[back_index];
                        }
                    }
                     */
                    /*
                    t -= ((int)((*(bt_min_count_second_block_[level]))[back_index]));
                    e += excess;
                    return -1;
                }
            }*/
            a = my_min_select_nobackmin(0, offset + j - level_length, t, level, first_block + 1, level_length, excess,m-excess);
            e += excess;
            if (a != -1) return a + level_length - offset;
            return -1;
        }
    }
}


int BPSPCBlockTree::fwdsearch(int i, int j, int d, int level, int level_index, int level_length, int& e) {
    if (i == -1 && j == level_length-1 && e + 1-(int)((*(bt_prefix_min_excess_[level]))[level_index]) > d) {
        int rank_ones = 0;
        if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
        else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
        e += 2*rank_ones - level_length;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                oe += (*pair.second[level])[level_index];
            } else {
                oe -= (*pair.second[level])[level_index];
            }
        }
         */
        return -1;
    }

    int a = -1;
    if (level == number_of_levels_-1) { // Case LeafBlock
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i+1;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;

                if (e + bottom_min_excess_2_[index] <= d) {
                    break;
                }
                e += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
*/
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            e += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == d) return k;
            if ((level_length*level_index + k + 1)%64 == 0) {
                ++chunk;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        return -1;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = (i+1)/child_length;
        int end_block = j/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
        if (initial_block == end_block) {
            a = fwdsearch(((i+1)%child_length) - 1, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != -1) return a + initial_block*child_length;
            return -1;
        } else {
            a = fwdsearch(((i+1)%child_length) - 1, child_length-1, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != -1) return a + initial_block*child_length;
            for (int block = initial_block+1; block<end_block; ++block) {
                a = fwdsearch(-1, child_length - 1, d, level+1, rank*r_ + block, child_length, e);
                if (a != -1) return a + block*child_length;
            }
            a = fwdsearch(-1, j%child_length, d, level+1, rank*r_ + end_block, child_length, e);
            if (a != -1) return a + end_block*child_length;
            return -1;
        }
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + 1 + offset < level_length && j + offset < level_length) {
            if (i + 1 == 0 && j + offset == level_length - 1 &&
                    e + 1-(int)((*bt_prefix_min_excess_first_block_[level])[back_index]) > d) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                return -1;
            }
            a = fwdsearch(i + offset, j + offset, d, level, first_block, level_length, e);
            if (a != -1) return a - offset;
            return -1;
        } else if (i + 1 + offset >= level_length && j + offset >= level_length) {
            if (i + 1 + offset == level_length && j == level_length - 1 &&
                    e + 1-(int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d) {
                int rank_ones = 0;
                if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                return -1;
            }
            a = fwdsearch(offset + i - level_length, offset + j - level_length, d, level, first_block + 1, level_length,
                          e);
            if (a != -1) return a + level_length - offset;
            return -1;
        } else {
            if (i + 1 == 0 && e + 1-(int)((*bt_prefix_min_excess_first_block_[level])[back_index]) > d) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
            } else {
                a = fwdsearch(i + offset, level_length - 1, d, level, first_block, level_length, e);
                if (a != -1) return a - offset;
            }

            if (j == level_length - 1 && e + 1-(int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d) {
                int rank_ones = 0;
                if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
            } else {
                a = fwdsearch(-1, offset + j - level_length, d, level, first_block + 1, level_length, e);
                if (a != -1) return a + level_length - offset;
            }
            return a;
        }
    }
}

int BPSPCBlockTree::my_fwdsearch(int i, int j, int d, int level, int level_index, int level_length, int& e) {
    if (i == -1 && j == level_length-1 && e + 1-(int)((*(bt_prefix_min_excess_[level]))[level_index]) > d) {
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        e += 2*rank_ones - level_length;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                oe += (*pair.second[level])[level_index];
            } else {
                oe -= (*pair.second[level])[level_index];
            }
        }
         */
        return -1;
    }

    int a = -1;
    if (level == number_of_levels_-1) { // Case LeafBlock
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i+1;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

        /*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;

                if (e + bottom_min_excess_2_[index] <= d) {
                    break;
                }
                e += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
        */
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            e += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == d) return k;

            if ((level_length*level_index + k + 1)%64 == 0) {
                chunk++;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        return -1;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = (i+1)/child_length;
        int end_block = j/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
        if (initial_block == end_block) {
            a = my_fwdsearch(((i+1)%child_length) - 1, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != -1) return a + initial_block*child_length;
            return -1;
        } else {
            a = my_fwdsearch(((i+1)%child_length) - 1, child_length-1, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != -1) return a + initial_block*child_length;
            for (int block = initial_block+1; block<end_block; ++block) {
                a = my_fwdsearch(-1, child_length - 1, d, level+1, rank*r_ + block, child_length, e);
                if (a != -1) return a + block*child_length;
            }
            a = my_fwdsearch(-1, j%child_length, d, level+1, rank*r_ + end_block, child_length, e);
            if (a != -1) return a + end_block*child_length;
            return -1;
        }
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + 1 + offset < level_length && j + offset < level_length) {
            if (i + 1 == 0 && j + offset == level_length - 1 &&
                e + 1-(int)((*bt_prefix_min_excess_first_block_[level])[back_index]) > d) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                return -1;
            }
            a = my_fwdsearch(i + offset, j + offset, d, level, first_block, level_length, e);
            if (a != -1) return a - offset;
            return -1;
        } else if (i + 1 + offset >= level_length && j + offset >= level_length) {
            if (i + 1 + offset == level_length && j == level_length - 1 &&
                e + 1-(int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d) {
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                return -1;
            }
            a = my_fwdsearch(offset + i - level_length, offset + j - level_length, d, level, first_block + 1, level_length,
                          e);
            if (a != -1) return a + level_length - offset;
            return -1;
        } else {
            if (i + 1 == 0 && e + 1-(int)((*bt_prefix_min_excess_first_block_[level])[back_index]) > d) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
            } else {
                a = my_fwdsearch(i + offset, level_length - 1, d, level, first_block, level_length, e);
                if (a != -1) return a - offset;
            }

            if (j == level_length - 1 && e + 1-(int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d) {
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
            } else {
                a = my_fwdsearch(-1, offset + j - level_length, d, level, first_block + 1, level_length, e);
                if (a != -1) return a + level_length - offset;
            }
            return a;
        }
    }
}


int BPSPCBlockTree::my_fwdsearch_cback(int i, int j, int d, int level, int level_index, int level_length, int& e) {
    if (i == -1 && j == level_length-1 && e + 1-(int)((*(bt_prefix_min_excess_[level]))[level_index]) > d) {
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        e += 2*rank_ones - level_length;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                oe += (*pair.second[level])[level_index];
            } else {
                oe -= (*pair.second[level])[level_index];
            }
        }
         */
        return -1;
    }

    int a = -1;
    if (level == number_of_levels_-1) { // Case LeafBlock
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i+1;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

        /*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;

                if (e + bottom_min_excess_2_[index] <= d) {
                    break;
                }
                e += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
        */
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            e += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == d) return k;

            if ((level_length*level_index + k + 1)%64 == 0) {
                chunk++;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        return -1;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = (i+1)/child_length;
        int end_block = j/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
        if (initial_block == end_block) {
            a = my_fwdsearch_cback(((i+1)%child_length) - 1, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != -1) return a + initial_block*child_length;
            return -1;
        } else {
            a = my_fwdsearch_cback(((i+1)%child_length) - 1, child_length-1, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != -1) return a + initial_block*child_length;
            for (int block = initial_block+1; block<end_block; ++block) {
                a = my_fwdsearch_cback(-1, child_length - 1, d, level+1, rank*r_ + block, child_length, e);
                if (a != -1) return a + block*child_length;
            }
            a = my_fwdsearch_cback(-1, j%child_length, d, level+1, rank*r_ + end_block, child_length, e);
            if (a != -1) return a + end_block*child_length;
            return -1;
        }
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + 1 + offset < level_length && j + offset < level_length) {
            int min_prefix_first_block = 1-((*(bt_prefix_min_excess_back_block_[level]))[back_index]);
            if (((*(bt_min_in_first_block_[level]))[back_index]) || ((*(bt_min_in_both_blocks_[level]))[back_index])) {
                min_prefix_first_block = (1-((*(bt_prefix_min_excess_[level]))[level_index]));
            }
            if (i + 1 == 0 && j + offset == level_length - 1 &&
                e + min_prefix_first_block > d) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                return -1;
            }
            a = my_fwdsearch_cback(i + offset, j + offset, d, level, first_block, level_length, e);
            if (a != -1) return a - offset;
            return -1;
        } else if (i + 1 + offset >= level_length && j + offset >= level_length) {
            int min_prefix_second_block = 1-((*(bt_prefix_min_excess_back_block_[level]))[back_index]);
            if (!((*(bt_min_in_first_block_[level]))[back_index]) && !((*(bt_min_in_both_blocks_[level]))[back_index])) {
                int first_block_excess = 0;
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                min_prefix_second_block = (1-((*(bt_prefix_min_excess_[level]))[level_index])) - first_block_excess;
            }
            if (i + 1 + offset == level_length && j == level_length - 1 &&
                e + min_prefix_second_block > d) {
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                return -1;
            }
            a = my_fwdsearch_cback(offset + i - level_length, offset + j - level_length, d, level, first_block + 1, level_length,
                             e);
            if (a != -1) return a + level_length - offset;
            return -1;
        } else {
            int min_prefix_first_block = 1-((*(bt_prefix_min_excess_back_block_[level]))[back_index]);
            if (((*(bt_min_in_first_block_[level]))[back_index]) || ((*(bt_min_in_both_blocks_[level]))[back_index])) {
                min_prefix_first_block = (1-((*(bt_prefix_min_excess_[level]))[level_index]));
            }
            if (i + 1 == 0 && e + min_prefix_first_block > d) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
            } else {
                a = my_fwdsearch_cback(i + offset, level_length - 1, d, level, first_block, level_length, e);
                if (a != -1) return a - offset;
            }

            int min_prefix_second_block = 1-((*(bt_prefix_min_excess_back_block_[level]))[back_index]);
            if (!((*(bt_min_in_first_block_[level]))[back_index]) && !((*(bt_min_in_both_blocks_[level]))[back_index])) {
                int first_block_excess = 0;
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                min_prefix_second_block = (1-((*(bt_prefix_min_excess_[level]))[level_index])) - first_block_excess;
            }
            if (j == level_length - 1 && e + min_prefix_second_block > d) {
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
            } else {
                a = my_fwdsearch_cback(-1, offset + j - level_length, d, level, first_block + 1, level_length, e);
                if (a != -1) return a + level_length - offset;
            }
            return a;
        }
    }
}

int BPSPCBlockTree::my_fwdsearch_nobackmin(int i, int j, int d, int level, int level_index, int level_length, int& e) {
        if (i == -1 && j == level_length-1 && e + 1-(int)((*(bt_prefix_min_excess_[level]))[level_index]) > d) {
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        e += 2*rank_ones - level_length;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                oe += (*pair.second[level])[level_index];
            } else {
                oe -= (*pair.second[level])[level_index];
            }
        }
         */
        return -1;
        }

        int a = -1;
        if (level == number_of_levels_-1) { // Case LeafBlock
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i+1;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

                /*
                if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                    while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                        int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                        if (index < 0) index += 1<<bottom_bit_size_;

                        if (e + bottom_min_excess_2_[index] <= d) {
                            break;
                        }
                        e += bottom_excess_2_[index];;
                        k += bottom_bit_size_;
                    }
                }
                */
                for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
                e += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
                //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
                if (e == d) return k;

                if ((level_length*level_index + k + 1)%64 == 0) {
                chunk++;
                chunk_info = *(leaf_string.m_data+chunk);
                }
                }
                return -1;
                }
                if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
                int child_length = level_length/r_;
                int initial_block = (i+1)/child_length;
                int end_block = j/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
        end_block = (next_level_size-1)%r_;
        j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
        if (initial_block == end_block) {
        a = my_fwdsearch_nobackmin(((i+1)%child_length) - 1, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
        if (a != -1) return a + initial_block*child_length;
        return -1;
        } else {
        a = my_fwdsearch_nobackmin(((i+1)%child_length) - 1, child_length-1, d, level+1, rank*r_ + initial_block,child_length, e);
        if (a != -1) return a + initial_block*child_length;
        for (int block = initial_block+1; block<end_block; ++block) {
        a = my_fwdsearch_nobackmin(-1, child_length - 1, d, level+1, rank*r_ + block, child_length, e);
        if (a != -1) return a + block*child_length;
        }
        a = my_fwdsearch_nobackmin(-1, j%child_length, d, level+1, rank*r_ + end_block, child_length, e);
        if (a != -1) return a + end_block*child_length;
        return -1;
        }
        } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + 1 + offset < level_length && j + offset < level_length) {
        /*
        if (i + 1 == 0 && j + offset == level_length - 1 &&
        e + 1-(int)((*bt_prefix_min_excess_first_block_[level])[back_index]) > d) {
        int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
        e += 2*rank_ones - (level_length-offset);
        */
         /*
        for (auto& pair: bt_second_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[back_index];
            } else {
                e -= (*pair.second[level])[back_index];
            }
        }
         */
        /*
         return -1;
        }
         */
        a = my_fwdsearch_nobackmin(i + offset, j + offset, d, level, first_block, level_length, e);
        if (a != -1) return a - offset;
        return -1;
        } else if (i + 1 + offset >= level_length && j + offset >= level_length) {
            /*
        if (i + 1 + offset == level_length && j == level_length - 1 &&
        e + 1-(int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d) {
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        e += 2*rank_ones - level_length;
             */
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[level_index];
            } else {
                e -= (*pair.second[level])[level_index];
            }
        }
         */
        /*
        rank_ones = (*bt_second_ranks_[open_][level])[back_index];
        e -= 2*rank_ones - (level_length-offset);
         */
        /*
        for (auto& pair: bt_second_ranks_) {
            if (pair.first == open_) {
                e -= (*pair.second[level])[back_index];
            } else {
                e += (*pair.second[level])[back_index];
            }
        }
         */
        /*
        return -1;
        }
         */
        a = my_fwdsearch_nobackmin(offset + i - level_length, offset + j - level_length, d, level, first_block + 1, level_length,
        e);
        if (a != -1) return a + level_length - offset;
        return -1;
        } else {
            /*
        if (i + 1 == 0 && e + 1-(int)((*bt_prefix_min_excess_first_block_[level])[back_index]) > d) {
        int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
        e += 2*rank_ones - (level_length-offset);
             */
        /*
        for (auto& pair: bt_second_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[back_index];
            } else {
                e -= (*pair.second[level])[back_index];
            }
        }
         */
        /*
        } else {
         */
        a = my_fwdsearch_nobackmin(i + offset, level_length - 1, d, level, first_block, level_length, e);
        if (a != -1) return a - offset;
        /*
        }
         */
/*
        if (j == level_length - 1 && e + 1-(int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d) {
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        e += 2*rank_ones - level_length;
  */
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[level_index];
            } else {
                e -= (*pair.second[level])[level_index];
            }
        }
         */
    /*

        rank_ones = (*bt_second_ranks_[open_][level])[back_index];
        e -= 2*rank_ones - (level_length-offset);
      */
        /*
        for (auto& pair: bt_second_ranks_) {
            if (pair.first == open_) {
                e -= (*pair.second[level])[back_index];
            } else {
                e += (*pair.second[level])[back_index];
            }
        }
         */
        /*
        } else {
         */
            a = my_fwdsearch_nobackmin(-1, offset + j - level_length, d, level, first_block + 1, level_length, e);
            if (a != -1) return a + level_length - offset;
        /*
        }
         */
        return a;
        }
    }
}



int BPSPCBlockTree::next_block_min_select(int initial_block, int end_block, int& t, int& e, int m) {
    int excess = 0;
    for (int i = initial_block+1; i <= end_block; ++i) {
        if (excess + 1-(int)((*(bt_prefix_min_excess_[0]))[i]) == m) {
            int min_count = ((int)((*(bt_min_count_[0]))[i]));
            if (min_count >= t) {
                e += excess;
                return i;
            }
            else t -= min_count;
        }
        int rank_ones = (i == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[i-1])));
        excess += 2*rank_ones - first_level_length_ ;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess += (*pair.second[0])[i];
            } else {
                excess -= (*pair.second[0])[i];
            }
        }
         */
        
    }
    e += excess;
    return -1;
}

int BPSPCBlockTree::my_next_block_min_select(int initial_block, int end_block, int& t, int& e, int m) {
    int excess = 0;
    for (int i = initial_block+1; i <= end_block; ++i) {
        if (excess + 1-(int)((*(bt_prefix_min_excess_[0]))[i]) == m) {
            int min_count = ((int)((*(bt_min_count_[0]))[i]));
            if (min_count >= t) {
                e += excess;
                return i;
            }
            else t -= min_count;
        }
        int rank_ones = ((int)((*bt_ranks_[open_][0])[i]));
        excess += 2*rank_ones - first_level_length_ ;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess += (*pair.second[0])[i];
            } else {
                excess -= (*pair.second[0])[i];
            }
        }
         */

    }
    e += excess;
    return -1;
}

int BPSPCBlockTree::faster_next_block_min_select(int initial_block, int end_block, int& t, int&e, int m) {
    int excess = 0;
    if (end_block > initial_block + 1) {
        int index_last = (top_excess_->size()+n_pre_last_level_)-1;
        int r_initial_block = 0;
        if (initial_block < n_last_level_) {
            r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
            index_last = r_*(index_last+1);
        }
        else
            r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;
        bool going_up = true;
        while (true) {
            if (going_up) {
                if (r_initial_block == 0) break;
                while (r_initial_block%r_ != 0) {
                    ++r_initial_block;
                    if (r_initial_block >= number_of_blocks_first_level_+n_internal_nodes_) break;
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;;
                        if (initial_block >= number_of_blocks_first_level_) break;
                        int local_min = excess + 1-(int)((*(bt_prefix_min_excess_[0]))[initial_block]);
                        int min_count =  (int)((*(bt_min_count_[0]))[initial_block]);
                        if (local_min < m ){
                            e += excess;
                            return initial_block;
                        }
                        else if (local_min == m &&
                                min_count >= t) {
                            e += excess;
                            return initial_block;
                        } else {
                            if (local_min == m) t -= min_count;
                            int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                            excess += 2*rank_ones - first_level_length_;
                            /*
                            for (auto& pair: bt_ranks_) {
                                if (pair.first == open_) {
                                    excess += (*pair.second[0])[initial_block];
                                } else {
                                    excess -= (*pair.second[0])[initial_block];
                                }
                            }
                             */
                        }
                    }
                    else {
                        int local_min = excess + 1-(int)((*(top_min_excess_))[r_initial_block]);
                        int min_count = (int)((*(top_min_count_))[r_initial_block]);
                        if (local_min < m) {
                            going_up = false;
                            break;
                        }
                        else if (local_min == m &&
                                min_count >= t  ) {
                            going_up = false;
                            break;
                        } else {
                            if (local_min == m) t -= min_count;
                            excess += decoded_excess((*top_excess_)[r_initial_block]);
                        }
                    }
                }
                // LOOKUP
                if (going_up && r_initial_block != index_last && r_initial_block+1 < number_of_blocks_first_level_+n_internal_nodes_) {
                    if (r_initial_block+1 >= n_internal_nodes_) {
                        if (r_initial_block+1-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block+1-n_internal_nodes_;
                        else initial_block = r_initial_block+1 - n_pre_last_level_-n_internal_nodes_;
                        if (initial_block >= number_of_blocks_first_level_) break;
                        int local_min = excess + 1-(int)((*(bt_prefix_min_excess_[0]))[initial_block]);
                        int min_count =  (int)((*(bt_min_count_[0]))[initial_block]);
                        if (local_min < m ){
                            e += excess;
                            return initial_block;
                        }
                        else if (local_min == m &&
                                 min_count >= t) {
                            e += excess;
                            return initial_block;
                        }
                    }
                    else {
                        int local_min = excess + 1-(int)((*(top_min_excess_))[r_initial_block+1]);
                        int min_count = (int)((*(top_min_count_))[r_initial_block+1]);
                        if (local_min < m) {
                            going_up = false;
                            ++r_initial_block;
                        }
                        else if (local_min == m &&
                                 min_count >= t  ) {
                            going_up = false;
                            ++r_initial_block;
                        }
                    }
                }


                if (going_up) {
                    r_initial_block = (r_initial_block-1)/r_;
                    index_last = index_last/r_ - 1;
                }
            } else {
                if (r_initial_block >= n_internal_nodes_) {
                    r_initial_block -= n_internal_nodes_;
                    e += excess;
                    if (r_initial_block < n_pre_last_level_) return  n_last_level_ + r_initial_block;
                    return  r_initial_block - n_pre_last_level_;
                } else {
                    r_initial_block = r_initial_block*r_ +1;

                    while (true) {
                        if (r_initial_block >= n_internal_nodes_) {
                            if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                            else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;
                            if (initial_block >= number_of_blocks_first_level_) break;
                            int local_min = excess + 1-(int) ((*(bt_prefix_min_excess_[0]))[initial_block]);
                            int min_count = (int) ((*(bt_min_count_[0]))[initial_block]);
                            if (local_min < m) {
                                e += excess;
                                return initial_block;
                            }
                            else if (local_min == m &&
                                    min_count >= t ) {
                                e += excess;
                                return initial_block;
                            } else {
                                if (local_min == m) t -= min_count;
                                int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                                excess += 2*rank_ones - first_level_length_;
                                /*
                                for (auto& pair: bt_ranks_) {
                                    if (pair.first == open_) {
                                        excess += (*pair.second[0])[initial_block];
                                    } else {
                                        excess -= (*pair.second[0])[initial_block];
                                    }
                                }
                                */
                            }
                            ++r_initial_block;
                        } else {
                            int local_min = excess +1- (int) ((*(top_min_excess_))[r_initial_block]);
                            int min_count = (int) ((*(top_min_count_))[r_initial_block]);
                            if (local_min < m) {
                                break;
                            }
                            else if (local_min == m &&
                                    min_count >= t ) {
                                break;
                            } else {
                                if(local_min == m) t -=  min_count;
                                excess += decoded_excess((*top_excess_)[r_initial_block]);
                            }
                            ++r_initial_block;
                        }
                    }
                }
            }
        }
        e += excess;
    }
    return end_block;
}



int BPSPCBlockTree::my_faster_next_block_min_select(int initial_block, int end_block, int& t, int&e, int m) {
    int excess = 0;
    if (end_block > initial_block + 1) {
        int index_last = (top_excess_->size()+n_pre_last_level_)-1;
        int r_initial_block = 0;
        if (initial_block < n_last_level_) {
            r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
            index_last = r_*(index_last+1);
        }
        else
            r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;
        bool going_up = true;
        while (true) {
            if (going_up) {
                if (r_initial_block == 0) break;
                while (r_initial_block%r_ != 0) {
                    ++r_initial_block;
                    if (r_initial_block >= number_of_blocks_first_level_+n_internal_nodes_) break;
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;;
                        if (initial_block >= number_of_blocks_first_level_) break;
                        int local_min = excess + 1-(int)((*(bt_prefix_min_excess_[0]))[initial_block]);
                        int min_count =  (int)((*(bt_min_count_[0]))[initial_block]);
                        if (local_min < m ){
                            e += excess;
                            return initial_block;
                        }
                        else if (local_min == m &&
                                 min_count >= t) {
                            e += excess;
                            return initial_block;
                        } else {
                            if (local_min == m) t -= min_count;
                            int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                            excess += 2*rank_ones - first_level_length_;
                            /*
                            for (auto& pair: bt_ranks_) {
                                if (pair.first == open_) {
                                    excess += (*pair.second[0])[initial_block];
                                } else {
                                    excess -= (*pair.second[0])[initial_block];
                                }
                            }
                             */
                        }
                    }
                    else {
                        int local_min = excess + 1-(int)((*(top_min_excess_))[r_initial_block]);
                        int min_count = (int)((*(top_min_count_))[r_initial_block]);
                        if (local_min < m) {
                            going_up = false;
                            break;
                        }
                        else if (local_min == m &&
                                 min_count >= t  ) {
                            going_up = false;
                            break;
                        } else {
                            if (local_min == m) t -= min_count;
                            excess += decoded_excess((*top_excess_)[r_initial_block]);
                        }
                    }
                }
                // LOOKUP
                if (going_up && r_initial_block != index_last && r_initial_block+1 < number_of_blocks_first_level_+n_internal_nodes_) {
                    if (r_initial_block+1 >= n_internal_nodes_) {
                        if (r_initial_block+1-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block+1-n_internal_nodes_;
                        else initial_block = r_initial_block+1 - n_pre_last_level_-n_internal_nodes_;
                        if (initial_block >= number_of_blocks_first_level_) break;
                        int local_min = excess + 1-(int)((*(bt_prefix_min_excess_[0]))[initial_block]);
                        int min_count =  (int)((*(bt_min_count_[0]))[initial_block]);
                        if (local_min < m ){
                            e += excess;
                            return initial_block;
                        }
                        else if (local_min == m &&
                                 min_count >= t) {
                            e += excess;
                            return initial_block;
                        }
                    }
                    else {
                        int local_min = excess + 1-(int)((*(top_min_excess_))[r_initial_block+1]);
                        int min_count = (int)((*(top_min_count_))[r_initial_block+1]);
                        if (local_min < m) {
                            going_up = false;
                            ++r_initial_block;
                        }
                        else if (local_min == m &&
                                 min_count >= t  ) {
                            going_up = false;
                            ++r_initial_block;
                        }
                    }
                }


                if (going_up) {
                    r_initial_block = (r_initial_block-1)/r_;
                    index_last = index_last/r_ - 1;
                }
            } else {
                if (r_initial_block >= n_internal_nodes_) {
                    r_initial_block -= n_internal_nodes_;
                    e += excess;
                    if (r_initial_block < n_pre_last_level_) return  n_last_level_ + r_initial_block;
                    return  r_initial_block - n_pre_last_level_;
                } else {
                    r_initial_block = r_initial_block*r_ +1;

                    while (true) {
                        if (r_initial_block >= n_internal_nodes_) {
                            if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                            else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;
                            if (initial_block >= number_of_blocks_first_level_) break;
                            int local_min = excess + 1-(int) ((*(bt_prefix_min_excess_[0]))[initial_block]);
                            int min_count = (int) ((*(bt_min_count_[0]))[initial_block]);
                            if (local_min < m) {
                                e += excess;
                                return initial_block;
                            }
                            else if (local_min == m &&
                                     min_count >= t ) {
                                e += excess;
                                return initial_block;
                            } else {
                                if (local_min == m) t -= min_count;
                                int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                                excess += 2*rank_ones - first_level_length_;
                                /*
                                for (auto& pair: bt_ranks_) {
                                    if (pair.first == open_) {
                                        excess += (*pair.second[0])[initial_block];
                                    } else {
                                        excess -= (*pair.second[0])[initial_block];
                                    }
                                }
                                */
                            }
                            ++r_initial_block;
                        } else {
                            int local_min = excess +1- (int) ((*(top_min_excess_))[r_initial_block]);
                            int min_count = (int) ((*(top_min_count_))[r_initial_block]);
                            if (local_min < m) {
                                break;
                            }
                            else if (local_min == m &&
                                     min_count >= t ) {
                                break;
                            } else {
                                if(local_min == m) t -=  min_count;
                                excess += decoded_excess((*top_excess_)[r_initial_block]);
                            }
                            ++r_initial_block;
                        }
                    }
                }
            }
        }
        e += excess;
    }
    return end_block;
}

int BPSPCBlockTree::next_block_fwdsearch(int initial_block, int d, int& e) {
    for (int i = initial_block+1; i < number_of_blocks_first_level_; ++i) {
        if (e + 1-(int)((*(bt_prefix_min_excess_[0]))[i]) <= d) {
            return i;
        } else {
            int rank_ones = (i == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[i-1])));
            e += 2*rank_ones - first_level_length_ ;
            /*
            for (auto& pair: bt_ranks_) {
                if (pair.first == open_) {
                    e += (*pair.second[0])[i];
                } else {
                    e -= (*pair.second[0])[i];
                }
            }
             */

        }
    }
    return -1;
}

int BPSPCBlockTree::my_next_block_fwdsearch(int initial_block, int d, int& e) {
    for (int i = initial_block+1; i < number_of_blocks_first_level_; ++i) {
        if (e + 1-(int)((*(bt_prefix_min_excess_[0]))[i]) <= d) {
            return i;
        } else {
            int rank_ones = ((int)((*bt_ranks_[open_][0])[i]));
            e += 2*rank_ones - first_level_length_ ;
            /*
            for (auto& pair: bt_ranks_) {
                if (pair.first == open_) {
                    e += (*pair.second[0])[i];
                } else {
                    e -= (*pair.second[0])[i];
                }
            }
             */

        }
    }
    return -1;
}

int BPSPCBlockTree::faster_next_block_fwdsearch(int initial_block, int d, int& e) {
    int r_initial_block = 0;
    int index_last = (top_excess_->size()+n_pre_last_level_)-1;
    if (initial_block < n_last_level_) {
        r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
        index_last = r_*(index_last+1);
    }
    else
        r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;
    bool going_up = true;
    while (true) {
        if (going_up) {
            if (r_initial_block == 0) break;
            while (r_initial_block%r_ != 0) {
                ++r_initial_block;
                if (r_initial_block >= number_of_blocks_first_level_+n_internal_nodes_) break;
                if (r_initial_block >= n_internal_nodes_) {
                    if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                    else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;;
                    if (initial_block >= number_of_blocks_first_level_) break;
                    if (e + 1-(int)((*(bt_prefix_min_excess_[0]))[initial_block]) <= d) {
                        return initial_block;
                    } else {
                        int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                        e += 2*rank_ones - first_level_length_;
                        /*
                        for (auto& pair: bt_ranks_) {
                            if (pair.first == open_) {
                                e += (*pair.second[0])[initial_block];
                            } else {
                                e -= (*pair.second[0])[initial_block];
                            }
                        }
                         */
                    }
                }
                else {
                    if (e + 1-(int)((*(top_min_excess_))[r_initial_block]) <= d) {
                        going_up = false;
                        break;
                    } else {
                        /*
                        int rank_ones = (*top_rank_ones_)[r_initial_block];
                        e += 2*rank_ones - level_length;
                         */
                        e += decoded_excess((*top_excess_)[r_initial_block]);
                    }
                }
            }

            // LOOKUP
            if (going_up && r_initial_block != index_last && r_initial_block+1 < number_of_blocks_first_level_+n_internal_nodes_) {
                if (r_initial_block+1 >= n_internal_nodes_) {
                    if (r_initial_block+1-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block+1-n_internal_nodes_;
                    else initial_block = r_initial_block+1 - n_pre_last_level_-n_internal_nodes_;
                    if (initial_block >= number_of_blocks_first_level_) break;
                    if (e + 1-(int)((*(bt_prefix_min_excess_[0]))[initial_block]) <= d) {
                        return initial_block;
                    }
                }
                else {
                    if (e + 1-(int)((*(top_min_excess_))[r_initial_block+1]) <= d) {
                        going_up = false;
                        ++r_initial_block;
                    }
                }
            }

            if (going_up) {
                r_initial_block = (r_initial_block-1)/r_;
                index_last = index_last/r_ - 1;
            }
        } else {
            if (r_initial_block >= n_internal_nodes_) {
                r_initial_block -= n_internal_nodes_;
                if (r_initial_block < n_pre_last_level_) return  n_last_level_ + r_initial_block;
                return  r_initial_block - n_pre_last_level_;
            } else {
                r_initial_block = r_initial_block*r_ +1;
                while (true) {
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;
                        if (initial_block >= number_of_blocks_first_level_) break;
                        if (e + 1-(int) ((*(bt_prefix_min_excess_[0]))[initial_block]) <= d) {
                            return initial_block;
                        } else {
                            int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                            e += 2*rank_ones - first_level_length_;
                            /*
                            for (auto& pair: bt_ranks_) {
                                if (pair.first == open_) {
                                    e += (*pair.second[0])[initial_block];
                                } else {
                                    e -= (*pair.second[0])[initial_block];
                                }
                            }
                             */
                        }
                        ++r_initial_block;
                    } else {
                        if (e + 1-(int) ((*(top_min_excess_))[r_initial_block]) <= d) {
                            break;
                        } else {
                            /*
                            int rank_ones = (*top_rank_ones_)[r_initial_block];
                            e += 2*rank_ones - level_length;
                             */
                            e += decoded_excess((*top_excess_)[r_initial_block]);
                        }
                        ++r_initial_block;
                    }
                }
            }
        }
    }
    return -1;

}



int BPSPCBlockTree::my_faster_next_block_fwdsearch(int initial_block, int d, int& e) {
    int r_initial_block = 0;
    int index_last = (top_excess_->size()+n_pre_last_level_)-1;
    if (initial_block < n_last_level_) {
        r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
        index_last = r_*(index_last+1);
    }
    else
        r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;
    bool going_up = true;
    while (true) {
        if (going_up) {
            if (r_initial_block == 0) break;
            while (r_initial_block%r_ != 0) {
                ++r_initial_block;
                if (r_initial_block >= number_of_blocks_first_level_+n_internal_nodes_) break;
                if (r_initial_block >= n_internal_nodes_) {
                    if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                    else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;;
                    if (initial_block >= number_of_blocks_first_level_) break;
                    if (e + 1-(int)((*(bt_prefix_min_excess_[0]))[initial_block]) <= d) {
                        return initial_block;
                    } else {
                        int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                        e += 2*rank_ones - first_level_length_;
                        /*
                        for (auto& pair: bt_ranks_) {
                            if (pair.first == open_) {
                                e += (*pair.second[0])[initial_block];
                            } else {
                                e -= (*pair.second[0])[initial_block];
                            }
                        }
                         */
                    }
                }
                else {
                    if (e + 1-(int)((*(top_min_excess_))[r_initial_block]) <= d) {
                        going_up = false;
                        break;
                    } else {
                        /*
                        int rank_ones = (*top_rank_ones_)[r_initial_block];
                        e += 2*rank_ones - level_length;
                         */
                        e += decoded_excess((*top_excess_)[r_initial_block]);
                    }
                }
            }

            // LOOKUP
            if (going_up && r_initial_block != index_last && r_initial_block+1 < number_of_blocks_first_level_+n_internal_nodes_) {
                if (r_initial_block+1 >= n_internal_nodes_) {
                    if (r_initial_block+1-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block+1-n_internal_nodes_;
                    else initial_block = r_initial_block+1 - n_pre_last_level_-n_internal_nodes_;
                    if (initial_block >= number_of_blocks_first_level_) break;
                    if (e + 1-(int)((*(bt_prefix_min_excess_[0]))[initial_block]) <= d) {
                        return initial_block;
                    }
                }
                else {
                    if (e + 1-(int)((*(top_min_excess_))[r_initial_block+1]) <= d) {
                        going_up = false;
                        ++r_initial_block;
                    }
                }
            }

            if (going_up) {
                r_initial_block = (r_initial_block-1)/r_;
                index_last = index_last/r_ - 1;
            }
        } else {
            if (r_initial_block >= n_internal_nodes_) {
                r_initial_block -= n_internal_nodes_;
                if (r_initial_block < n_pre_last_level_) return  n_last_level_ + r_initial_block;
                return  r_initial_block - n_pre_last_level_;
            } else {
                r_initial_block = r_initial_block*r_ +1;
                while (true) {
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;
                        if (initial_block >= number_of_blocks_first_level_) break;
                        if (e + 1-(int) ((*(bt_prefix_min_excess_[0]))[initial_block]) <= d) {
                            return initial_block;
                        } else {
                            int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                            e += 2*rank_ones - first_level_length_;
                            /*
                            for (auto& pair: bt_ranks_) {
                                if (pair.first == open_) {
                                    e += (*pair.second[0])[initial_block];
                                } else {
                                    e -= (*pair.second[0])[initial_block];
                                }
                            }
                             */
                        }
                        ++r_initial_block;
                    } else {
                        if (e + 1-(int) ((*(top_min_excess_))[r_initial_block]) <= d) {
                            break;
                        } else {
                            /*
                            int rank_ones = (*top_rank_ones_)[r_initial_block];
                            e += 2*rank_ones - level_length;
                             */
                            e += decoded_excess((*top_excess_)[r_initial_block]);
                        }
                        ++r_initial_block;
                    }
                }
            }
        }
    }
    return -1;

}

int BPSPCBlockTree::bwdsearch(int i, int d) {
    int e = 0;
    int a = bwdsearch(0,i%first_level_length_,d,0, i/first_level_length_, first_level_length_, e);
    if (a != input_.length()) return a + (i/first_level_length_)*first_level_length_;
    //int index = next_block_bwdsearch(i/first_level_length_, d , e);
    int index = faster_next_block_bwdsearch(i/first_level_length_, d , e);
    if (index == -1) return -1;
    a = bwdsearch(0,first_level_length_-1,d,0, index, first_level_length_, e);
    if (a != input_.length()) return a + index*first_level_length_;
    return -1;
}

int BPSPCBlockTree::bwdsearch(int i, int j, int d, int level, int level_index, int level_length, int& e) {

    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) {
        int excess = 0;
        int rank_ones = 0;
        if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
        else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
        excess += 2*rank_ones - (j+1);
        /*
        int excess2 = 0;
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess2 += (*pair.second[level])[level_index];
            } else {
                excess2 -= (*pair.second[level])[level_index];
            }
        }
        */

        int min_prefix = 1-((*(bt_prefix_min_excess_[level]))[level_index]);
        int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
        if (e + reached < -d) {
            e += excess;
            return input_.length();
        }
    }

    int a = input_.length();
    if (level == number_of_levels_-1) { // Case LeafBlock
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = (level_length*level_index + j < leaf_string_size) ? j : leaf_string_size - 1 - level_length*level_index;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (k-i >= bottom_bit_size_-1) {
            while (k - i >= bottom_bit_size_-1) {
                int index = (chunk_info >> ((level_length * level_index + k-bottom_bit_size_+1) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;
                int min_excess = bottom_min_excess_2_[index];
                int excess = bottom_excess_2_[index];
                int reached = ((excess - min_excess) > excess) ? (excess - min_excess) : excess;
                if (e + reached >= -d) {
                    break;
                }
                e += excess;
                k -= bottom_bit_size_;
            }
        }
*/
        for (; k >= i; --k) {
            e += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
                    //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == -d) return k-1;
            if ((level_length*level_index + k)%64 == 0) {
                chunk--;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        return input_.length();
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = j/child_length;
        int end_block = i/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_max_excess_[level+1]).size();
        if (rank*r_ + initial_block >= next_level_size) {
            initial_block = (next_level_size-1)%r_;
            j = (initial_block+1)*child_length-1;
        }
         */
        if (initial_block == end_block) {
            a = bwdsearch(i%child_length, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a !=input_.length()) return a + initial_block*child_length;
            return input_.length();
        } else {
            a = bwdsearch(0, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != input_.length()) return a + initial_block*child_length;
            for (int block = initial_block-1; block>end_block; --block) {
                a = bwdsearch(0, child_length - 1, d, level+1, rank*r_ + block, child_length, e);
                if (a != input_.length()) return a + block*child_length;
            }
            a = bwdsearch(i%child_length, child_length-1, d, level+1, rank*r_ + end_block, child_length, e);
            if (a != input_.length()) return a + end_block*child_length;
            return input_.length();
        }
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int first_block_excess = 0;
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                 if (first_block_excess != first_block_excess2)
                     std::cout << std::endl;
                     */
                int min_prefix_first_block = 1-((*(bt_prefix_min_excess_first_block_[level]))[back_index]);
                int reached = ((first_block_excess-min_prefix_first_block) > first_block_excess) ? (first_block_excess-min_prefix_first_block) : first_block_excess;
                if (e + reached < -d) {
                    e += first_block_excess;
                    return input_.length();
                }

            }
            a = bwdsearch(i + offset, j + offset, d, level, first_block, level_length, e);
            if (a != input_.length()) return a - offset;
            return input_.length();
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = 0;
                if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                if (second_block_excess != second_block_excess2)
                    std::cout << std::endl;
                    */
                int min_prefix_second_block = 1-((*(bt_prefix_min_excess_second_block_[level]))[back_index]);
                int reached = ((second_block_excess-min_prefix_second_block) > second_block_excess) ? (second_block_excess-min_prefix_second_block) : second_block_excess;
                if (e + reached < -d) {
                    e += second_block_excess;
                    return input_.length();
                }
            }
            a = bwdsearch(offset + i - level_length, offset + j - level_length, d, level, first_block + 1, level_length,
                          e);
            if (a != input_.length()) return a + level_length - offset;
            return input_.length();
        } else {
            if (j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = 0;
                if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                 if (second_block_excess != second_block_excess2)
                     std::cout << std::endl;
                     */
                int min_prefix_second_block = 1-((*(bt_prefix_min_excess_second_block_[level]))[back_index]);
                int reached = ((second_block_excess-min_prefix_second_block) > second_block_excess) ? (second_block_excess-min_prefix_second_block) : second_block_excess;
                if (e + reached < -d) {
                    e += second_block_excess;
                } else {
                    a = bwdsearch(0, offset + j - level_length, d, level, first_block + 1, level_length,
                                  e);
                    if (a != input_.length()) return a + level_length - offset;
                }
            } else {
                a = bwdsearch(0, offset + j - level_length, d, level, first_block + 1, level_length,
                              e);
                if (a != input_.length()) return a + level_length - offset;
            }


            if (i == 0) {
                int first_block_excess = 0;
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                if (first_block_excess != first_block_excess2)
                    std::cout << std::endl;
                */
                int min_prefix_first_block = 1-((*(bt_prefix_min_excess_first_block_[level]))[back_index]);
                int reached = ((first_block_excess-min_prefix_first_block) > first_block_excess) ? (first_block_excess-min_prefix_first_block) : first_block_excess;
                if (e + reached < -d) {
                    e += first_block_excess;
                } else {
                    a = bwdsearch(i + offset, level_length-1, d, level, first_block, level_length, e);
                    if (a != input_.length()) return a - offset;
                }

            } else {
                a = bwdsearch(i + offset, level_length - 1, d, level, first_block, level_length, e);
                if (a != input_.length()) return a - offset;
            }
            return a;
        }
    }
}

int BPSPCBlockTree::next_block_bwdsearch(int initial_block, int d, int& e) {
    for (int i = initial_block-1; i >= 0; --i) {
        int excess = 0;
        int rank_ones = (i == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[i-1])));
        excess += 2*rank_ones - first_level_length_ ;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess += (*pair.second[0])[i];
            } else {
                excess -= (*pair.second[0])[i];
            }
        }
         */
        int min_prefix = 1-((*(bt_prefix_min_excess_[0]))[i]);
        int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
        if (e + reached >= -d) {
            return i;
        } else {
            e+=excess;
        }
    }
    return -1;
}

int BPSPCBlockTree::faster_next_block_bwdsearch(int initial_block, int d, int& e) {
    int r_initial_block = 0;
    int index_first = (top_excess_->size()+n_pre_last_level_);
    if (initial_block < n_last_level_)
        r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
    else {
        r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;
        index_first = (index_first-1)/r_;
    }

    bool going_up = true;
    while (true) {
        if (going_up) {
            if (r_initial_block == 0) break;
            while ((r_initial_block-1)%r_ != 0) {
                --r_initial_block;
                //if (r_initial_block >= number_of_blocks_first_level_+n_internal_nodes_) break;
                if (r_initial_block >= n_internal_nodes_) {
                    if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                    else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;
                    //if (initial_block >= number_of_blocks_first_level_) break;

                    int excess = 0;
                    int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                    excess += 2*rank_ones - first_level_length_;
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[0])[initial_block];
                        } else {
                            excess -= (*pair.second[0])[initial_block];
                        }
                    }
                     */
                    int min_prefix = 1-((*(bt_prefix_min_excess_[0]))[initial_block]);
                    int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
                    if (e + reached >= -d) {
                        return initial_block;
                    } else {
                        e += excess;
                    }
                }
                else {
                    int excess = decoded_excess((*top_excess_)[r_initial_block]);
                    int min_prefix = 1-((*(top_min_excess_))[r_initial_block]);
                    int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
                    if (e + reached >= -d) {
                        going_up = false;
                        break;
                    } else {
                        e += excess;
                    }
                }
            }
            // LOOKUP
            if (going_up && r_initial_block != index_first) {
                if (r_initial_block-1 >= n_internal_nodes_) {
                    if (r_initial_block-1-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-1-n_internal_nodes_;
                    else initial_block = r_initial_block-1 - n_pre_last_level_-n_internal_nodes_;
                    int excess = 0;
                    int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                    excess += 2*rank_ones - first_level_length_;
                    int min_prefix = 1-((*(bt_prefix_min_excess_[0]))[initial_block]);
                    int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
                    if (e + reached >= -d) {
                        return initial_block;
                    }
                }
                else {
                    int excess = decoded_excess((*top_excess_)[r_initial_block-1]);
                    int min_prefix = 1-((*(top_min_excess_))[r_initial_block-1]);
                    int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
                    if (e + reached >= -d) {
                        going_up = false;
                        --r_initial_block;
                    }
                }
            }

            if (going_up) {
                r_initial_block = (r_initial_block-1)/r_;
                index_first = (index_first-1)/r_;
            }
        } else {
            if (r_initial_block >= n_internal_nodes_) {
                r_initial_block -= n_internal_nodes_;
                if (r_initial_block < n_pre_last_level_) return  n_last_level_ + r_initial_block;
                return  r_initial_block - n_pre_last_level_;
            } else {
                r_initial_block = r_initial_block*r_ +r_;
                if (r_initial_block >= n_internal_nodes_ + n_last_level_ + n_pre_last_level_)
                    r_initial_block = n_internal_nodes_ + n_last_level_ + n_pre_last_level_-1;
                while (true) {
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;
                        //if (initial_block >= number_of_blocks_first_level_) break;

                        int excess = 0;
                        int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                        excess += 2*rank_ones - first_level_length_;
                        /*
                        for (auto& pair: bt_ranks_) {
                            if (pair.first == open_) {
                                excess += (*pair.second[0])[initial_block];
                            } else {
                                excess -= (*pair.second[0])[initial_block];
                            }
                        }
                         */
                        int min_prefix = 1-((*(bt_prefix_min_excess_[0]))[initial_block]);
                        int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
                        if (e + reached >= -d) {
                            return initial_block;
                        } else {
                            e += excess;
                        }
                        --r_initial_block;
                    } else {
                        int excess = decoded_excess((*top_excess_)[r_initial_block]);
                        int min_prefix = 1-((*(top_min_excess_))[r_initial_block]);
                        int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
                        if (e + reached >= -d) {
                            break;
                        } else {
                            e += excess;
                        }
                        --r_initial_block;
                    }
                }
            }
        }
    }
    return -1;

}




int BPSPCBlockTree::c_bwdsearch(int i, int d) {
    int e = 0;
    int a = c_bwdsearch(0,i%first_level_length_,d,0, i/first_level_length_, first_level_length_, e);
    if (a != input_.length()) return a + (i/first_level_length_)*first_level_length_;
    //int index = next_block_bwdsearch(i/first_level_length_, d , e);
    int index = c_faster_next_block_bwdsearch(i/first_level_length_, d , e);
    if (index == -1) return -1;
    a = c_bwdsearch(0,first_level_length_-1,d,0, index, first_level_length_, e);
    if (a != input_.length()) return a + index*first_level_length_;
    return -1;
}

int BPSPCBlockTree::c_bwdsearch(int i, int j, int d, int level, int level_index, int level_length, int& e) {

    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*c_bt_prefix_min_excess_[level]).size()-1)))) {
        int excess = 0;
        int rank_ones = 0;
        if (level == 0) rank_ones = (level_index == 0) ? (*(c_bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(c_bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(c_bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
        else rank_ones = (level_index % r_ == 0) ? (*(c_bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(c_bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(c_bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
        excess += 2*rank_ones - (j+1);
        /*
        int excess2 = 0;
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess2 += (*pair.second[level])[level_index];
            } else {
                excess2 -= (*pair.second[level])[level_index];
            }
        }
        */

        int min_prefix = 1-((*(c_bt_prefix_min_excess_[level]))[level_index]);
        int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
        if (e + reached < -d) {
            e += excess;
            return input_.length();
        }
    }

    int a = input_.length();
    if (level == number_of_levels_-1) { // Case LeafBlock
        auto& leaf_string = *c_leaf_string_;
        int leaf_string_size = leaf_string.size();
        for (int k = (level_length*level_index + j < leaf_string_size) ? j : leaf_string_size - 1 - level_length*level_index; k >= i; --k) {
            e += (leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == -d) return k-1;
        }
        return input_.length();
    }
    if ((*c_bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = j/child_length;
        int end_block = i/child_length;
        int rank = (*c_bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_max_excess_[level+1]).size();
        if (rank*r_ + initial_block >= next_level_size) {
            initial_block = (next_level_size-1)%r_;
            j = (initial_block+1)*child_length-1;
        }
         */
        if (initial_block == end_block) {
            a = c_bwdsearch(i%child_length, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a !=input_.length()) return a + initial_block*child_length;
            return input_.length();
        } else {
            a = c_bwdsearch(0, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != input_.length()) return a + initial_block*child_length;
            for (int block = initial_block-1; block>end_block; --block) {
                a = c_bwdsearch(0, child_length - 1, d, level+1, rank*r_ + block, child_length, e);
                if (a != input_.length()) return a + block*child_length;
            }
            a = c_bwdsearch(i%child_length, child_length-1, d, level+1, rank*r_ + end_block, child_length, e);
            if (a != input_.length()) return a + end_block*child_length;
            return input_.length();
        }
    } else { // Case BackBlock
        int back_index = level_index - (*c_bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*c_bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int first_block_excess = 0;
                int rank_ones = (*c_bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                 if (first_block_excess != first_block_excess2)
                     std::cout << std::endl;
                     */
                int min_prefix_first_block = 1-((*(c_bt_prefix_min_excess_first_block_[level]))[back_index]);
                int reached = ((first_block_excess-min_prefix_first_block) > first_block_excess) ? (first_block_excess-min_prefix_first_block) : first_block_excess;
                if (e + reached < -d) {
                    e += first_block_excess;
                    return input_.length();
                }

            }
            a = c_bwdsearch(i + offset, j + offset, d, level, first_block, level_length, e);
            if (a != input_.length()) return a - offset;
            return input_.length();
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = 0;
                if (level == 0) rank_ones = (level_index == 0) ? (*(c_bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(c_bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(c_bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                else rank_ones = (level_index % r_ == 0) ? (*(c_bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(c_bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(c_bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*c_bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                if (second_block_excess != second_block_excess2)
                    std::cout << std::endl;
                    */
                int min_prefix_second_block = 1-((*(c_bt_prefix_min_excess_second_block_[level]))[back_index]);
                int reached = ((second_block_excess-min_prefix_second_block) > second_block_excess) ? (second_block_excess-min_prefix_second_block) : second_block_excess;
                if (e + reached < -d) {
                    e += second_block_excess;
                    return input_.length();
                }
            }
            a = c_bwdsearch(offset + i - level_length, offset + j - level_length, d, level, first_block + 1, level_length,
                          e);
            if (a != input_.length()) return a + level_length - offset;
            return input_.length();
        } else {
            if (j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = 0;
                if (level == 0) rank_ones = (level_index == 0) ? (*(c_bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(c_bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(c_bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                else rank_ones = (level_index % r_ == 0) ? (*(c_bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(c_bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(c_bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*c_bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                 if (second_block_excess != second_block_excess2)
                     std::cout << std::endl;
                     */
                int min_prefix_second_block = 1-((*(c_bt_prefix_min_excess_second_block_[level]))[back_index]);
                int reached = ((second_block_excess-min_prefix_second_block) > second_block_excess) ? (second_block_excess-min_prefix_second_block) : second_block_excess;
                if (e + reached < -d) {
                    e += second_block_excess;
                } else {
                    a = c_bwdsearch(0, offset + j - level_length, d, level, first_block + 1, level_length,
                                  e);
                    if (a != input_.length()) return a + level_length - offset;
                }
            } else {
                a = c_bwdsearch(0, offset + j - level_length, d, level, first_block + 1, level_length,
                              e);
                if (a != input_.length()) return a + level_length - offset;
            }


            if (i == 0) {
                int first_block_excess = 0;
                int rank_ones = (*c_bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                if (first_block_excess != first_block_excess2)
                    std::cout << std::endl;
                */
                int min_prefix_first_block = 1-((*(c_bt_prefix_min_excess_first_block_[level]))[back_index]);
                int reached = ((first_block_excess-min_prefix_first_block) > first_block_excess) ? (first_block_excess-min_prefix_first_block) : first_block_excess;
                if (e + reached < -d) {
                    e += first_block_excess;
                } else {
                    a = c_bwdsearch(i + offset, level_length-1, d, level, first_block, level_length, e);
                    if (a != input_.length()) return a - offset;
                }

            } else {
                a = c_bwdsearch(i + offset, level_length - 1, d, level, first_block, level_length, e);
                if (a != input_.length()) return a - offset;
            }
            return a;
        }
    }
}

int BPSPCBlockTree::c_next_block_bwdsearch(int initial_block, int d, int& e) {
    for (int i = initial_block-1; i >= 0; --i) {
        int excess = 0;
        int rank_ones = (i == 0) ? ((int)((*c_bt_first_level_cumulated_ranks_[open_])[i])) : ( ((int)((*c_bt_first_level_cumulated_ranks_[open_])[i])) - ((int)((*c_bt_first_level_cumulated_ranks_[open_])[i-1])));
        excess += 2*rank_ones - first_level_length_ ;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess += (*pair.second[0])[i];
            } else {
                excess -= (*pair.second[0])[i];
            }
        }
         */
        int min_prefix = 1-((*(c_bt_prefix_min_excess_[0]))[i]);
        int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
        if (e + reached >= -d) {
            return i;
        } else {
            e+=excess;
        }
    }
    return -1;
}

int BPSPCBlockTree::c_faster_next_block_bwdsearch(int initial_block, int d, int& e) {
    int r_initial_block = 0;
    int index_first = (top_excess_->size()+n_pre_last_level_);
    if (initial_block < n_last_level_)
        r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
    else {
        r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;
        index_first = (index_first-1)/r_;
    }

    bool going_up = true;
    while (true) {
        if (going_up) {
            if (r_initial_block == 0) break;
            while ((r_initial_block-1)%r_ != 0) {
                --r_initial_block;
                //if (r_initial_block >= number_of_blocks_first_level_+n_internal_nodes_) break;
                if (r_initial_block >= n_internal_nodes_) {
                    if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                    else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;
                    //if (initial_block >= number_of_blocks_first_level_) break;

                    int excess = 0;
                    int rank_ones = (initial_block == 0) ? ((int)((*c_bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*c_bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*c_bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                    excess += 2*rank_ones - first_level_length_;
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[0])[initial_block];
                        } else {
                            excess -= (*pair.second[0])[initial_block];
                        }
                    }
                     */
                    int min_prefix = 1-((*(c_bt_prefix_min_excess_[0]))[initial_block]);
                    int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
                    if (e + reached >= -d) {
                        return initial_block;
                    } else {
                        e += excess;
                    }
                }
                else {
                    int excess = decoded_excess((*c_top_excess_)[r_initial_block]);
                    int min_prefix = 1-((*(c_top_min_excess_))[r_initial_block]);
                    int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
                    if (e + reached >= -d) {
                        going_up = false;
                        break;
                    } else {
                        e += excess;
                    }
                }
            }
            // LOOKUP
            if (going_up && r_initial_block != index_first) {
                if (r_initial_block-1 >= n_internal_nodes_) {
                    if (r_initial_block-1-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-1-n_internal_nodes_;
                    else initial_block = r_initial_block-1 - n_pre_last_level_-n_internal_nodes_;
                    int excess = 0;
                    int rank_ones = (initial_block == 0) ? ((int)((*c_bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*c_bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*c_bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                    excess += 2*rank_ones - first_level_length_;
                    int min_prefix = 1-((*(c_bt_prefix_min_excess_[0]))[initial_block]);
                    int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
                    if (e + reached >= -d) {
                        return initial_block;
                    }
                }
                else {
                    int excess = decoded_excess((*c_top_excess_)[r_initial_block-1]);
                    int min_prefix = 1-((*(c_top_min_excess_))[r_initial_block-1]);
                    int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
                    if (e + reached >= -d) {
                        going_up = false;
                        --r_initial_block;
                    }
                }
            }

            if (going_up) {
                r_initial_block = (r_initial_block-1)/r_;
                index_first = (index_first-1)/r_;
            }
        } else {
            if (r_initial_block >= n_internal_nodes_) {
                r_initial_block -= n_internal_nodes_;
                if (r_initial_block < n_pre_last_level_) return  n_last_level_ + r_initial_block;
                return  r_initial_block - n_pre_last_level_;
            } else {
                r_initial_block = r_initial_block*r_ +r_;
                if (r_initial_block >= n_internal_nodes_ + n_last_level_ + n_pre_last_level_)
                    r_initial_block = n_internal_nodes_ + n_last_level_ + n_pre_last_level_-1;
                while (true) {
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;
                        //if (initial_block >= number_of_blocks_first_level_) break;

                        int excess = 0;
                        int rank_ones = (initial_block == 0) ? ((int)((*c_bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*c_bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*c_bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                        excess += 2*rank_ones - first_level_length_;
                        /*
                        for (auto& pair: bt_ranks_) {
                            if (pair.first == open_) {
                                excess += (*pair.second[0])[initial_block];
                            } else {
                                excess -= (*pair.second[0])[initial_block];
                            }
                        }
                         */
                        int min_prefix = 1-((*(c_bt_prefix_min_excess_[0]))[initial_block]);
                        int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
                        if (e + reached >= -d) {
                            return initial_block;
                        } else {
                            e += excess;
                        }
                        --r_initial_block;
                    } else {
                        int excess = decoded_excess((*c_top_excess_)[r_initial_block]);
                        int min_prefix = 1-((*(c_top_min_excess_))[r_initial_block]);
                        int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
                        if (e + reached >= -d) {
                            break;
                        } else {
                            e += excess;
                        }
                        --r_initial_block;
                    }
                }
            }
        }
    }
    return -1;

}



int BPSPCBlockTree::myc_bwdsearch(int i, int d) {
    int e = 0;
    int a = myc_bwdsearch(0,i%first_level_length_,d,0, i/first_level_length_, first_level_length_, e);
    if (a != input_.length()) return a + (i/first_level_length_)*first_level_length_;
    //int index = next_block_bwdsearch(i/first_level_length_, d , e);
    int index = myc_faster_next_block_bwdsearch(i/first_level_length_, d , e);
    if (index == -1) return -1;
    a = myc_bwdsearch(0,first_level_length_-1,d,0, index, first_level_length_, e);
    if (a != input_.length()) return a + index*first_level_length_;
    return -1;
}

int BPSPCBlockTree::myc_bwdsearch(int i, int j, int d, int level, int level_index, int level_length, int& e) {

    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*c_bt_prefix_min_excess_[level]).size()-1)))) {
        int excess = 0;
        int rank_ones = (*(c_bt_ranks_[open_][level]))[level_index];
        //if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
        //else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
        excess += 2*rank_ones - (j+1);
        /*
        int excess2 = 0;
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess2 += (*pair.second[level])[level_index];
            } else {
                excess2 -= (*pair.second[level])[level_index];
            }
        }
        */

        int min_prefix = 1-((*(c_bt_prefix_min_excess_[level]))[level_index]);
        int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
        if (e + reached < -d) {
            e += excess;
            return input_.length();
        }
    }

    int a = input_.length();
    if (level == number_of_levels_-1) { // Case LeafBlock
        auto& leaf_string = *c_leaf_string_;
        int leaf_string_size = leaf_string.size();
        for (int k = (level_length*level_index + j < leaf_string_size) ? j : leaf_string_size - 1 - level_length*level_index; k >= i; --k) {
            e += (leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == -d) return k-1;
        }
        return input_.length();
    }
    if ((*c_bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = j/child_length;
        int end_block = i/child_length;
        int rank = (*c_bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_max_excess_[level+1]).size();
        if (rank*r_ + initial_block >= next_level_size) {
            initial_block = (next_level_size-1)%r_;
            j = (initial_block+1)*child_length-1;
        }
         */
        if (initial_block == end_block) {
            a = myc_bwdsearch(i%child_length, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a !=input_.length()) return a + initial_block*child_length;
            return input_.length();
        } else {
            a = myc_bwdsearch(0, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != input_.length()) return a + initial_block*child_length;
            for (int block = initial_block-1; block>end_block; --block) {
                a = myc_bwdsearch(0, child_length - 1, d, level+1, rank*r_ + block, child_length, e);
                if (a != input_.length()) return a + block*child_length;
            }
            a = myc_bwdsearch(i%child_length, child_length-1, d, level+1, rank*r_ + end_block, child_length, e);
            if (a != input_.length()) return a + end_block*child_length;
            return input_.length();
        }
    } else { // Case BackBlock
        int back_index = level_index - (*c_bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*c_bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int first_block_excess = 0;
                int rank_ones = (*c_bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                 if (first_block_excess != first_block_excess2)
                     std::cout << std::endl;
                     */
                int min_prefix_first_block = 1-((*(c_bt_prefix_min_excess_first_block_[level]))[back_index]);
                int reached = ((first_block_excess-min_prefix_first_block) > first_block_excess) ? (first_block_excess-min_prefix_first_block) : first_block_excess;
                if (e + reached < -d) {
                    e += first_block_excess;
                    return input_.length();
                }

            }
            a = myc_bwdsearch(i + offset, j + offset, d, level, first_block, level_length, e);
            if (a != input_.length()) return a - offset;
            return input_.length();
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = (*(c_bt_ranks_[open_][level]))[level_index];
                //if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                //else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*c_bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                if (second_block_excess != second_block_excess2)
                    std::cout << std::endl;
                    */
                int min_prefix_second_block = 1-((*(c_bt_prefix_min_excess_second_block_[level]))[back_index]);
                int reached = ((second_block_excess-min_prefix_second_block) > second_block_excess) ? (second_block_excess-min_prefix_second_block) : second_block_excess;
                if (e + reached < -d) {
                    e += second_block_excess;
                    return input_.length();
                }
            }
            a = myc_bwdsearch(offset + i - level_length, offset + j - level_length, d, level, first_block + 1, level_length,
                            e);
            if (a != input_.length()) return a + level_length - offset;
            return input_.length();
        } else {
            if (j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = (*(c_bt_ranks_[open_][level]))[level_index];
                //if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                //else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*c_bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                 if (second_block_excess != second_block_excess2)
                     std::cout << std::endl;
                     */
                int min_prefix_second_block = 1-((*(c_bt_prefix_min_excess_second_block_[level]))[back_index]);
                int reached = ((second_block_excess-min_prefix_second_block) > second_block_excess) ? (second_block_excess-min_prefix_second_block) : second_block_excess;
                if (e + reached < -d) {
                    e += second_block_excess;
                } else {
                    a = myc_bwdsearch(0, offset + j - level_length, d, level, first_block + 1, level_length,
                                    e);
                    if (a != input_.length()) return a + level_length - offset;
                }
            } else {
                a = myc_bwdsearch(0, offset + j - level_length, d, level, first_block + 1, level_length,
                                e);
                if (a != input_.length()) return a + level_length - offset;
            }


            if (i == 0) {
                int first_block_excess = 0;
                int rank_ones = (*c_bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                if (first_block_excess != first_block_excess2)
                    std::cout << std::endl;
                */
                int min_prefix_first_block = 1-((*(c_bt_prefix_min_excess_first_block_[level]))[back_index]);
                int reached = ((first_block_excess-min_prefix_first_block) > first_block_excess) ? (first_block_excess-min_prefix_first_block) : first_block_excess;
                if (e + reached < -d) {
                    e += first_block_excess;
                } else {
                    a = myc_bwdsearch(i + offset, level_length-1, d, level, first_block, level_length, e);
                    if (a != input_.length()) return a - offset;
                }

            } else {
                a = myc_bwdsearch(i + offset, level_length - 1, d, level, first_block, level_length, e);
                if (a != input_.length()) return a - offset;
            }
            return a;
        }
    }
}

int BPSPCBlockTree::myc_next_block_bwdsearch(int initial_block, int d, int& e) {
    for (int i = initial_block-1; i >= 0; --i) {
        int excess = 0;
        int rank_ones = ((int)((*c_bt_ranks_[open_][0])[i]));
        //int rank_ones = (i == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[i-1])));
        excess += 2*rank_ones - first_level_length_ ;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess += (*pair.second[0])[i];
            } else {
                excess -= (*pair.second[0])[i];
            }
        }
         */
        int min_prefix = 1-((*(c_bt_prefix_min_excess_[0]))[i]);
        int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
        if (e + reached >= -d) {
            return i;
        } else {
            e+=excess;
        }
    }
    return -1;
}

int BPSPCBlockTree::myc_faster_next_block_bwdsearch(int initial_block, int d, int& e) {
    int r_initial_block = 0;
    int index_first = (c_top_excess_->size() + n_pre_last_level_);
    if (initial_block < n_last_level_)
        r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
    else {
        r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;
        index_first = (index_first - 1) / r_;
    }

    bool going_up = true;
    while (true) {
        if (going_up) {
            if (r_initial_block == 0) break;
            while ((r_initial_block - 1) % r_ != 0) {
                --r_initial_block;
                //if (r_initial_block >= number_of_blocks_first_level_+n_internal_nodes_) break;
                if (r_initial_block >= n_internal_nodes_) {
                    if (r_initial_block - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                 r_initial_block -
                                                                                                 n_internal_nodes_;
                    else initial_block = r_initial_block - n_pre_last_level_ - n_internal_nodes_;
                    //if (initial_block >= number_of_blocks_first_level_) break;

                    int excess = 0;
                    //int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                    int rank_ones = ((int) ((*c_bt_ranks_[open_][0])[initial_block]));
                    excess += 2 * rank_ones - first_level_length_;
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[0])[initial_block];
                        } else {
                            excess -= (*pair.second[0])[initial_block];
                        }
                    }
                     */
                    int min_prefix = 1 - ((*(c_bt_prefix_min_excess_[0]))[initial_block]);
                    int reached = ((excess - min_prefix) > excess) ? (excess - min_prefix) : excess;
                    if (e + reached >= -d) {
                        return initial_block;
                    } else {
                        e += excess;
                    }
                } else {
                    int excess = decoded_excess((*c_top_excess_)[r_initial_block]);
                    int min_prefix = 1 - ((*(c_top_min_excess_))[r_initial_block]);
                    int reached = ((excess - min_prefix) > excess) ? (excess - min_prefix) : excess;
                    if (e + reached >= -d) {
                        going_up = false;
                        break;
                    } else {
                        e += excess;
                    }
                }
            }
            // LOOKUP
            if (going_up && r_initial_block != index_first) {
                if (r_initial_block - 1 >= n_internal_nodes_) {
                    if (r_initial_block - 1 - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                     r_initial_block -
                                                                                                     1 -
                                                                                                     n_internal_nodes_;
                    else initial_block = r_initial_block - 1 - n_pre_last_level_ - n_internal_nodes_;
                    int excess = 0;
                    //int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                    int rank_ones = ((int) ((*c_bt_ranks_[open_][0])[initial_block]));
                    excess += 2 * rank_ones - first_level_length_;
                    int min_prefix = 1 - ((*(c_bt_prefix_min_excess_[0]))[initial_block]);
                    int reached = ((excess - min_prefix) > excess) ? (excess - min_prefix) : excess;
                    if (e + reached >= -d) {
                        return initial_block;
                    }
                } else {
                    int excess = decoded_excess((*c_top_excess_)[r_initial_block - 1]);
                    int min_prefix = 1 - ((*(c_top_min_excess_))[r_initial_block - 1]);
                    int reached = ((excess - min_prefix) > excess) ? (excess - min_prefix) : excess;
                    if (e + reached >= -d) {
                        going_up = false;
                        --r_initial_block;
                    }
                }
            }

            if (going_up) {
                r_initial_block = (r_initial_block - 1) / r_;
                index_first = (index_first - 1) / r_;
            }
        } else {
            if (r_initial_block >= n_internal_nodes_) {
                r_initial_block -= n_internal_nodes_;
                if (r_initial_block < n_pre_last_level_) return n_last_level_ + r_initial_block;
                return r_initial_block - n_pre_last_level_;
            } else {
                r_initial_block = r_initial_block * r_ + r_;
                if (r_initial_block >= n_internal_nodes_ + n_last_level_ + n_pre_last_level_)
                    r_initial_block = n_internal_nodes_ + n_last_level_ + n_pre_last_level_ - 1;
                while (true) {
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                     r_initial_block -
                                                                                                     n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_ - n_internal_nodes_;
                        //if (initial_block >= number_of_blocks_first_level_) break;

                        int excess = 0;
                        //int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                        int rank_ones = ((int) ((*c_bt_ranks_[open_][0])[initial_block]));
                        excess += 2 * rank_ones - first_level_length_;
                        /*
                        for (auto& pair: bt_ranks_) {
                            if (pair.first == open_) {
                                excess += (*pair.second[0])[initial_block];
                            } else {
                                excess -= (*pair.second[0])[initial_block];
                            }
                        }
                         */
                        int min_prefix = 1 - ((*(c_bt_prefix_min_excess_[0]))[initial_block]);
                        int reached = ((excess - min_prefix) > excess) ? (excess - min_prefix) : excess;
                        if (e + reached >= -d) {
                            return initial_block;
                        } else {
                            e += excess;
                        }
                        --r_initial_block;
                    } else {
                        int excess = decoded_excess((*c_top_excess_)[r_initial_block]);
                        int min_prefix = 1 - ((*(c_top_min_excess_))[r_initial_block]);
                        int reached = ((excess - min_prefix) > excess) ? (excess - min_prefix) : excess;
                        if (e + reached >= -d) {
                            break;
                        } else {
                            e += excess;
                        }
                        --r_initial_block;
                    }
                }
            }
        }
    }
    return -1;

}

int BPSPCBlockTree::my_bwdsearch_cback(int i, int d) {
    int e = 0;
    int a = my_bwdsearch_cback(0,i%first_level_length_,d,0, i/first_level_length_, first_level_length_, e);
    if (a != input_.length()) return a + (i/first_level_length_)*first_level_length_;
    //int index = next_block_bwdsearch(i/first_level_length_, d , e);
    int index = my_faster_next_block_bwdsearch(i/first_level_length_, d , e);
    if (index == -1) return -1;
    a = my_bwdsearch_cback(0,first_level_length_-1,d,0, index, first_level_length_, e);
    if (a != input_.length()) return a + index*first_level_length_;
    return -1;
}

int BPSPCBlockTree::my_bwdsearch_cback(int i, int j, int d, int level, int level_index, int level_length, int& e) {

    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) {
        int excess = 0;
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        //if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
        //else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
        excess += 2*rank_ones - (j+1);
        /*
        int excess2 = 0;
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess2 += (*pair.second[level])[level_index];
            } else {
                excess2 -= (*pair.second[level])[level_index];
            }
        }
        */

        int min_prefix = 1-((*(bt_prefix_min_excess_[level]))[level_index]);
        int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
        if (e + reached < -d) {
            e += excess;
            return input_.length();
        }
    }

    int a = input_.length();
    if (level == number_of_levels_-1) { // Case LeafBlock
        /*
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        for (int k = (level_length*level_index + j < leaf_string_size) ? j : leaf_string_size - 1 - level_length*level_index; k >= i; --k) {
            e += (leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == -d) return k-1;
        }
        return input_.length();*/
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = (level_length*level_index + j < leaf_string_size) ? j : leaf_string_size - 1 - level_length*level_index;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (k-i >= bottom_bit_size_-1) {
            while (k - i >= bottom_bit_size_-1) {
                int index = (chunk_info >> ((level_length * level_index + k-bottom_bit_size_+1) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;
                int min_excess = bottom_min_excess_2_[index];
                int excess = bottom_excess_2_[index];
                int reached = ((excess - min_excess) > excess) ? (excess - min_excess) : excess;
                if (e + reached >= -d) {
                    break;
                }
                e += excess;
                k -= bottom_bit_size_;
            }
        }
        */

        for (; k >= i; --k) {
            e += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == -d) return k-1;
            if ((level_length*level_index + k)%64 == 0) {
                chunk--;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        return input_.length();
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = j/child_length;
        int end_block = i/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_max_excess_[level+1]).size();
        if (rank*r_ + initial_block >= next_level_size) {
            initial_block = (next_level_size-1)%r_;
            j = (initial_block+1)*child_length-1;
        }
         */
        if (initial_block == end_block) {
            a = my_bwdsearch_cback(i%child_length, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a !=input_.length()) return a + initial_block*child_length;
            return input_.length();
        } else {
            a = my_bwdsearch_cback(0, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != input_.length()) return a + initial_block*child_length;
            for (int block = initial_block-1; block>end_block; --block) {
                a = my_bwdsearch_cback(0, child_length - 1, d, level+1, rank*r_ + block, child_length, e);
                if (a != input_.length()) return a + block*child_length;
            }
            a = my_bwdsearch_cback(i%child_length, child_length-1, d, level+1, rank*r_ + end_block, child_length, e);
            if (a != input_.length()) return a + end_block*child_length;
            return input_.length();
        }
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int first_block_excess = 0;
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                 if (first_block_excess != first_block_excess2)
                     std::cout << std::endl;
                     */
                int min_prefix_first_block = 1-((*(bt_prefix_min_excess_back_block_[level]))[back_index]);
                if (((*(bt_min_in_first_block_[level]))[back_index]) || ((*(bt_min_in_both_blocks_[level]))[back_index])) {
                    min_prefix_first_block = (1-((*(bt_prefix_min_excess_[level]))[level_index]));
                }

                int reached = ((first_block_excess-min_prefix_first_block) > first_block_excess) ? (first_block_excess-min_prefix_first_block) : first_block_excess;
                if (e + reached < -d) {
                    e += first_block_excess;
                    return input_.length();
                }

            }
            a = my_bwdsearch_cback(i + offset, j + offset, d, level, first_block, level_length, e);
            if (a != input_.length()) return a - offset;
            return input_.length();
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                //if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                //else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                if (second_block_excess != second_block_excess2)
                    std::cout << std::endl;
                    */

                int min_prefix_second_block = 1-((*(bt_prefix_min_excess_back_block_[level]))[back_index]);
                if (!((*(bt_min_in_first_block_[level]))[back_index]) && !((*(bt_min_in_both_blocks_[level]))[back_index])) {
                    int first_block_excess = 0;
                    //int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    first_block_excess += 2*rank_ones - (level_length-offset);
                    min_prefix_second_block = (1-((*(bt_prefix_min_excess_[level]))[level_index])) - first_block_excess;
                }

                int reached = ((second_block_excess-min_prefix_second_block) > second_block_excess) ? (second_block_excess-min_prefix_second_block) : second_block_excess;
                if (e + reached < -d) {
                    e += second_block_excess;
                    return input_.length();
                }
            }
            a = my_bwdsearch_cback(offset + i - level_length, offset + j - level_length, d, level, first_block + 1, level_length,
                             e);
            if (a != input_.length()) return a + level_length - offset;
            return input_.length();
        } else {
            if (j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                //if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                //else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                 if (second_block_excess != second_block_excess2)
                     std::cout << std::endl;
                     */
                int min_prefix_second_block = 1-((*(bt_prefix_min_excess_back_block_[level]))[back_index]);
                if (!((*(bt_min_in_first_block_[level]))[back_index]) && !((*(bt_min_in_both_blocks_[level]))[back_index])) {
                    int first_block_excess = 0;
                    //int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                    first_block_excess += 2*rank_ones - (level_length-offset);
                    min_prefix_second_block = (1-((*(bt_prefix_min_excess_[level]))[level_index])) - first_block_excess;
                }
                int reached = ((second_block_excess-min_prefix_second_block) > second_block_excess) ? (second_block_excess-min_prefix_second_block) : second_block_excess;
                if (e + reached < -d) {
                    e += second_block_excess;
                } else {
                    a = my_bwdsearch_cback(0, offset + j - level_length, d, level, first_block + 1, level_length,
                                     e);
                    if (a != input_.length()) return a + level_length - offset;
                }
            } else {
                a = my_bwdsearch_cback(0, offset + j - level_length, d, level, first_block + 1, level_length,
                                 e);
                if (a != input_.length()) return a + level_length - offset;
            }


            if (i == 0) {
                int first_block_excess = 0;
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                if (first_block_excess != first_block_excess2)
                    std::cout << std::endl;
                */
                int min_prefix_first_block = 1-((*(bt_prefix_min_excess_back_block_[level]))[back_index]);
                if (((*(bt_min_in_first_block_[level]))[back_index]) || ((*(bt_min_in_both_blocks_[level]))[back_index])) {
                    min_prefix_first_block = (1-((*(bt_prefix_min_excess_[level]))[level_index]));
                }

                int reached = ((first_block_excess-min_prefix_first_block) > first_block_excess) ? (first_block_excess-min_prefix_first_block) : first_block_excess;
                if (e + reached < -d) {
                    e += first_block_excess;
                } else {
                    a = my_bwdsearch_cback(i + offset, level_length-1, d, level, first_block, level_length, e);
                    if (a != input_.length()) return a - offset;
                }

            } else {
                a = my_bwdsearch_cback(i + offset, level_length - 1, d, level, first_block, level_length, e);
                if (a != input_.length()) return a - offset;
            }
            return a;
        }
    }
}


int BPSPCBlockTree::my_bwdsearch(int i, int d) {
    int e = 0;
    int a = my_bwdsearch(0,i%first_level_length_,d,0, i/first_level_length_, first_level_length_, e);
    if (a != input_.length()) return a + (i/first_level_length_)*first_level_length_;
    //int index = next_block_bwdsearch(i/first_level_length_, d , e);
    int index = my_faster_next_block_bwdsearch(i/first_level_length_, d , e);
    if (index == -1) return -1;
    a = my_bwdsearch(0,first_level_length_-1,d,0, index, first_level_length_, e);
    if (a != input_.length()) return a + index*first_level_length_;
    return -1;
}

int BPSPCBlockTree::my_bwdsearch_nobackmin(int i, int d) {
    int e = 0;
    int a = my_bwdsearch_nobackmin(0,i%first_level_length_,d,0, i/first_level_length_, first_level_length_, e);
    if (a != input_.length()) return a + (i/first_level_length_)*first_level_length_;
    //int index = next_block_bwdsearch(i/first_level_length_, d , e);
    int index = my_faster_next_block_bwdsearch(i/first_level_length_, d , e);
    if (index == -1) return -1;
    a = my_bwdsearch_nobackmin(0,first_level_length_-1,d,0, index, first_level_length_, e);
    if (a != input_.length()) return a + index*first_level_length_;
    return -1;
}

int BPSPCBlockTree::my_bwdsearch(int i, int j, int d, int level, int level_index, int level_length, int& e) {

    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) {
        int excess = 0;
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        //if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
        //else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
        excess += 2*rank_ones - (j+1);
        /*
        int excess2 = 0;
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess2 += (*pair.second[level])[level_index];
            } else {
                excess2 -= (*pair.second[level])[level_index];
            }
        }
        */

        int min_prefix = 1-((*(bt_prefix_min_excess_[level]))[level_index]);
        int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
        if (e + reached < -d) {
            e += excess;
            return input_.length();
        }
    }

    int a = input_.length();
    if (level == number_of_levels_-1) { // Case LeafBlock
        /*
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        for (int k = (level_length*level_index + j < leaf_string_size) ? j : leaf_string_size - 1 - level_length*level_index; k >= i; --k) {
            e += (leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == -d) return k-1;
        }
        return input_.length();*/
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = (level_length*level_index + j < leaf_string_size) ? j : leaf_string_size - 1 - level_length*level_index;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (k-i >= bottom_bit_size_-1) {
            while (k - i >= bottom_bit_size_-1) {
                int index = (chunk_info >> ((level_length * level_index + k-bottom_bit_size_+1) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;
                int min_excess = bottom_min_excess_2_[index];
                int excess = bottom_excess_2_[index];
                int reached = ((excess - min_excess) > excess) ? (excess - min_excess) : excess;
                if (e + reached >= -d) {
                    break;
                }
                e += excess;
                k -= bottom_bit_size_;
            }
        }
        */

        for (; k >= i; --k) {
            e += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == -d) return k-1;
            if ((level_length*level_index + k)%64 == 0) {
                chunk--;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        return input_.length();
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = j/child_length;
        int end_block = i/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_max_excess_[level+1]).size();
        if (rank*r_ + initial_block >= next_level_size) {
            initial_block = (next_level_size-1)%r_;
            j = (initial_block+1)*child_length-1;
        }
         */
        if (initial_block == end_block) {
            a = my_bwdsearch(i%child_length, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a !=input_.length()) return a + initial_block*child_length;
            return input_.length();
        } else {
            a = my_bwdsearch(0, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != input_.length()) return a + initial_block*child_length;
            for (int block = initial_block-1; block>end_block; --block) {
                a = my_bwdsearch(0, child_length - 1, d, level+1, rank*r_ + block, child_length, e);
                if (a != input_.length()) return a + block*child_length;
            }
            a = my_bwdsearch(i%child_length, child_length-1, d, level+1, rank*r_ + end_block, child_length, e);
            if (a != input_.length()) return a + end_block*child_length;
            return input_.length();
        }
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int first_block_excess = 0;
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                 if (first_block_excess != first_block_excess2)
                     std::cout << std::endl;
                     */
                int min_prefix_first_block = 1-((*(bt_prefix_min_excess_first_block_[level]))[back_index]);
                int reached = ((first_block_excess-min_prefix_first_block) > first_block_excess) ? (first_block_excess-min_prefix_first_block) : first_block_excess;
                if (e + reached < -d) {
                    e += first_block_excess;
                    return input_.length();
                }

            }
            a = my_bwdsearch(i + offset, j + offset, d, level, first_block, level_length, e);
            if (a != input_.length()) return a - offset;
            return input_.length();
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                //if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                //else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                if (second_block_excess != second_block_excess2)
                    std::cout << std::endl;
                    */
                int min_prefix_second_block = 1-((*(bt_prefix_min_excess_second_block_[level]))[back_index]);
                int reached = ((second_block_excess-min_prefix_second_block) > second_block_excess) ? (second_block_excess-min_prefix_second_block) : second_block_excess;
                if (e + reached < -d) {
                    e += second_block_excess;
                    return input_.length();
                }
            }
            a = my_bwdsearch(offset + i - level_length, offset + j - level_length, d, level, first_block + 1, level_length,
                             e);
            if (a != input_.length()) return a + level_length - offset;
            return input_.length();
        } else {
            if (j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                //if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                //else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                 if (second_block_excess != second_block_excess2)
                     std::cout << std::endl;
                     */
                int min_prefix_second_block = 1-((*(bt_prefix_min_excess_second_block_[level]))[back_index]);
                int reached = ((second_block_excess-min_prefix_second_block) > second_block_excess) ? (second_block_excess-min_prefix_second_block) : second_block_excess;
                if (e + reached < -d) {
                    e += second_block_excess;
                } else {
                    a = my_bwdsearch(0, offset + j - level_length, d, level, first_block + 1, level_length,
                                     e);
                    if (a != input_.length()) return a + level_length - offset;
                }
            } else {
                a = my_bwdsearch(0, offset + j - level_length, d, level, first_block + 1, level_length,
                                 e);
                if (a != input_.length()) return a + level_length - offset;
            }


            if (i == 0) {
                int first_block_excess = 0;
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                if (first_block_excess != first_block_excess2)
                    std::cout << std::endl;
                */
                int min_prefix_first_block = 1-((*(bt_prefix_min_excess_first_block_[level]))[back_index]);
                int reached = ((first_block_excess-min_prefix_first_block) > first_block_excess) ? (first_block_excess-min_prefix_first_block) : first_block_excess;
                if (e + reached < -d) {
                    e += first_block_excess;
                } else {
                    a = my_bwdsearch(i + offset, level_length-1, d, level, first_block, level_length, e);
                    if (a != input_.length()) return a - offset;
                }

            } else {
                a = my_bwdsearch(i + offset, level_length - 1, d, level, first_block, level_length, e);
                if (a != input_.length()) return a - offset;
            }
            return a;
        }
    }
}

int BPSPCBlockTree::my_bwdsearch_nobackmin(int i, int j, int d, int level, int level_index, int level_length, int& e) {

    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) {
        int excess = 0;
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        //if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
        //else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
        excess += 2*rank_ones - (j+1);
        /*
        int excess2 = 0;
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess2 += (*pair.second[level])[level_index];
            } else {
                excess2 -= (*pair.second[level])[level_index];
            }
        }
        */

        int min_prefix = 1-((*(bt_prefix_min_excess_[level]))[level_index]);
        int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
        if (e + reached < -d) {
            e += excess;
            return input_.length();
        }
    }

    int a = input_.length();
    if (level == number_of_levels_-1) { // Case LeafBlock
        /*
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        for (int k = (level_length*level_index + j < leaf_string_size) ? j : leaf_string_size - 1 - level_length*level_index; k >= i; --k) {
            e += (leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == -d) return k-1;
        }
        return input_.length();*/
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = (level_length*level_index + j < leaf_string_size) ? j : leaf_string_size - 1 - level_length*level_index;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (k-i >= bottom_bit_size_-1) {
            while (k - i >= bottom_bit_size_-1) {
                int index = (chunk_info >> ((level_length * level_index + k-bottom_bit_size_+1) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;
                int min_excess = bottom_min_excess_2_[index];
                int excess = bottom_excess_2_[index];
                int reached = ((excess - min_excess) > excess) ? (excess - min_excess) : excess;
                if (e + reached >= -d) {
                    break;
                }
                e += excess;
                k -= bottom_bit_size_;
            }
        }
        */

        for (; k >= i; --k) {
            e += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == -d) return k-1;
            if ((level_length*level_index + k)%64 == 0) {
                chunk--;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        return input_.length();
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = j/child_length;
        int end_block = i/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_max_excess_[level+1]).size();
        if (rank*r_ + initial_block >= next_level_size) {
            initial_block = (next_level_size-1)%r_;
            j = (initial_block+1)*child_length-1;
        }
         */
        if (initial_block == end_block) {
            a = my_bwdsearch_nobackmin(i%child_length, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a !=input_.length()) return a + initial_block*child_length;
            return input_.length();
        } else {
            a = my_bwdsearch_nobackmin(0, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != input_.length()) return a + initial_block*child_length;
            for (int block = initial_block-1; block>end_block; --block) {
                a = my_bwdsearch_nobackmin(0, child_length - 1, d, level+1, rank*r_ + block, child_length, e);
                if (a != input_.length()) return a + block*child_length;
            }
            a = my_bwdsearch_nobackmin(i%child_length, child_length-1, d, level+1, rank*r_ + end_block, child_length, e);
            if (a != input_.length()) return a + end_block*child_length;
            return input_.length();
        }
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            /*
            if (i == 0 && j + offset == level_length - 1) {
                int first_block_excess = 0;
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                */
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                 if (first_block_excess != first_block_excess2)
                     std::cout << std::endl;
                     */
                /*
                int min_prefix_first_block = 1-((*(bt_prefix_min_excess_first_block_[level]))[back_index]);
                int reached = ((first_block_excess-min_prefix_first_block) > first_block_excess) ? (first_block_excess-min_prefix_first_block) : first_block_excess;
                if (e + reached < -d) {
                    e += first_block_excess;
                    return input_.length();
                }

            }*/
            a = my_bwdsearch_nobackmin(i + offset, j + offset, d, level, first_block, level_length, e);
            if (a != input_.length()) return a - offset;
            return input_.length();
        } else if (i + offset >= level_length && j + offset >= level_length) {
            /*
            if (i + offset == level_length && j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                //if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                //else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                */
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                if (second_block_excess != second_block_excess2)
                    std::cout << std::endl;
                    */
                /*
                int min_prefix_second_block = 1-((*(bt_prefix_min_excess_second_block_[level]))[back_index]);
                int reached = ((second_block_excess-min_prefix_second_block) > second_block_excess) ? (second_block_excess-min_prefix_second_block) : second_block_excess;
                if (e + reached < -d) {
                    e += second_block_excess;
                    return input_.length();
                }
            }
                 */
            a = my_bwdsearch_nobackmin(offset + i - level_length, offset + j - level_length, d, level, first_block + 1, level_length,
                             e);
            if (a != input_.length()) return a + level_length - offset;
            return input_.length();
        } else {
            /*
            if (j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                //if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                //else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                */
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                 if (second_block_excess != second_block_excess2)
                     std::cout << std::endl;
                     */
                /*
                int min_prefix_second_block = 1-((*(bt_prefix_min_excess_second_block_[level]))[back_index]);
                int reached = ((second_block_excess-min_prefix_second_block) > second_block_excess) ? (second_block_excess-min_prefix_second_block) : second_block_excess;
                if (e + reached < -d) {
                    e += second_block_excess;
                } else {
                    a = my_bwdsearch_nobackmin(0, offset + j - level_length, d, level, first_block + 1, level_length,
                                     e);
                    if (a != input_.length()) return a + level_length - offset;
                }
            } else {*/
                a = my_bwdsearch_nobackmin(0, offset + j - level_length, d, level, first_block + 1, level_length,
                                 e);
                if (a != input_.length()) return a + level_length - offset;
                /*
            }
                 */

/*
            if (i == 0) {
                int first_block_excess = 0;
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                */
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                if (first_block_excess != first_block_excess2)
                    std::cout << std::endl;
                */
                /*
                int min_prefix_first_block = 1-((*(bt_prefix_min_excess_first_block_[level]))[back_index]);
                int reached = ((first_block_excess-min_prefix_first_block) > first_block_excess) ? (first_block_excess-min_prefix_first_block) : first_block_excess;
                if (e + reached < -d) {
                    e += first_block_excess;
                } else {
                    a = my_bwdsearch_nobackmin(i + offset, level_length-1, d, level, first_block, level_length, e);
                    if (a != input_.length()) return a - offset;
                }

            } else {*/
                a = my_bwdsearch_nobackmin(i + offset, level_length - 1, d, level, first_block, level_length, e);
                if (a != input_.length()) return a - offset;
            /*}*/
            return a;
        }
    }
}


int BPSPCBlockTree::my_next_block_bwdsearch(int initial_block, int d, int& e) {
    for (int i = initial_block-1; i >= 0; --i) {
        int excess = 0;
        int rank_ones = ((int)((*bt_ranks_[open_][0])[i]));
        //int rank_ones = (i == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[i-1])));
        excess += 2*rank_ones - first_level_length_ ;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess += (*pair.second[0])[i];
            } else {
                excess -= (*pair.second[0])[i];
            }
        }
         */
        int min_prefix = 1-((*(bt_prefix_min_excess_[0]))[i]);
        int reached = ((excess-min_prefix) > excess) ? (excess-min_prefix) : excess;
        if (e + reached >= -d) {
            return i;
        } else {
            e+=excess;
        }
    }
    return -1;
}

int BPSPCBlockTree::my_faster_next_block_bwdsearch(int initial_block, int d, int& e) {
    int r_initial_block = 0;
    int index_first = (top_excess_->size() + n_pre_last_level_);
    if (initial_block < n_last_level_)
        r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
    else {
        r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;
        index_first = (index_first - 1) / r_;
    }

    bool going_up = true;
    while (true) {
        if (going_up) {
            if (r_initial_block == 0) break;
            while ((r_initial_block - 1) % r_ != 0) {
                --r_initial_block;
                //if (r_initial_block >= number_of_blocks_first_level_+n_internal_nodes_) break;
                if (r_initial_block >= n_internal_nodes_) {
                    if (r_initial_block - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                 r_initial_block -
                                                                                                 n_internal_nodes_;
                    else initial_block = r_initial_block - n_pre_last_level_ - n_internal_nodes_;
                    //if (initial_block >= number_of_blocks_first_level_) break;

                    int excess = 0;
                    //int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                    int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                    excess += 2 * rank_ones - first_level_length_;
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[0])[initial_block];
                        } else {
                            excess -= (*pair.second[0])[initial_block];
                        }
                    }
                     */
                    int min_prefix = 1 - ((*(bt_prefix_min_excess_[0]))[initial_block]);
                    int reached = ((excess - min_prefix) > excess) ? (excess - min_prefix) : excess;
                    if (e + reached >= -d) {
                        return initial_block;
                    } else {
                        e += excess;
                    }
                } else {
                    int excess = decoded_excess((*top_excess_)[r_initial_block]);
                    int min_prefix = 1 - ((*(top_min_excess_))[r_initial_block]);
                    int reached = ((excess - min_prefix) > excess) ? (excess - min_prefix) : excess;
                    if (e + reached >= -d) {
                        going_up = false;
                        break;
                    } else {
                        e += excess;
                    }
                }
            }
            // LOOKUP
            if (going_up && r_initial_block != index_first) {
                if (r_initial_block - 1 >= n_internal_nodes_) {
                    if (r_initial_block - 1 - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                     r_initial_block -
                                                                                                     1 -
                                                                                                     n_internal_nodes_;
                    else initial_block = r_initial_block - 1 - n_pre_last_level_ - n_internal_nodes_;
                    int excess = 0;
                    //int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                    int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                    excess += 2 * rank_ones - first_level_length_;
                    int min_prefix = 1 - ((*(bt_prefix_min_excess_[0]))[initial_block]);
                    int reached = ((excess - min_prefix) > excess) ? (excess - min_prefix) : excess;
                    if (e + reached >= -d) {
                        return initial_block;
                    }
                } else {
                    int excess = decoded_excess((*top_excess_)[r_initial_block - 1]);
                    int min_prefix = 1 - ((*(top_min_excess_))[r_initial_block - 1]);
                    int reached = ((excess - min_prefix) > excess) ? (excess - min_prefix) : excess;
                    if (e + reached >= -d) {
                        going_up = false;
                        --r_initial_block;
                    }
                }
            }

            if (going_up) {
                r_initial_block = (r_initial_block - 1) / r_;
                index_first = (index_first - 1) / r_;
            }
        } else {
            if (r_initial_block >= n_internal_nodes_) {
                r_initial_block -= n_internal_nodes_;
                if (r_initial_block < n_pre_last_level_) return n_last_level_ + r_initial_block;
                return r_initial_block - n_pre_last_level_;
            } else {
                r_initial_block = r_initial_block * r_ + r_;
                if (r_initial_block >= n_internal_nodes_ + n_last_level_ + n_pre_last_level_)
                    r_initial_block = n_internal_nodes_ + n_last_level_ + n_pre_last_level_ - 1;
                while (true) {
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block - n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ +
                                                                                                     r_initial_block -
                                                                                                     n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_ - n_internal_nodes_;
                        //if (initial_block >= number_of_blocks_first_level_) break;

                        int excess = 0;
                        //int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                        int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                        excess += 2 * rank_ones - first_level_length_;
                        /*
                        for (auto& pair: bt_ranks_) {
                            if (pair.first == open_) {
                                excess += (*pair.second[0])[initial_block];
                            } else {
                                excess -= (*pair.second[0])[initial_block];
                            }
                        }
                         */
                        int min_prefix = 1 - ((*(bt_prefix_min_excess_[0]))[initial_block]);
                        int reached = ((excess - min_prefix) > excess) ? (excess - min_prefix) : excess;
                        if (e + reached >= -d) {
                            return initial_block;
                        } else {
                            e += excess;
                        }
                        --r_initial_block;
                    } else {
                        int excess = decoded_excess((*top_excess_)[r_initial_block]);
                        int min_prefix = 1 - ((*(top_min_excess_))[r_initial_block]);
                        int reached = ((excess - min_prefix) > excess) ? (excess - min_prefix) : excess;
                        if (e + reached >= -d) {
                            break;
                        } else {
                            e += excess;
                        }
                        --r_initial_block;
                    }
                }
            }
        }
    }
    return -1;

}


int BPSPCBlockTree::positive_fwdsearch(int i, int j, int d, int level, int level_index, int level_length, int& e) {
    if (i == -1 && j == level_length-1 && e + -1+(int)((*(bt_prefix_max_excess_[level]))[level_index]) < d) {
        int rank_ones = 0;
        if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
        else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
        e += 2*rank_ones - level_length;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[level_index];
            } else {
                e -= (*pair.second[level])[level_index];
            }
        }
         */
        return -1;
    }

    int a = -1;
    if (level == number_of_levels_-1) { // Case LeafBlock
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i+1;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;

                if (e + bottom_max_excess_2_[index] >= d) {
                    break;
                }
                e += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
*/
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            e += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == d) return k;
            if ((level_length*level_index + k + 1)%64 == 0) {
                ++chunk;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        return -1;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = (i+1)/child_length;
        int end_block = j/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
        if (initial_block == end_block) {
            a = positive_fwdsearch(((i+1)%child_length) - 1, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != -1) return a + initial_block*child_length;
            return -1;
        } else {
            a = positive_fwdsearch(((i+1)%child_length) - 1, child_length-1, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != -1) return a + initial_block*child_length;
            for (int block = initial_block+1; block<end_block; ++block) {
                a = positive_fwdsearch(-1, child_length - 1, d, level+1, rank*r_ + block, child_length, e);
                if (a != -1) return a + block*child_length;
            }
            a = positive_fwdsearch(-1, j%child_length, d, level+1, rank*r_ + end_block, child_length, e);
            if (a != -1) return a + end_block*child_length;
            return -1;
        }
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + 1 + offset < level_length && j + offset < level_length) {
            if (i + 1 == 0 && j + offset == level_length - 1 &&
                e + -1+(int)((*bt_prefix_max_excess_first_block_[level])[back_index]) < d) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                return -1;
            }
            a = positive_fwdsearch(i + offset, j + offset, d, level, first_block, level_length, e);
            if (a != -1) return a - offset;
            return -1;
        } else if (i + 1 + offset >= level_length && j + offset >= level_length) {
            if (i + 1 + offset == level_length && j == level_length - 1 &&
                e + -1+(int)((*bt_prefix_max_excess_second_block_[level])[back_index]) < d) {
                int rank_ones = 0;
                if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                return -1;
            }
            a = positive_fwdsearch(offset + i - level_length, offset + j - level_length, d, level, first_block + 1, level_length,
                          e);
            if (a != -1) return a + level_length - offset;
            return -1;
        } else {
            if (i + 1 == 0 && e + -1+(int)((*bt_prefix_max_excess_first_block_[level])[back_index]) < d) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
            } else {
                a = positive_fwdsearch(i + offset, level_length - 1, d, level, first_block, level_length, e);
                if (a != -1) return a - offset;
            }

            if (j == level_length - 1 && e + -1+(int)((*bt_prefix_max_excess_second_block_[level])[back_index]) < d) {
                int rank_ones = 0;
                if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }*/
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
            } else {
                a = positive_fwdsearch(-1, offset + j - level_length, d, level, first_block + 1, level_length, e);
                if (a != -1) return a + level_length - offset;
            }
            return a;
        }
    }
}

int BPSPCBlockTree::my_positive_fwdsearch(int i, int j, int d, int level, int level_index, int level_length, int& e) {
    if (i == -1 && j == level_length-1 && e + -1+(int)((*(bt_prefix_max_excess_[level]))[level_index]) < d) {
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        e += 2*rank_ones - level_length;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[level_index];
            } else {
                e -= (*pair.second[level])[level_index];
            }
        }
         */
        return -1;
    }

    int a = -1;
    if (level == number_of_levels_-1) { // Case LeafBlock
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i+1;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;

                if (e + bottom_max_excess_2_[index] >= d) {
                    break;
                }
                e += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
*/
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            e += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == d) return k;
            if ((level_length*level_index + k + 1)%64 == 0) {
                ++chunk;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        return -1;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = (i+1)/child_length;
        int end_block = j/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
        if (initial_block == end_block) {
            a = my_positive_fwdsearch(((i+1)%child_length) - 1, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != -1) return a + initial_block*child_length;
            return -1;
        } else {
            a = my_positive_fwdsearch(((i+1)%child_length) - 1, child_length-1, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != -1) return a + initial_block*child_length;
            for (int block = initial_block+1; block<end_block; ++block) {
                a = my_positive_fwdsearch(-1, child_length - 1, d, level+1, rank*r_ + block, child_length, e);
                if (a != -1) return a + block*child_length;
            }
            a = my_positive_fwdsearch(-1, j%child_length, d, level+1, rank*r_ + end_block, child_length, e);
            if (a != -1) return a + end_block*child_length;
            return -1;
        }
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + 1 + offset < level_length && j + offset < level_length) {
            if (i + 1 == 0 && j + offset == level_length - 1 &&
                e + -1+(int)((*bt_prefix_max_excess_first_block_[level])[back_index]) < d) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                return -1;
            }
            a = my_positive_fwdsearch(i + offset, j + offset, d, level, first_block, level_length, e);
            if (a != -1) return a - offset;
            return -1;
        } else if (i + 1 + offset >= level_length && j + offset >= level_length) {
            if (i + 1 + offset == level_length && j == level_length - 1 &&
                e + -1+(int)((*bt_prefix_max_excess_second_block_[level])[back_index]) < d) {
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                return -1;
            }
            a = my_positive_fwdsearch(offset + i - level_length, offset + j - level_length, d, level, first_block + 1, level_length,
                                   e);
            if (a != -1) return a + level_length - offset;
            return -1;
        } else {
            if (i + 1 == 0 && e + -1+(int)((*bt_prefix_max_excess_first_block_[level])[back_index]) < d) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
            } else {
                a = my_positive_fwdsearch(i + offset, level_length - 1, d, level, first_block, level_length, e);
                if (a != -1) return a - offset;
            }

            if (j == level_length - 1 && e + -1+(int)((*bt_prefix_max_excess_second_block_[level])[back_index]) < d) {
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                e += 2*rank_ones - level_length;
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }*/
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
            } else {
                a = my_positive_fwdsearch(-1, offset + j - level_length, d, level, first_block + 1, level_length, e);
                if (a != -1) return a + level_length - offset;
            }
            return a;
        }
    }
}


int BPSPCBlockTree::my_positive_fwdsearch_nobackmax(int i, int j, int d, int level, int level_index, int level_length, int& e) {
    if (i == -1 && j == level_length-1 && e + -1+(int)((*(bt_prefix_max_excess_[level]))[level_index]) < d) {
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        e += 2*rank_ones - level_length;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                e += (*pair.second[level])[level_index];
            } else {
                e -= (*pair.second[level])[level_index];
            }
        }
         */
        return -1;
    }

    int a = -1;
    if (level == number_of_levels_-1) { // Case LeafBlock
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = i+1;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
            while (j-k >= bottom_bit_size_-1 && -level_length*level_index - k +leaf_string_size>= bottom_bit_size_) {
                int index = (chunk_info >> ((level_length * level_index + k) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;

                if (e + bottom_max_excess_2_[index] >= d) {
                    break;
                }
                e += bottom_excess_2_[index];;
                k += bottom_bit_size_;
            }
        }
*/
        for (; k <= j && level_length*level_index + k < leaf_string_size; ++k) {
            e += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == d) return k;
            if ((level_length*level_index + k + 1)%64 == 0) {
                ++chunk;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        return -1;
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = (i+1)/child_length;
        int end_block = j/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        int next_level_size = (*bt_prefix_min_excess_[level+1]).size();
        if (rank*r_ + end_block >= next_level_size) {
            end_block = (next_level_size-1)%r_;
            j = (end_block+1)*child_length-1;
        }
        if (rank*r_ + initial_block >= next_level_size) return -1;
        if (initial_block == end_block) {
            a = my_positive_fwdsearch_nobackmax(((i+1)%child_length) - 1, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != -1) return a + initial_block*child_length;
            return -1;
        } else {
            a = my_positive_fwdsearch_nobackmax(((i+1)%child_length) - 1, child_length-1, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != -1) return a + initial_block*child_length;
            for (int block = initial_block+1; block<end_block; ++block) {
                a = my_positive_fwdsearch_nobackmax(-1, child_length - 1, d, level+1, rank*r_ + block, child_length, e);
                if (a != -1) return a + block*child_length;
            }
            a = my_positive_fwdsearch_nobackmax(-1, j%child_length, d, level+1, rank*r_ + end_block, child_length, e);
            if (a != -1) return a + end_block*child_length;
            return -1;
        }
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + 1 + offset < level_length && j + offset < level_length) {
            /*
            if (i + 1 == 0 && j + offset == level_length - 1 &&
                e + -1+(int)((*bt_prefix_max_excess_first_block_[level])[back_index]) < d) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                */
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                /*
                return -1;
            }
                 */
            a = my_positive_fwdsearch_nobackmax(i + offset, j + offset, d, level, first_block, level_length, e);
            if (a != -1) return a - offset;
            return -1;
        } else if (i + 1 + offset >= level_length && j + offset >= level_length) {
            /*
            if (i + 1 + offset == level_length && j == level_length - 1 &&
                e + -1+(int)((*bt_prefix_max_excess_second_block_[level])[back_index]) < d) {
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                e += 2*rank_ones - level_length;
                */
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }
                 */
                /*
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                 */
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                /*
                return -1;
            }
                 */
            a = my_positive_fwdsearch_nobackmax(offset + i - level_length, offset + j - level_length, d, level, first_block + 1, level_length,
                                      e);
            if (a != -1) return a + level_length - offset;
            return -1;
        } else {
            /*
            if (i + 1 == 0 && e + -1+(int)((*bt_prefix_max_excess_first_block_[level])[back_index]) < d) {
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e += 2*rank_ones - (level_length-offset);
                */
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[back_index];
                    } else {
                        e -= (*pair.second[level])[back_index];
                    }
                }
                 */
                /*
            } else {
                 */
                a = my_positive_fwdsearch_nobackmax(i + offset, level_length - 1, d, level, first_block, level_length, e);
                if (a != -1) return a - offset;
            /*}*/

            /*
            if (j == level_length - 1 && e + -1+(int)((*bt_prefix_max_excess_second_block_[level])[back_index]) < d) {
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                e += 2*rank_ones - level_length;
                */
                /*
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        e += (*pair.second[level])[level_index];
                    } else {
                        e -= (*pair.second[level])[level_index];
                    }
                }*/
                /*
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                e -= 2*rank_ones - (level_length-offset);
                 */
                /*
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        e -= (*pair.second[level])[back_index];
                    } else {
                        e += (*pair.second[level])[back_index];
                    }
                }
                 */
                /*
            } else {
                 */
                a = my_positive_fwdsearch_nobackmax(-1, offset + j - level_length, d, level, first_block + 1, level_length, e);
                if (a != -1) return a + level_length - offset;
            /*}*/
            return a;
        }
    }
}


int BPSPCBlockTree::positive_fwdsearch(int i, int d) {
    int e = 0;
    int a = positive_fwdsearch(((i+1)%first_level_length_) - 1,first_level_length_-1,d,0, (i+1)/first_level_length_, first_level_length_, e);
    if (a != -1) return a + ((i+1)/first_level_length_)*first_level_length_;
    int index = faster_next_block_positive_fwdsearch((i+1)/first_level_length_, d , e);
    if (index == -1) return input_.size();
    a = positive_fwdsearch(-1,first_level_length_-1,d,0, index, first_level_length_, e);
    if (a != -1) return a + index*first_level_length_;
    return input_.size();
}


int BPSPCBlockTree::my_positive_fwdsearch(int i, int d) {
    int e = 0;
    int a = my_positive_fwdsearch(((i+1)%first_level_length_) - 1,first_level_length_-1,d,0, (i+1)/first_level_length_, first_level_length_, e);
    if (a != -1) return a + ((i+1)/first_level_length_)*first_level_length_;
    int index = my_faster_next_block_positive_fwdsearch((i+1)/first_level_length_, d , e);
    if (index == -1) return input_.size();
    a = my_positive_fwdsearch(-1,first_level_length_-1,d,0, index, first_level_length_, e);
    if (a != -1) return a + index*first_level_length_;
    return input_.size();
}

int BPSPCBlockTree::my_positive_fwdsearch_nobackmax(int i, int d) {
    int e = 0;
    int a = my_positive_fwdsearch_nobackmax(((i+1)%first_level_length_) - 1,first_level_length_-1,d,0, (i+1)/first_level_length_, first_level_length_, e);
    if (a != -1) return a + ((i+1)/first_level_length_)*first_level_length_;
    int index = my_faster_next_block_positive_fwdsearch((i+1)/first_level_length_, d , e);
    if (index == -1) return input_.size();
    a = my_positive_fwdsearch_nobackmax(-1,first_level_length_-1,d,0, index, first_level_length_, e);
    if (a != -1) return a + index*first_level_length_;
    return input_.size();
}


int BPSPCBlockTree::next_block_positive_fwdsearch(int initial_block, int d, int& e) {
    for (int i = initial_block+1; i < number_of_blocks_first_level_; ++i) {
        if (e + -1+(int)((*(bt_prefix_max_excess_[0]))[i]) >= d) {
            return i;
        } else {
            int rank_ones = (i == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[i-1])));
            e += 2*rank_ones - first_level_length_ ;
            /*
            for (auto& pair: bt_ranks_) {
                if (pair.first == open_) {
                    e += (*pair.second[0])[i];
                } else {
                    e -= (*pair.second[0])[i];
                }
            }
             */
        }
    }
    return -1;
}

int BPSPCBlockTree::my_next_block_positive_fwdsearch(int initial_block, int d, int& e) {
    for (int i = initial_block+1; i < number_of_blocks_first_level_; ++i) {
        if (e + -1+(int)((*(bt_prefix_max_excess_[0]))[i]) >= d) {
            return i;
        } else {
            int rank_ones = ((int)((*bt_ranks_[open_][0])[i]));
            e += 2*rank_ones - first_level_length_ ;
            /*
            for (auto& pair: bt_ranks_) {
                if (pair.first == open_) {
                    e += (*pair.second[0])[i];
                } else {
                    e -= (*pair.second[0])[i];
                }
            }
             */
        }
    }
    return -1;
}

int BPSPCBlockTree::faster_next_block_positive_fwdsearch(int initial_block, int d, int& e) {
    int r_initial_block = 0;
    int index_last = (top_excess_->size()+n_pre_last_level_)-1;
    if (initial_block < n_last_level_) {
        r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
        index_last = r_*(index_last+1);
    }
    else
        r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;
    bool going_up = true;


    while (true) {
        if (going_up) {
            if (r_initial_block == 0) break;
            while (r_initial_block%r_ != 0) {
                ++r_initial_block;
                if (r_initial_block >= number_of_blocks_first_level_+n_internal_nodes_) break;
                if (r_initial_block >= n_internal_nodes_) {
                    if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                    else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;;
                    if (initial_block >= number_of_blocks_first_level_) break;
                    if (e + -1+(int)((*(bt_prefix_max_excess_[0]))[initial_block]) >= d) {
                        return initial_block;
                    } else {
                        int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                        e += 2*rank_ones - first_level_length_ ;
                        /*
                        for (auto& pair: bt_ranks_) {
                            if (pair.first == open_) {
                                e += (*pair.second[0])[initial_block];
                            } else {
                                e -= (*pair.second[0])[initial_block];
                            }
                        }
                         */
                    }
                }
                else {
                    if (e + -1+(int)((*(top_max_excess_))[r_initial_block]) >= d) {
                        going_up = false;
                        break;
                    } else {
                        e += decoded_excess((*top_excess_)[r_initial_block]);
                    }
                }
            }

            // LOOKUP
            if (going_up && r_initial_block != index_last && r_initial_block+1 < number_of_blocks_first_level_+n_internal_nodes_) {
                if (r_initial_block+1 >= n_internal_nodes_) {
                    if (r_initial_block+1-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block+1-n_internal_nodes_;
                    else initial_block = r_initial_block+1 - n_pre_last_level_-n_internal_nodes_;;
                    if (initial_block >= number_of_blocks_first_level_) break;
                    if (e + -1+(int)((*(bt_prefix_max_excess_[0]))[initial_block]) >= d) {
                        return initial_block;
                    }
                }
                else {
                    if (e + -1+(int)((*(top_max_excess_))[r_initial_block+1]) >= d) {
                        going_up = false;
                        ++r_initial_block;
                    }
                }
            }


            if (going_up) {
                r_initial_block = (r_initial_block-1)/r_;
                index_last = index_last/r_ - 1;
            }
        } else {
            if (r_initial_block >= n_internal_nodes_) {
                r_initial_block -= n_internal_nodes_;
                if (r_initial_block < n_pre_last_level_) return  n_last_level_ + r_initial_block;
                return  r_initial_block - n_pre_last_level_;
            } else {
                r_initial_block = r_initial_block*r_ +1;

                while (true) {
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;
                        if (initial_block >= number_of_blocks_first_level_) break;
                        if (e + -1+ (int) ((*(bt_prefix_max_excess_[0]))[initial_block]) >= d) {
                            return initial_block;
                        } else {
                            int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                            e += 2*rank_ones - first_level_length_ ;
                            /*
                            for (auto& pair: bt_ranks_) {
                                if (pair.first == open_) {
                                    e += (*pair.second[0])[initial_block];
                                } else {
                                    e -= (*pair.second[0])[initial_block];
                                }
                            }
                             */
                        }
                        ++r_initial_block;
                    } else {
                        if (e + -1+ (int) ((*(top_max_excess_))[r_initial_block]) >= d) {
                            break;
                        } else {
                            e += decoded_excess((*top_excess_)[r_initial_block]);
                        }
                        ++r_initial_block;
                    }
                }
            }
        }
    }
    return -1;

}


int BPSPCBlockTree::my_faster_next_block_positive_fwdsearch(int initial_block, int d, int& e) {
    int r_initial_block = 0;
    int index_last = (top_excess_->size()+n_pre_last_level_)-1;
    if (initial_block < n_last_level_) {
        r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
        index_last = r_*(index_last+1);
    }
    else
        r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;
    bool going_up = true;


    while (true) {
        if (going_up) {
            if (r_initial_block == 0) break;
            while (r_initial_block%r_ != 0) {
                ++r_initial_block;
                if (r_initial_block >= number_of_blocks_first_level_+n_internal_nodes_) break;
                if (r_initial_block >= n_internal_nodes_) {
                    if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                    else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;;
                    if (initial_block >= number_of_blocks_first_level_) break;
                    if (e + -1+(int)((*(bt_prefix_max_excess_[0]))[initial_block]) >= d) {
                        return initial_block;
                    } else {
                        int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                        e += 2*rank_ones - first_level_length_ ;
                        /*
                        for (auto& pair: bt_ranks_) {
                            if (pair.first == open_) {
                                e += (*pair.second[0])[initial_block];
                            } else {
                                e -= (*pair.second[0])[initial_block];
                            }
                        }
                         */
                    }
                }
                else {
                    if (e + -1+(int)((*(top_max_excess_))[r_initial_block]) >= d) {
                        going_up = false;
                        break;
                    } else {
                        e += decoded_excess((*top_excess_)[r_initial_block]);
                    }
                }
            }

            // LOOKUP
            if (going_up && r_initial_block != index_last && r_initial_block+1 < number_of_blocks_first_level_+n_internal_nodes_) {
                if (r_initial_block+1 >= n_internal_nodes_) {
                    if (r_initial_block+1-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block+1-n_internal_nodes_;
                    else initial_block = r_initial_block+1 - n_pre_last_level_-n_internal_nodes_;;
                    if (initial_block >= number_of_blocks_first_level_) break;
                    if (e + -1+(int)((*(bt_prefix_max_excess_[0]))[initial_block]) >= d) {
                        return initial_block;
                    }
                }
                else {
                    if (e + -1+(int)((*(top_max_excess_))[r_initial_block+1]) >= d) {
                        going_up = false;
                        ++r_initial_block;
                    }
                }
            }


            if (going_up) {
                r_initial_block = (r_initial_block-1)/r_;
                index_last = index_last/r_ - 1;
            }
        } else {
            if (r_initial_block >= n_internal_nodes_) {
                r_initial_block -= n_internal_nodes_;
                if (r_initial_block < n_pre_last_level_) return  n_last_level_ + r_initial_block;
                return  r_initial_block - n_pre_last_level_;
            } else {
                r_initial_block = r_initial_block*r_ +1;

                while (true) {
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;
                        if (initial_block >= number_of_blocks_first_level_) break;
                        if (e + -1+ (int) ((*(bt_prefix_max_excess_[0]))[initial_block]) >= d) {
                            return initial_block;
                        } else {
                            int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                            e += 2*rank_ones - first_level_length_ ;
                            /*
                            for (auto& pair: bt_ranks_) {
                                if (pair.first == open_) {
                                    e += (*pair.second[0])[initial_block];
                                } else {
                                    e -= (*pair.second[0])[initial_block];
                                }
                            }
                             */
                        }
                        ++r_initial_block;
                    } else {
                        if (e + -1+ (int) ((*(top_max_excess_))[r_initial_block]) >= d) {
                            break;
                        } else {
                            e += decoded_excess((*top_excess_)[r_initial_block]);
                        }
                        ++r_initial_block;
                    }
                }
            }
        }
    }
    return -1;

}

int BPSPCBlockTree::positive_bwdsearch(int i, int d) {
    int e = 0;
    int a = positive_bwdsearch(0,i%first_level_length_,d,0, i/first_level_length_, first_level_length_, e);
    if (a != input_.length()) return a + (i/first_level_length_)*first_level_length_;
    int index = faster_next_block_positive_bwdsearch(i/first_level_length_, d , e);
    if (index == -1) return -1;
    a = positive_bwdsearch(0,first_level_length_-1,d,0, index, first_level_length_, e);
    if (a != input_.length()) return a + index*first_level_length_;
    return -1;
}

int BPSPCBlockTree::my_positive_bwdsearch(int i, int d) {
    int e = 0;
    int a = my_positive_bwdsearch(0,i%first_level_length_,d,0, i/first_level_length_, first_level_length_, e);
    if (a != input_.length()) return a + (i/first_level_length_)*first_level_length_;
    int index = my_faster_next_block_positive_bwdsearch(i/first_level_length_, d , e);
    if (index == -1) return -1;
    a = my_positive_bwdsearch(0,first_level_length_-1,d,0, index, first_level_length_, e);
    if (a != input_.length()) return a + index*first_level_length_;
    return -1;
}


int BPSPCBlockTree::my_positive_bwdsearch_nobackmax(int i, int d) {
    int e = 0;
    int a = my_positive_bwdsearch_nobackmax(0,i%first_level_length_,d,0, i/first_level_length_, first_level_length_, e);
    if (a != input_.length()) return a + (i/first_level_length_)*first_level_length_;
    int index = my_faster_next_block_positive_bwdsearch(i/first_level_length_, d , e);
    if (index == -1) return -1;
    a = my_positive_bwdsearch_nobackmax(0,first_level_length_-1,d,0, index, first_level_length_, e);
    if (a != input_.length()) return a + index*first_level_length_;
    return -1;
}

int BPSPCBlockTree::positive_bwdsearch(int i, int j, int d, int level, int level_index, int level_length, int& e) {

    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) {
        int excess = 0;
        int rank_ones = 0;
        if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
        else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
        excess += 2*rank_ones - (j+1);
        /*
        int excess2 = 0;
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess2 += (*pair.second[level])[level_index];
            } else {
                excess2 -= (*pair.second[level])[level_index];
            }
        }
         */
        int max_prefix = -1+((*(bt_prefix_max_excess_[level]))[level_index]);
        int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
        if (e + reached > -d) {
            e += excess;
            return input_.length();
        }
    }

    int a = input_.length();
    if (level == number_of_levels_-1) { // Case LeafBlock
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = (level_length*level_index + j < leaf_string_size) ? j : leaf_string_size - 1 - level_length*level_index;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (k-i >= bottom_bit_size_-1) {
            while (k - i >= bottom_bit_size_-1) {
                int index = (chunk_info >> ((level_length * level_index + k-bottom_bit_size_+1) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;
                int max_excess = bottom_max_excess_2_[index];
                int excess = bottom_excess_2_[index];
                int reached = ((excess-max_excess) < excess) ? (excess-max_excess) : excess;
                if (e + reached <= -d) {
                    break;
                }
                e += excess;
                k -= bottom_bit_size_;
            }
        }
*/
        for (; k >= i; --k) {
            e += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == -d) return k-1;
            if ((level_length*level_index + k)%64 == 0) {
                chunk--;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        return input_.length();
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = j/child_length;
        int end_block = i/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_max_excess_[level+1]).size();
        if (rank*r_ + initial_block >= next_level_size) {
            initial_block = (next_level_size-1)%r_;
            j = (initial_block+1)*child_length-1;
        }
         */
        if (initial_block == end_block) {
            a = positive_bwdsearch(i%child_length, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a !=input_.length()) return a + initial_block*child_length;
            return input_.length();
        } else {
            a = positive_bwdsearch(0, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != input_.length()) return a + initial_block*child_length;
            for (int block = initial_block-1; block>end_block; --block) {
                a = positive_bwdsearch(0, child_length - 1, d, level+1, rank*r_ + block, child_length, e);
                if (a != input_.length()) return a + block*child_length;
            }
            a = positive_bwdsearch(i%child_length, child_length-1, d, level+1, rank*r_ + end_block, child_length, e);
            if (a != input_.length()) return a + end_block*child_length;
            return input_.length();
        }
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int first_block_excess = 0;
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                 if (first_block_excess != first_block_excess2)
                     std::cout << std::endl;
                     */
                int max_prefix_first_block = -1+((*(bt_prefix_max_excess_first_block_[level]))[back_index]);
                int reached = ((first_block_excess-max_prefix_first_block) < first_block_excess) ? (first_block_excess-max_prefix_first_block) : first_block_excess;
                if (e + reached > -d) {
                    e += first_block_excess;
                    return input_.length();
                }

            }
            a = positive_bwdsearch(i + offset, j + offset, d, level, first_block, level_length, e);
            if (a != input_.length()) return a - offset;
            return input_.length();
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = 0;
                if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                if (second_block_excess != second_block_excess2)
                    std::cout << std::endl;
                    */
                int max_prefix_second_block = -1+((*(bt_prefix_max_excess_second_block_[level]))[back_index]);
                int reached = ((second_block_excess-max_prefix_second_block) < second_block_excess) ? (second_block_excess-max_prefix_second_block) : second_block_excess;
                if (e + reached > -d) {
                    e += second_block_excess;
                    return input_.length();
                }
            }
            a = positive_bwdsearch(offset + i - level_length, offset + j - level_length, d, level, first_block + 1, level_length,
                          e);
            if (a != input_.length()) return a + level_length - offset;
            return input_.length();
        } else {
            if (j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = 0;
                if (level == 0) rank_ones = (level_index == 0) ? (*(bt_first_level_cumulated_ranks_[open_]))[level_index] : (((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index]) - ((int)(*(bt_first_level_cumulated_ranks_[open_]))[level_index-1]));
                else rank_ones = (level_index % r_ == 0) ? (*(bt_cumulated_ranks_[open_][level-1]))[level_index] : (((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index]) - ((int)(*(bt_cumulated_ranks_[open_][level-1]))[level_index-1]));
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                 if (second_block_excess != second_block_excess2)
                     std::cout << std::endl;
                     */
                int max_prefix_second_block = -1+((*(bt_prefix_max_excess_second_block_[level]))[back_index]);
                int reached = ((second_block_excess-max_prefix_second_block) < second_block_excess) ? (second_block_excess-max_prefix_second_block) : second_block_excess;
                if (e + reached > -d) {
                    e += second_block_excess;
                } else {
                    a = positive_bwdsearch(0, offset + j - level_length, d, level, first_block + 1, level_length,
                                  e);
                    if (a != input_.length()) return a + level_length - offset;
                }
            } else {
                a = positive_bwdsearch(0, offset + j - level_length, d, level, first_block + 1, level_length,
                              e);
                if (a != input_.length()) return a + level_length - offset;
            }


            if (i == 0) {
                int first_block_excess = 0;
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                if (first_block_excess != first_block_excess2)
                    std::cout << std::endl;
                */
                int max_prefix_first_block = -1+((*(bt_prefix_max_excess_first_block_[level]))[back_index]);
                int reached = ((first_block_excess-max_prefix_first_block) < first_block_excess) ? (first_block_excess-max_prefix_first_block) : first_block_excess;
                if (e + reached > -d) {
                    e += first_block_excess;
                } else {
                    a = positive_bwdsearch(i + offset, level_length-1, d, level, first_block, level_length, e);
                    if (a != input_.length()) return a - offset;
                }

            } else {
                a = positive_bwdsearch(i + offset, level_length - 1, d, level, first_block, level_length, e);
                if (a != input_.length()) return a - offset;
            }
            return a;
        }
    }
}


int BPSPCBlockTree::my_positive_bwdsearch(int i, int j, int d, int level, int level_index, int level_length, int& e) {

    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) {
        int excess = 0;
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        excess += 2*rank_ones - (j+1);
        /*
        int excess2 = 0;
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess2 += (*pair.second[level])[level_index];
            } else {
                excess2 -= (*pair.second[level])[level_index];
            }
        }
         */
        int max_prefix = -1+((*(bt_prefix_max_excess_[level]))[level_index]);
        int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
        if (e + reached > -d) {
            e += excess;
            return input_.length();
        }
    }

    int a = input_.length();
    if (level == number_of_levels_-1) { // Case LeafBlock
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = (level_length*level_index + j < leaf_string_size) ? j : leaf_string_size - 1 - level_length*level_index;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (k-i >= bottom_bit_size_-1) {
            while (k - i >= bottom_bit_size_-1) {
                int index = (chunk_info >> ((level_length * level_index + k-bottom_bit_size_+1) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;
                int max_excess = bottom_max_excess_2_[index];
                int excess = bottom_excess_2_[index];
                int reached = ((excess-max_excess) < excess) ? (excess-max_excess) : excess;
                if (e + reached <= -d) {
                    break;
                }
                e += excess;
                k -= bottom_bit_size_;
            }
        }
*/
        for (; k >= i; --k) {
            e += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == -d) return k-1;
            if ((level_length*level_index + k)%64 == 0) {
                chunk--;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        return input_.length();
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = j/child_length;
        int end_block = i/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_max_excess_[level+1]).size();
        if (rank*r_ + initial_block >= next_level_size) {
            initial_block = (next_level_size-1)%r_;
            j = (initial_block+1)*child_length-1;
        }
         */
        if (initial_block == end_block) {
            a = my_positive_bwdsearch(i%child_length, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a !=input_.length()) return a + initial_block*child_length;
            return input_.length();
        } else {
            a = my_positive_bwdsearch(0, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != input_.length()) return a + initial_block*child_length;
            for (int block = initial_block-1; block>end_block; --block) {
                a = my_positive_bwdsearch(0, child_length - 1, d, level+1, rank*r_ + block, child_length, e);
                if (a != input_.length()) return a + block*child_length;
            }
            a = my_positive_bwdsearch(i%child_length, child_length-1, d, level+1, rank*r_ + end_block, child_length, e);
            if (a != input_.length()) return a + end_block*child_length;
            return input_.length();
        }
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int first_block_excess = 0;
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                 if (first_block_excess != first_block_excess2)
                     std::cout << std::endl;
                     */
                int max_prefix_first_block = -1+((*(bt_prefix_max_excess_first_block_[level]))[back_index]);
                int reached = ((first_block_excess-max_prefix_first_block) < first_block_excess) ? (first_block_excess-max_prefix_first_block) : first_block_excess;
                if (e + reached > -d) {
                    e += first_block_excess;
                    return input_.length();
                }

            }
            a = my_positive_bwdsearch(i + offset, j + offset, d, level, first_block, level_length, e);
            if (a != input_.length()) return a - offset;
            return input_.length();
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                if (second_block_excess != second_block_excess2)
                    std::cout << std::endl;
                    */
                int max_prefix_second_block = -1+((*(bt_prefix_max_excess_second_block_[level]))[back_index]);
                int reached = ((second_block_excess-max_prefix_second_block) < second_block_excess) ? (second_block_excess-max_prefix_second_block) : second_block_excess;
                if (e + reached > -d) {
                    e += second_block_excess;
                    return input_.length();
                }
            }
            a = my_positive_bwdsearch(offset + i - level_length, offset + j - level_length, d, level, first_block + 1, level_length,
                                   e);
            if (a != input_.length()) return a + level_length - offset;
            return input_.length();
        } else {
            if (j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                 if (second_block_excess != second_block_excess2)
                     std::cout << std::endl;
                     */
                int max_prefix_second_block = -1+((*(bt_prefix_max_excess_second_block_[level]))[back_index]);
                int reached = ((second_block_excess-max_prefix_second_block) < second_block_excess) ? (second_block_excess-max_prefix_second_block) : second_block_excess;
                if (e + reached > -d) {
                    e += second_block_excess;
                } else {
                    a = my_positive_bwdsearch(0, offset + j - level_length, d, level, first_block + 1, level_length,
                                           e);
                    if (a != input_.length()) return a + level_length - offset;
                }
            } else {
                a = my_positive_bwdsearch(0, offset + j - level_length, d, level, first_block + 1, level_length,
                                       e);
                if (a != input_.length()) return a + level_length - offset;
            }


            if (i == 0) {
                int first_block_excess = 0;
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                if (first_block_excess != first_block_excess2)
                    std::cout << std::endl;
                */
                int max_prefix_first_block = -1+((*(bt_prefix_max_excess_first_block_[level]))[back_index]);
                int reached = ((first_block_excess-max_prefix_first_block) < first_block_excess) ? (first_block_excess-max_prefix_first_block) : first_block_excess;
                if (e + reached > -d) {
                    e += first_block_excess;
                } else {
                    a = my_positive_bwdsearch(i + offset, level_length-1, d, level, first_block, level_length, e);
                    if (a != input_.length()) return a - offset;
                }

            } else {
                a = my_positive_bwdsearch(i + offset, level_length - 1, d, level, first_block, level_length, e);
                if (a != input_.length()) return a - offset;
            }
            return a;
        }
    }
}

int BPSPCBlockTree::my_positive_bwdsearch_nobackmax(int i, int j, int d, int level, int level_index, int level_length, int& e) {

    if (i == 0 && (j == level_length-1 || (j == ((input_.size()-1) % level_length) && level_index == ((*bt_prefix_min_excess_[level]).size()-1)))) {
        int excess = 0;
        int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
        excess += 2*rank_ones - (j+1);
        /*
        int excess2 = 0;
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess2 += (*pair.second[level])[level_index];
            } else {
                excess2 -= (*pair.second[level])[level_index];
            }
        }
         */
        int max_prefix = -1+((*(bt_prefix_max_excess_[level]))[level_index]);
        int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
        if (e + reached > -d) {
            e += excess;
            return input_.length();
        }
    }

    int a = input_.length();
    if (level == number_of_levels_-1) { // Case LeafBlock
        auto& leaf_string = *leaf_string_;
        int leaf_string_size = leaf_string.size();
        int k = (level_length*level_index + j < leaf_string_size) ? j : leaf_string_size - 1 - level_length*level_index;
        int chunk = (level_length*level_index + k)/64;
        uint64_t chunk_info = *(leaf_string.m_data+chunk);

/*
        if (k-i >= bottom_bit_size_-1) {
            while (k - i >= bottom_bit_size_-1) {
                int index = (chunk_info >> ((level_length * level_index + k-bottom_bit_size_+1) % 64)) % (1 << bottom_bit_size_);
                if (index < 0) index += 1<<bottom_bit_size_;
                int max_excess = bottom_max_excess_2_[index];
                int excess = bottom_excess_2_[index];
                int reached = ((excess-max_excess) < excess) ? (excess-max_excess) : excess;
                if (e + reached <= -d) {
                    break;
                }
                e += excess;
                k -= bottom_bit_size_;
            }
        }
*/
        for (; k >= i; --k) {
            e += ((chunk_info >> ((level_length*level_index + k)%64))%2)?1:-1;
            //(leaf_string[level_length*level_index + k] == new_alphabet_open_) ? 1 : -1;
            if (e == -d) return k-1;
            if ((level_length*level_index + k)%64 == 0) {
                chunk--;
                chunk_info = *(leaf_string.m_data+chunk);
            }
        }
        return input_.length();
    }
    if ((*bt_bv_[level])[level_index]) { // Case InternalBlock
        int child_length = level_length/r_;
        int initial_block = j/child_length;
        int end_block = i/child_length;
        int rank = (*bt_bv_rank_[level])(level_index);
        /*
        int next_level_size = (*bt_prefix_max_excess_[level+1]).size();
        if (rank*r_ + initial_block >= next_level_size) {
            initial_block = (next_level_size-1)%r_;
            j = (initial_block+1)*child_length-1;
        }
         */
        if (initial_block == end_block) {
            a = my_positive_bwdsearch_nobackmax(i%child_length, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a !=input_.length()) return a + initial_block*child_length;
            return input_.length();
        } else {
            a = my_positive_bwdsearch_nobackmax(0, j%child_length, d, level+1, rank*r_ + initial_block,child_length, e);
            if (a != input_.length()) return a + initial_block*child_length;
            for (int block = initial_block-1; block>end_block; --block) {
                a = my_positive_bwdsearch_nobackmax(0, child_length - 1, d, level+1, rank*r_ + block, child_length, e);
                if (a != input_.length()) return a + block*child_length;
            }
            a = my_positive_bwdsearch_nobackmax(i%child_length, child_length-1, d, level+1, rank*r_ + end_block, child_length, e);
            if (a != input_.length()) return a + end_block*child_length;
            return input_.length();
        }
    } else { // Case BackBlock
        int back_index = level_index - (*bt_bv_rank_[level])(level_index + 1);
        int encoded_offset = (*bt_offsets_[level])[back_index];

        int first_block = encoded_offset / level_length;
        int offset = encoded_offset % level_length;

        if (i + offset < level_length && j + offset < level_length) {
            if (i == 0 && j + offset == level_length - 1) {
                int first_block_excess = 0;
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                 if (first_block_excess != first_block_excess2)
                     std::cout << std::endl;
                     */
                int max_prefix_first_block = -1+((*(bt_prefix_max_excess_first_block_[level]))[back_index]);
                int reached = ((first_block_excess-max_prefix_first_block) < first_block_excess) ? (first_block_excess-max_prefix_first_block) : first_block_excess;
                if (e + reached > -d) {
                    e += first_block_excess;
                    return input_.length();
                }

            }
            a = my_positive_bwdsearch_nobackmax(i + offset, j + offset, d, level, first_block, level_length, e);
            if (a != input_.length()) return a - offset;
            return input_.length();
        } else if (i + offset >= level_length && j + offset >= level_length) {
            if (i + offset == level_length && j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                if (second_block_excess != second_block_excess2)
                    std::cout << std::endl;
                    */
                int max_prefix_second_block = -1+((*(bt_prefix_max_excess_second_block_[level]))[back_index]);
                int reached = ((second_block_excess-max_prefix_second_block) < second_block_excess) ? (second_block_excess-max_prefix_second_block) : second_block_excess;
                if (e + reached > -d) {
                    e += second_block_excess;
                    return input_.length();
                }
            }
            a = my_positive_bwdsearch_nobackmax(offset + i - level_length, offset + j - level_length, d, level, first_block + 1, level_length,
                                      e);
            if (a != input_.length()) return a + level_length - offset;
            return input_.length();
        } else {
            /*
            if (j == level_length - 1) {
                //&&
                //                e + (int)((*bt_prefix_min_excess_second_block_[level])[back_index]) > d
                int second_block_excess = 0;
                int rank_ones = (*(bt_ranks_[open_][level]))[level_index];
                second_block_excess += 2*rank_ones - level_length;
                rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                second_block_excess -= 2*rank_ones - (level_length-offset);
                */
                /*
                int second_block_excess2 = 0;
                for (auto& pair: bt_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 += (*pair.second[level])[level_index];
                    } else {
                        second_block_excess2 -= (*pair.second[level])[level_index];
                    }
                }
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        second_block_excess2 -= (*pair.second[level])[back_index];
                    } else {
                        second_block_excess2 += (*pair.second[level])[back_index];
                    }
                }
                 if (second_block_excess != second_block_excess2)
                     std::cout << std::endl;
                     */
                /*
                int max_prefix_second_block = -1+((*(bt_prefix_max_excess_second_block_[level]))[back_index]);
                int reached = ((second_block_excess-max_prefix_second_block) < second_block_excess) ? (second_block_excess-max_prefix_second_block) : second_block_excess;
                if (e + reached > -d) {
                    e += second_block_excess;
                } else {
                    a = my_positive_bwdsearch_nobackmax(0, offset + j - level_length, d, level, first_block + 1, level_length,
                                              e);
                    if (a != input_.length()) return a + level_length - offset;
                }
            } else {*/
                a = my_positive_bwdsearch_nobackmax(0, offset + j - level_length, d, level, first_block + 1, level_length,
                                          e);
                if (a != input_.length()) return a + level_length - offset;
            /*}*/

            /*
            if (i == 0) {
                int first_block_excess = 0;
                int rank_ones = (*bt_second_ranks_[open_][level])[back_index];
                first_block_excess += 2*rank_ones - (level_length-offset);
                */
                /*
                int first_block_excess2 = 0;
                for (auto& pair: bt_second_ranks_) {
                    if (pair.first == open_) {
                        first_block_excess2 += (*pair.second[level])[back_index];
                    } else {
                        first_block_excess2 -= (*pair.second[level])[back_index];
                    }
                }
                if (first_block_excess != first_block_excess2)
                    std::cout << std::endl;
                */
                /*
                int max_prefix_first_block = -1+((*(bt_prefix_max_excess_first_block_[level]))[back_index]);
                int reached = ((first_block_excess-max_prefix_first_block) < first_block_excess) ? (first_block_excess-max_prefix_first_block) : first_block_excess;
                if (e + reached > -d) {
                    e += first_block_excess;
                } else {
                    a = my_positive_bwdsearch_nobackmax(i + offset, level_length-1, d, level, first_block, level_length, e);
                    if (a != input_.length()) return a - offset;
                }

            } else {*/
                a = my_positive_bwdsearch_nobackmax(i + offset, level_length - 1, d, level, first_block, level_length, e);
                if (a != input_.length()) return a - offset;
            /*}*/
            return a;
        }
    }
}


int BPSPCBlockTree::next_block_positive_bwdsearch(int initial_block, int d, int& e) {
    for (int i = initial_block-1; i >= 0; --i) {
        int excess = 0;
        int rank_ones = (i == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[i])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[i-1])));
        excess += 2*rank_ones - first_level_length_ ;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess += (*pair.second[0])[i];
            } else {
                excess -= (*pair.second[0])[i];
            }
        }
         */
        int max_prefix = -1+((*(bt_prefix_max_excess_[0]))[i]);
        int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
        if (e + reached <= -d) {
            return i;
        } else {
            e+=excess;
        }
    }
    return -1;
}


int BPSPCBlockTree::my_next_block_positive_bwdsearch(int initial_block, int d, int& e) {
    for (int i = initial_block-1; i >= 0; --i) {
        int excess = 0;
        int rank_ones = ((int)((*bt_ranks_[open_][0])[i]));
        excess += 2*rank_ones - first_level_length_ ;
        /*
        for (auto& pair: bt_ranks_) {
            if (pair.first == open_) {
                excess += (*pair.second[0])[i];
            } else {
                excess -= (*pair.second[0])[i];
            }
        }
         */
        int max_prefix = -1+((*(bt_prefix_max_excess_[0]))[i]);
        int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
        if (e + reached <= -d) {
            return i;
        } else {
            e+=excess;
        }
    }
    return -1;
}

int BPSPCBlockTree::faster_next_block_positive_bwdsearch(int initial_block, int d, int& e) {
    int r_initial_block = 0;
    int index_first = (top_excess_->size()+n_pre_last_level_);
    if (initial_block < n_last_level_)
        r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
    else {
        r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;
        index_first = (index_first-1)/r_;
    }


    bool going_up = true;
    while (true) {
        if (going_up) {
            if (r_initial_block == 0) break;
            while ((r_initial_block-1)%r_ != 0) {
                --r_initial_block;
                //if (r_initial_block >= number_of_blocks_first_level_+n_internal_nodes_) break;
                if (r_initial_block >= n_internal_nodes_) {
                    if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                    else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;
                    //if (initial_block >= number_of_blocks_first_level_) break;

                    int excess = 0;
                    int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                    excess += 2*rank_ones - first_level_length_;
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[0])[initial_block];
                        } else {
                            excess -= (*pair.second[0])[initial_block];
                        }
                    }
                     */
                    int max_prefix = -1+((*(bt_prefix_max_excess_[0]))[initial_block]);
                    int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
                    if (e + reached <= -d) {
                        return initial_block;
                    } else {
                        e += excess;
                    }
                }
                else {
                    int excess = decoded_excess((*top_excess_)[r_initial_block]);
                    int max_prefix = -1+((*(top_max_excess_))[r_initial_block]);
                    int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
                    if (e + reached <= -d) {
                        going_up = false;
                        break;
                    } else {
                        e += excess;
                    }
                }
            }

            // LOOKUP
            if (going_up && r_initial_block != index_first) {
                if (r_initial_block-1 >= n_internal_nodes_) {
                    if (r_initial_block-1-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-1-n_internal_nodes_;
                    else initial_block = r_initial_block-1 - n_pre_last_level_-n_internal_nodes_;
                    int excess = 0;
                    int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                    excess += 2*rank_ones - first_level_length_;
                    int max_prefix = -1+((*(bt_prefix_max_excess_[0]))[initial_block]);
                    int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
                    if (e + reached <= -d) {
                        return initial_block;
                    }
                }
                else {
                    int excess = decoded_excess((*top_excess_)[r_initial_block-1]);
                    int max_prefix = -1+((*(top_max_excess_))[r_initial_block-1]);
                    int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
                    if (e + reached <= -d) {
                        going_up = false;
                        --r_initial_block;
                    }
                }
            }

            if (going_up) {
                r_initial_block = (r_initial_block-1)/r_;
                index_first = (index_first-1)/r_;
            }
        } else {
            if (r_initial_block >= n_internal_nodes_) {
                r_initial_block -= n_internal_nodes_;
                if (r_initial_block < n_pre_last_level_) return  n_last_level_ + r_initial_block;
                return  r_initial_block - n_pre_last_level_;
            } else {
                r_initial_block = r_initial_block*r_ +r_;
                if (r_initial_block >= n_internal_nodes_ + n_last_level_ + n_pre_last_level_)
                    r_initial_block = n_internal_nodes_ + n_last_level_ + n_pre_last_level_-1;
                while (true) {
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;
                        //if (initial_block >= number_of_blocks_first_level_) break;

                        int excess = 0;
                        int rank_ones = (initial_block == 0) ? ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) : ( ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block])) - ((int)((*bt_first_level_cumulated_ranks_[open_])[initial_block-1])));
                        excess += 2*rank_ones - first_level_length_;
                        /*
                        for (auto& pair: bt_ranks_) {
                            if (pair.first == open_) {
                                excess += (*pair.second[0])[initial_block];
                            } else {
                                excess -= (*pair.second[0])[initial_block];
                            }
                        }
                         */
                        int max_prefix = -1+((*(bt_prefix_max_excess_[0]))[initial_block]);
                        int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
                        if (e + reached <= -d) {
                            return initial_block;
                        } else {
                            e += excess;
                        }
                        --r_initial_block;
                    } else {
                        int excess = decoded_excess((*top_excess_)[r_initial_block]);
                        int max_prefix = -1+((*(top_max_excess_))[r_initial_block]);
                        int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
                        if (e + reached <= -d) {
                            break;
                        } else {
                            e += excess;
                        }
                        --r_initial_block;
                    }
                }
            }
        }
    }
    return -1;

}


int BPSPCBlockTree::my_faster_next_block_positive_bwdsearch(int initial_block, int d, int& e) {
    int r_initial_block = 0;
    int index_first = (top_excess_->size()+n_pre_last_level_);
    if (initial_block < n_last_level_)
        r_initial_block = initial_block + n_pre_last_level_ + n_internal_nodes_;
    else {
        r_initial_block = initial_block - n_last_level_ + n_internal_nodes_;
        index_first = (index_first-1)/r_;
    }


    bool going_up = true;
    while (true) {
        if (going_up) {
            if (r_initial_block == 0) break;
            while ((r_initial_block-1)%r_ != 0) {
                --r_initial_block;
                //if (r_initial_block >= number_of_blocks_first_level_+n_internal_nodes_) break;
                if (r_initial_block >= n_internal_nodes_) {
                    if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                    else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;
                    //if (initial_block >= number_of_blocks_first_level_) break;

                    int excess = 0;
                    int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                    excess += 2*rank_ones - first_level_length_;
                    /*
                    for (auto& pair: bt_ranks_) {
                        if (pair.first == open_) {
                            excess += (*pair.second[0])[initial_block];
                        } else {
                            excess -= (*pair.second[0])[initial_block];
                        }
                    }
                     */
                    int max_prefix = -1+((*(bt_prefix_max_excess_[0]))[initial_block]);
                    int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
                    if (e + reached <= -d) {
                        return initial_block;
                    } else {
                        e += excess;
                    }
                }
                else {
                    int excess = decoded_excess((*top_excess_)[r_initial_block]);
                    int max_prefix = -1+((*(top_max_excess_))[r_initial_block]);
                    int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
                    if (e + reached <= -d) {
                        going_up = false;
                        break;
                    } else {
                        e += excess;
                    }
                }
            }

            // LOOKUP
            if (going_up && r_initial_block != index_first) {
                if (r_initial_block-1 >= n_internal_nodes_) {
                    if (r_initial_block-1-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-1-n_internal_nodes_;
                    else initial_block = r_initial_block-1 - n_pre_last_level_-n_internal_nodes_;
                    int excess = 0;
                    int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                    excess += 2*rank_ones - first_level_length_;
                    int max_prefix = -1+((*(bt_prefix_max_excess_[0]))[initial_block]);
                    int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
                    if (e + reached <= -d) {
                        return initial_block;
                    }
                }
                else {
                    int excess = decoded_excess((*top_excess_)[r_initial_block-1]);
                    int max_prefix = -1+((*(top_max_excess_))[r_initial_block-1]);
                    int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
                    if (e + reached <= -d) {
                        going_up = false;
                        --r_initial_block;
                    }
                }
            }

            if (going_up) {
                r_initial_block = (r_initial_block-1)/r_;
                index_first = (index_first-1)/r_;
            }
        } else {
            if (r_initial_block >= n_internal_nodes_) {
                r_initial_block -= n_internal_nodes_;
                if (r_initial_block < n_pre_last_level_) return  n_last_level_ + r_initial_block;
                return  r_initial_block - n_pre_last_level_;
            } else {
                r_initial_block = r_initial_block*r_ +r_;
                if (r_initial_block >= n_internal_nodes_ + n_last_level_ + n_pre_last_level_)
                    r_initial_block = n_internal_nodes_ + n_last_level_ + n_pre_last_level_-1;
                while (true) {
                    if (r_initial_block >= n_internal_nodes_) {
                        if (r_initial_block-n_internal_nodes_ < n_pre_last_level_) initial_block = n_last_level_ + r_initial_block-n_internal_nodes_;
                        else initial_block = r_initial_block - n_pre_last_level_-n_internal_nodes_;
                        //if (initial_block >= number_of_blocks_first_level_) break;

                        int excess = 0;
                        int rank_ones = ((int) ((*bt_ranks_[open_][0])[initial_block]));
                        excess += 2*rank_ones - first_level_length_;
                        /*
                        for (auto& pair: bt_ranks_) {
                            if (pair.first == open_) {
                                excess += (*pair.second[0])[initial_block];
                            } else {
                                excess -= (*pair.second[0])[initial_block];
                            }
                        }
                         */
                        int max_prefix = -1+((*(bt_prefix_max_excess_[0]))[initial_block]);
                        int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
                        if (e + reached <= -d) {
                            return initial_block;
                        } else {
                            e += excess;
                        }
                        --r_initial_block;
                    } else {
                        int excess = decoded_excess((*top_excess_)[r_initial_block]);
                        int max_prefix = -1+((*(top_max_excess_))[r_initial_block]);
                        int reached = ((excess-max_prefix) < excess) ? (excess-max_prefix) : excess;
                        if (e + reached <= -d) {
                            break;
                        } else {
                            e += excess;
                        }
                        --r_initial_block;
                    }
                }
            }
        }
    }
    return -1;

}


void BPSPCBlockTree::print_statistics() {
    PCBlockTree::print_statistics();

    int bottom_excess_size = sdsl::size_in_bytes(*bottom_excess_);
    int bottom_min_excess_size = sdsl::size_in_bytes(*bottom_min_excess_);
    int bottom_max_excess_size = sdsl::size_in_bytes(*bottom_max_excess_);
    int bottom_min_count_size = sdsl::size_in_bytes(*bottom_min_count_);

    int c_bottom_excess_size = sdsl::size_in_bytes(*c_bottom_excess_);
    int c_bottom_min_excess_size = sdsl::size_in_bytes(*c_bottom_min_excess_);
    int c_bottom_max_excess_size = sdsl::size_in_bytes(*c_bottom_max_excess_);
    int c_bottom_min_count_size = sdsl::size_in_bytes(*c_bottom_min_count_);

    int bottom_excess_2_size = sizeof(int)*bottom_excess_2_.size();
    int bottom_min_excess_2_size = sizeof(int)*bottom_min_excess_2_.size();
    int bottom_max_excess_2_size = sizeof(int)*bottom_max_excess_2_.size();
    int bottom_min_count_2_size = sizeof(int)*bottom_min_count_2_.size();

    int bt_first_level_prefix_leaf_ranks_size = sdsl::size_in_bytes(*bt_first_level_prefix_leaf_ranks_);
    int bt_first_level_cumulated_leaf_ranks_size = sdsl::size_in_bytes(*bt_first_level_cumulated_leaf_ranks_);
    int c_bt_first_level_prefix_leaf_ranks_size = sdsl::size_in_bytes(*c_bt_first_level_prefix_leaf_ranks_);
    int c_bt_first_level_cumulated_leaf_ranks_size = sdsl::size_in_bytes(*c_bt_first_level_cumulated_leaf_ranks_);

    int bt_first_level_prefix_zeros_ranks_size = sdsl::size_in_bytes(*bt_first_level_prefix_zeros_ranks_);
    int bt_first_level_cumulated_zeros_ranks_size = sdsl::size_in_bytes(*bt_first_level_cumulated_zeros_ranks_);
    int c_bt_first_level_prefix_zeros_ranks_size = sdsl::size_in_bytes(*c_bt_first_level_prefix_zeros_ranks_);
    int c_bt_first_level_cumulated_zeros_ranks_size = sdsl::size_in_bytes(*c_bt_first_level_cumulated_zeros_ranks_);

    //int bt_jea15_first_level_prefix_leaf_ranks_size = sdsl::size_in_bytes(*bt_jea15_first_level_prefix_leaf_ranks_); // Jea15 encoding
    //int bt_jea15_first_level_cumulated_leaf_ranks_size = sdsl::size_in_bytes(*bt_jea15_first_level_cumulated_leaf_ranks_); // Jea15 encoding
    //int c_bt_jea15_first_level_prefix_leaf_ranks_size = sdsl::size_in_bytes(*c_bt_jea15_first_level_prefix_leaf_ranks_); // Jea15 encoding
    //int c_bt_jea15_first_level_cumulated_leaf_ranks_size = sdsl::size_in_bytes(*c_bt_jea15_first_level_cumulated_leaf_ranks_); // Jea15 encoding


    int bt_min_in_first_block_size = sizeof(void *);
    for (sdsl::bit_vector* bv : bt_min_in_first_block_) {
        bt_min_in_first_block_size += sdsl::size_in_bytes(*bv);
    }
    int c_bt_min_in_first_block_size = sizeof(void *);
    for (sdsl::rrr_vector<63>* bv : c_bt_min_in_first_block_) {
        c_bt_min_in_first_block_size += sdsl::size_in_bytes(*bv);
    }

    int bt_min_in_both_blocks_size = sizeof(void *);
    for (sdsl::bit_vector* bv : bt_min_in_both_blocks_) {
        bt_min_in_both_blocks_size += sdsl::size_in_bytes(*bv);
    }
    int c_bt_min_in_both_blocks_size = sizeof(void *);
    for (sdsl::rrr_vector<63>* bv : c_bt_min_in_both_blocks_) {
        c_bt_min_in_both_blocks_size += sdsl::size_in_bytes(*bv);
    }


    int bt_max_in_first_block_size = sizeof(void *);
    for (sdsl::bit_vector* bv : bt_max_in_first_block_) {
        bt_max_in_first_block_size += sdsl::size_in_bytes(*bv);
    }
    int c_bt_max_in_first_block_size = sizeof(void *);
    for (sdsl::rrr_vector<63>* bv : c_bt_max_in_first_block_) {
        c_bt_max_in_first_block_size += sdsl::size_in_bytes(*bv);
    }

    int bt_max_in_both_blocks_size = sizeof(void *);
    for (sdsl::bit_vector* bv : bt_max_in_both_blocks_) {
        bt_max_in_both_blocks_size += sdsl::size_in_bytes(*bv);
    }
    int c_bt_max_in_both_blocks_size = sizeof(void *);
    for (sdsl::rrr_vector<63>* bv : c_bt_max_in_both_blocks_) {
        c_bt_max_in_both_blocks_size += sdsl::size_in_bytes(*bv);
    }



    int bt_min_in_both_blocks_rank_size = sizeof(void *);
    for (sdsl::rank_support_v<1>* r : bt_min_in_both_blocks_rank_) {
        bt_min_in_both_blocks_rank_size += sdsl::size_in_bytes(*r);
    }
    int c_bt_min_in_both_blocks_rank_size = sizeof(void *);
    for (sdsl::rrr_vector<63>::rank_1_type* r : c_bt_min_in_both_blocks_rank_) {
        c_bt_min_in_both_blocks_rank_size += sdsl::size_in_bytes(*r);
    }


    int bt_max_in_both_blocks_rank_size = sizeof(void *);
    for (sdsl::rank_support_v<1>* r : bt_max_in_both_blocks_rank_) {
        bt_max_in_both_blocks_rank_size += sdsl::size_in_bytes(*r);
    }
    int c_bt_max_in_both_blocks_rank_size = sizeof(void *);
    for (sdsl::rrr_vector<63>::rank_1_type* r : c_bt_max_in_both_blocks_rank_) {
        c_bt_max_in_both_blocks_rank_size += sdsl::size_in_bytes(*r);
    }



    int bt_starts_with_end_leaf_size = sizeof(void *);
    for (sdsl::bit_vector *bv : bt_starts_with_end_leaf_) {
        bt_starts_with_end_leaf_size += sdsl::size_in_bytes(*bv);
    }
    int c_bt_starts_with_end_leaf_size = sizeof(void *);
    for (sdsl::rrr_vector<63> *bv : c_bt_starts_with_end_leaf_) {
        c_bt_starts_with_end_leaf_size += sdsl::size_in_bytes(*bv);
    }

    int bt_suffix_start_with_end_leaf_size = sizeof(void *);
    for (sdsl::bit_vector *bv : bt_suffix_start_with_end_leaf_) {
        bt_suffix_start_with_end_leaf_size += sdsl::size_in_bytes(*bv);
    }
    int c_bt_suffix_start_with_end_leaf_size = sizeof(void *);
    for (sdsl::rrr_vector<63> *bv : c_bt_suffix_start_with_end_leaf_) {
        c_bt_suffix_start_with_end_leaf_size += sdsl::size_in_bytes(*bv);
    }

    int bt_prefix_start_with_end_leaf_size = sizeof(void *);
    for (sdsl::bit_vector *bv : bt_prefix_start_with_end_leaf_) {
        bt_prefix_start_with_end_leaf_size += sdsl::size_in_bytes(*bv);
    }
    int c_bt_prefix_start_with_end_leaf_size = sizeof(void *);
    for (sdsl::rrr_vector<63> *bv : c_bt_prefix_start_with_end_leaf_) {
        c_bt_prefix_start_with_end_leaf_size += sdsl::size_in_bytes(*bv);
    }


    int bt_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::int_vector<> *vector : bt_leaf_ranks_) {
        bt_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>> *vector : c_bt_leaf_ranks_) {
        c_bt_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }


    int bt_prefix_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::int_vector<> *vector : bt_prefix_leaf_ranks_) {
        bt_prefix_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_prefix_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>> *vector : c_bt_prefix_leaf_ranks_) {
        c_bt_prefix_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }


    int bt_cumulated_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::int_vector<> *vector : bt_cumulated_leaf_ranks_) {
        bt_cumulated_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_cumulated_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>> *vector : c_bt_cumulated_leaf_ranks_) {
        c_bt_cumulated_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }


    int bt_first_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::int_vector<> *vector : bt_first_leaf_ranks_) {
        bt_first_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_first_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>> *vector : c_bt_first_leaf_ranks_) {
        c_bt_first_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }


    int bt_second_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::int_vector<> *vector : bt_second_leaf_ranks_) {
        bt_second_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_second_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>> *vector : c_bt_second_leaf_ranks_) {
        c_bt_second_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }



    int bt_zeros_ranks_total_size = sizeof(void *);
    for (sdsl::int_vector<> *vector : bt_zeros_ranks_) {
        bt_zeros_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_zeros_ranks_total_size = sizeof(void *);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>> *vector : c_bt_zeros_ranks_) {
        c_bt_zeros_ranks_total_size += sdsl::size_in_bytes(*vector);
    }


    int bt_prefix_zeros_ranks_total_size = sizeof(void *);
    for (sdsl::int_vector<> *vector : bt_prefix_zeros_ranks_) {
        bt_prefix_zeros_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_prefix_zeros_ranks_total_size = sizeof(void *);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>> *vector : c_bt_prefix_zeros_ranks_) {
        c_bt_prefix_zeros_ranks_total_size += sdsl::size_in_bytes(*vector);
    }


    int bt_cumulated_zeros_ranks_total_size = sizeof(void *);
    for (sdsl::int_vector<> *vector : bt_cumulated_zeros_ranks_) {
        bt_cumulated_zeros_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_cumulated_zeros_ranks_total_size = sizeof(void *);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>> *vector : c_bt_cumulated_zeros_ranks_) {
        c_bt_cumulated_zeros_ranks_total_size += sdsl::size_in_bytes(*vector);
    }


    int bt_first_zeros_ranks_total_size = sizeof(void *);
    for (sdsl::int_vector<> *vector : bt_first_zeros_ranks_) {
        bt_first_zeros_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_first_zeros_ranks_total_size = sizeof(void *);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>> *vector : c_bt_first_zeros_ranks_) {
        c_bt_first_zeros_ranks_total_size += sdsl::size_in_bytes(*vector);
    }


    int bt_second_zeros_ranks_total_size = sizeof(void *);
    for (sdsl::int_vector<> *vector : bt_second_zeros_ranks_) {
        bt_second_zeros_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_second_zeros_ranks_total_size = sizeof(void *);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>> *vector : c_bt_second_zeros_ranks_) {
        c_bt_second_zeros_ranks_total_size += sdsl::size_in_bytes(*vector);
    }



    //JEA15 ENCODING
    int bt_jea15_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::int_vector<> *vector : bt_jea15_leaf_ranks_) {
        bt_jea15_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_jea15_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>> *vector : c_bt_jea15_leaf_ranks_) {
        c_bt_jea15_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }

/*
    int bt_jea15_prefix_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::int_vector<> *vector : bt_jea15_prefix_leaf_ranks_) {
        bt_jea15_prefix_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_jea15_prefix_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>> *vector : c_bt_jea15_prefix_leaf_ranks_) {
        c_bt_jea15_prefix_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }


    int bt_jea15_cumulated_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::int_vector<> *vector : bt_jea15_cumulated_leaf_ranks_) {
        bt_jea15_cumulated_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_jea15_cumulated_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>> *vector : c_bt_jea15_cumulated_leaf_ranks_) {
        c_bt_jea15_cumulated_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
*/

    int bt_jea15_first_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::int_vector<> *vector : bt_jea15_first_leaf_ranks_) {
        bt_jea15_first_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_jea15_first_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>> *vector : c_bt_jea15_first_leaf_ranks_) {
        c_bt_jea15_first_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }


    int bt_jea15_second_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::int_vector<> *vector : bt_jea15_second_leaf_ranks_) {
        bt_jea15_second_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_jea15_second_leaf_ranks_total_size = sizeof(void *);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>> *vector : c_bt_jea15_second_leaf_ranks_) {
        c_bt_jea15_second_leaf_ranks_total_size += sdsl::size_in_bytes(*vector);
    }
    //JEA15 ENCODING


    int leaf_rank_paper_version_size =
            bt_first_level_prefix_leaf_ranks_size + bt_first_leaf_ranks_total_size + bt_second_leaf_ranks_total_size +
            bt_prefix_leaf_ranks_total_size + bt_starts_with_end_leaf_size + bt_suffix_start_with_end_leaf_size +
            bt_prefix_start_with_end_leaf_size;
    int leaf_rank_alternative_version_size =
            bt_second_leaf_ranks_total_size + bt_leaf_ranks_total_size + bt_first_level_prefix_leaf_ranks_size +
            bt_starts_with_end_leaf_size + bt_suffix_start_with_end_leaf_size + bt_prefix_start_with_end_leaf_size;
    int leaf_rank_better_version_size = bt_second_leaf_ranks_total_size + bt_first_level_cumulated_leaf_ranks_size +
                                        bt_cumulated_leaf_ranks_total_size + bt_starts_with_end_leaf_size +
                                        bt_suffix_start_with_end_leaf_size + bt_prefix_start_with_end_leaf_size;
    int c_leaf_rank_paper_version_size = c_bt_first_level_prefix_leaf_ranks_size + c_bt_first_leaf_ranks_total_size +
                                         c_bt_second_leaf_ranks_total_size + c_bt_prefix_leaf_ranks_total_size +
                                         c_bt_starts_with_end_leaf_size + c_bt_suffix_start_with_end_leaf_size +
                                         c_bt_prefix_start_with_end_leaf_size;
    int c_leaf_rank_alternative_version_size =
            c_bt_second_leaf_ranks_total_size + c_bt_leaf_ranks_total_size + c_bt_first_level_prefix_leaf_ranks_size +
            c_bt_starts_with_end_leaf_size + c_bt_suffix_start_with_end_leaf_size +
            c_bt_prefix_start_with_end_leaf_size;
    int c_leaf_rank_better_version_size =
            c_bt_second_leaf_ranks_total_size + c_bt_first_level_cumulated_leaf_ranks_size +
            c_bt_cumulated_leaf_ranks_total_size + c_bt_starts_with_end_leaf_size +
            c_bt_suffix_start_with_end_leaf_size + c_bt_prefix_start_with_end_leaf_size;


    int zeros_rank_paper_version_size =
            bt_first_level_prefix_zeros_ranks_size + bt_first_zeros_ranks_total_size + bt_second_zeros_ranks_total_size +
            bt_prefix_zeros_ranks_total_size;
    int zeros_rank_alternative_version_size =
            bt_second_zeros_ranks_total_size + bt_zeros_ranks_total_size + bt_first_level_prefix_zeros_ranks_size;
    int zeros_rank_better_version_size = bt_second_zeros_ranks_total_size + bt_first_level_cumulated_zeros_ranks_size +
                                        bt_cumulated_zeros_ranks_total_size;
    int c_zeros_rank_paper_version_size = c_bt_first_level_prefix_zeros_ranks_size + c_bt_first_zeros_ranks_total_size +
                                         c_bt_second_zeros_ranks_total_size + c_bt_prefix_zeros_ranks_total_size;
    int c_zeros_rank_alternative_version_size =
            c_bt_second_zeros_ranks_total_size + c_bt_zeros_ranks_total_size + c_bt_first_level_prefix_zeros_ranks_size;
    int c_zeros_rank_better_version_size =
            c_bt_second_zeros_ranks_total_size + c_bt_first_level_cumulated_zeros_ranks_size +
            c_bt_cumulated_zeros_ranks_total_size;

    // JEA15 ENCODING
    /*
    int jea15_leaf_rank_paper_version_size =
            bt_jea15_first_level_prefix_leaf_ranks_size + bt_jea15_first_leaf_ranks_total_size + bt_jea15_second_leaf_ranks_total_size +
            bt_jea15_prefix_leaf_ranks_total_size + bt_starts_with_end_leaf_size + bt_suffix_start_with_end_leaf_size +
            bt_prefix_start_with_end_leaf_size;
            */
    int jea15_leaf_rank_alternative_version_size =
            bt_jea15_second_leaf_ranks_total_size + bt_jea15_leaf_ranks_total_size + bt_first_level_prefix_leaf_ranks_size +
            bt_starts_with_end_leaf_size + bt_suffix_start_with_end_leaf_size + bt_prefix_start_with_end_leaf_size;
    /*
    int jea15_leaf_rank_better_version_size = bt_jea15_second_leaf_ranks_total_size + bt_jea15_first_level_cumulated_leaf_ranks_size +
                                        bt_jea15_cumulated_leaf_ranks_total_size + bt_starts_with_end_leaf_size +
                                        bt_suffix_start_with_end_leaf_size + bt_prefix_start_with_end_leaf_size;

    int jea15_c_leaf_rank_paper_version_size = c_bt_jea15_first_level_prefix_leaf_ranks_size + c_bt_jea15_first_leaf_ranks_total_size +
                                         c_bt_jea15_second_leaf_ranks_total_size + c_bt_jea15_prefix_leaf_ranks_total_size +
                                         c_bt_starts_with_end_leaf_size + c_bt_suffix_start_with_end_leaf_size +
                                         c_bt_prefix_start_with_end_leaf_size;
                                         */
    int jea15_c_leaf_rank_alternative_version_size =
            c_bt_jea15_second_leaf_ranks_total_size + c_bt_jea15_leaf_ranks_total_size + c_bt_first_level_prefix_leaf_ranks_size +
            c_bt_starts_with_end_leaf_size + c_bt_suffix_start_with_end_leaf_size +
            c_bt_prefix_start_with_end_leaf_size;
    /*
    int jea15_c_leaf_rank_better_version_size =
            c_bt_jea15_second_leaf_ranks_total_size + c_bt_jea15_first_level_cumulated_leaf_ranks_size +
            c_bt_jea15_cumulated_leaf_ranks_total_size + c_bt_starts_with_end_leaf_size +
            c_bt_suffix_start_with_end_leaf_size + c_bt_prefix_start_with_end_leaf_size;
            */
    // JEA15 ENCODING



    std::cout << std::endl << "LEAF RANK" << std::endl;
    std::cout << "first_level_prefix_leaf_ ranks Plain(B): " << bt_first_level_prefix_leaf_ranks_size << std::endl;
    std::cout << "first_level_cumulated_leaf_ranks Plain(B): " << bt_first_level_cumulated_leaf_ranks_size << std::endl;
    std::cout << "leaf_ranks Size Plain(B): " << bt_leaf_ranks_total_size << std::endl;
    std::cout << "prefix_leaf_ranks Size Plain(B): " << bt_prefix_leaf_ranks_total_size << std::endl;
    std::cout << "cumulated_leaf_ranks Size Plain(B): " << bt_cumulated_leaf_ranks_total_size << std::endl;
    std::cout << "first_leaf_ranks Size Plain(B): " << bt_first_leaf_ranks_total_size << std::endl;
    std::cout << "second_leaf_ranks Size Plain(B): " << bt_second_leaf_ranks_total_size << std::endl << std::endl;
    std::cout << "cumulated_leaf_ranks Size Plain(B): " << bt_cumulated_leaf_ranks_total_size << std::endl;
    std::cout << "first_leaf_ranks Size Plain(B): " << bt_first_leaf_ranks_total_size << std::endl;
    std::cout << "second_leaf_ranks Size Plain(B): " << bt_second_leaf_ranks_total_size << std::endl;
    std::cout << "starts_with_end_leaf Plain(B): " << bt_starts_with_end_leaf_size << std::endl;
    std::cout << "suffix_start_with_end_leaf Plain(B): " << bt_suffix_start_with_end_leaf_size << std::endl;
    std::cout << "prefix_start_with_end_leaf Plain(B): " << bt_prefix_start_with_end_leaf_size << std::endl
              << std::endl;


    std::cout << std::endl << "Compressed LEAF RANK" << std::endl;
    std::cout << "Compressed first_level_prefix_leaf_ ranks (B): " << c_bt_first_level_prefix_leaf_ranks_size
              << std::endl;
    std::cout << "Compressed first_level_cumulated_leaf_ranks (B): " << c_bt_first_level_cumulated_leaf_ranks_size
              << std::endl;
    std::cout << "Compressed leaf_ranks Size (B): " << c_bt_leaf_ranks_total_size << std::endl;
    std::cout << "Compressed prefix_leaf_ranks Size (B): " << c_bt_prefix_leaf_ranks_total_size << std::endl;
    std::cout << "Compressed cumulated_leaf_ranks Size (B): " << c_bt_cumulated_leaf_ranks_total_size << std::endl;
    std::cout << "Compressed first_leaf_ranks Size (B): " << c_bt_first_leaf_ranks_total_size << std::endl;
    std::cout << "Compressed second_leaf_ranks Size (B): " << c_bt_second_leaf_ranks_total_size << std::endl;
    std::cout << "Compressed cumulated_leaf_ranks Size (B): " << c_bt_cumulated_leaf_ranks_total_size << std::endl;
    std::cout << "Compressed first_leaf_ranks Size (B): " << c_bt_first_leaf_ranks_total_size << std::endl;
    std::cout << "Compressed second_leaf_ranks Size (B): " << c_bt_second_leaf_ranks_total_size << std::endl;
    std::cout << "Compressed starts_with_end_leaf (B): " << c_bt_starts_with_end_leaf_size << std::endl;
    std::cout << "Compressed suffix_start_with_end_leaf (B): " << c_bt_suffix_start_with_end_leaf_size << std::endl;
    std::cout << "Compressed prefix_start_with_end_leaf (B): " << c_bt_prefix_start_with_end_leaf_size << std::endl
              << std::endl;

    std::cout << "LEAF RANK Paper Version Size(B): " << leaf_rank_paper_version_size << std::endl;
    std::cout << "LEAF RANK Alternative Version Size(B): " << leaf_rank_alternative_version_size << std::endl;
    std::cout << "LEAF RANK Better Version Size(B): " << leaf_rank_better_version_size << std::endl << std::endl;

    std::cout << "Compressed LEAF RANK Paper Version Size(B): " << c_leaf_rank_paper_version_size << std::endl;
    std::cout << "Compressed LEAF RANK Alternative Version Size(B): " << c_leaf_rank_alternative_version_size
              << std::endl;
    std::cout << "Compressed LEAF RANK Better Version Size(B): " << c_leaf_rank_better_version_size << std::endl
              << std::endl;



    std::cout << std::endl << "ZEROS RANK" << std::endl;
    std::cout << "first_level_prefix_zeros_ ranks Plain(B): " << bt_first_level_prefix_zeros_ranks_size << std::endl;
    std::cout << "first_level_cumulated_zeros_ranks Plain(B): " << bt_first_level_cumulated_zeros_ranks_size << std::endl;
    std::cout << "zeros_ranks Size Plain(B): " << bt_zeros_ranks_total_size << std::endl;
    std::cout << "prefix_zeros_ranks Size Plain(B): " << bt_prefix_zeros_ranks_total_size << std::endl;
    std::cout << "cumulated_zeros_ranks Size Plain(B): " << bt_cumulated_zeros_ranks_total_size << std::endl;
    std::cout << "first_zeros_ranks Size Plain(B): " << bt_first_zeros_ranks_total_size << std::endl;
    std::cout << "second_zeros_ranks Size Plain(B): " << bt_second_zeros_ranks_total_size << std::endl << std::endl;
    std::cout << "cumulated_zeros_ranks Size Plain(B): " << bt_cumulated_zeros_ranks_total_size << std::endl;
    std::cout << "first_zeros_ranks Size Plain(B): " << bt_first_zeros_ranks_total_size << std::endl;
    std::cout << "second_zeros_ranks Size Plain(B): " << bt_second_zeros_ranks_total_size << std::endl<< std::endl;


    std::cout << std::endl << "Compressed ZEROS RANK" << std::endl;
    std::cout << "Compressed first_level_prefix_zeros_ ranks (B): " << c_bt_first_level_prefix_zeros_ranks_size
              << std::endl;
    std::cout << "Compressed first_level_cumulated_zeros_ranks (B): " << c_bt_first_level_cumulated_zeros_ranks_size
              << std::endl;
    std::cout << "Compressed zeros_ranks Size (B): " << c_bt_zeros_ranks_total_size << std::endl;
    std::cout << "Compressed prefix_zeros_ranks Size (B): " << c_bt_prefix_zeros_ranks_total_size << std::endl;
    std::cout << "Compressed cumulated_zeros_ranks Size (B): " << c_bt_cumulated_zeros_ranks_total_size << std::endl;
    std::cout << "Compressed first_zeros_ranks Size (B): " << c_bt_first_zeros_ranks_total_size << std::endl;
    std::cout << "Compressed second_zeros_ranks Size (B): " << c_bt_second_zeros_ranks_total_size << std::endl;
    std::cout << "Compressed cumulated_zeros_ranks Size (B): " << c_bt_cumulated_zeros_ranks_total_size << std::endl;
    std::cout << "Compressed first_zeros_ranks Size (B): " << c_bt_first_zeros_ranks_total_size << std::endl;
    std::cout << "Compressed second_zeros_ranks Size (B): " << c_bt_second_zeros_ranks_total_size << std::endl<< std::endl;

    std::cout << "ZEROS RANK Paper Version Size(B): " << zeros_rank_paper_version_size << std::endl;
    std::cout << "ZEROS RANK Alternative Version Size(B): " << zeros_rank_alternative_version_size << std::endl;
    std::cout << "ZEROS RANK Better Version Size(B): " << zeros_rank_better_version_size << std::endl << std::endl;

    std::cout << "Compressed ZEROS RANK Paper Version Size(B): " << c_zeros_rank_paper_version_size << std::endl;
    std::cout << "Compressed ZEROS RANK Alternative Version Size(B): " << c_zeros_rank_alternative_version_size
              << std::endl;
    std::cout << "Compressed ZEROS RANK Better Version Size(B): " << c_zeros_rank_better_version_size << std::endl
              << std::endl;



    // JEA15 ENCODING
    std::cout << std::endl << "JEA15 LEAF RANK" << std::endl;
//    std::cout << "jea15_first_level_prefix_leaf_ ranks Plain(B): " << bt_jea15_first_level_prefix_leaf_ranks_size << std::endl;
//    std::cout << "jea15_first_level_cumulated_leaf_ranks Plain(B): " << bt_jea15_first_level_cumulated_leaf_ranks_size << std::endl;
    std::cout << "jea15_leaf_ranks Size Plain(B): " << bt_jea15_leaf_ranks_total_size << std::endl;
    //std::cout << "jea15_prefix_leaf_ranks Size Plain(B): " << bt_jea15_prefix_leaf_ranks_total_size << std::endl;
    //std::cout << "jea15_cumulated_leaf_ranks Size Plain(B): " << bt_jea15_cumulated_leaf_ranks_total_size << std::endl;
    std::cout << "jea15_first_leaf_ranks Size Plain(B): " << bt_jea15_first_leaf_ranks_total_size << std::endl;
    std::cout << "jea15_second_leaf_ranks Size Plain(B): " << bt_jea15_second_leaf_ranks_total_size << std::endl << std::endl;
    //std::cout << "jea15_cumulated_leaf_ranks Size Plain(B): " << bt_jea15_cumulated_leaf_ranks_total_size << std::endl;
    std::cout << "jea15_first_leaf_ranks Size Plain(B): " << bt_jea15_first_leaf_ranks_total_size << std::endl;
    std::cout << "jea15_second_leaf_ranks Size Plain(B): " << bt_jea15_second_leaf_ranks_total_size << std::endl;
    std::cout << "starts_with_end_leaf Plain(B): " << bt_starts_with_end_leaf_size << std::endl;
    std::cout << "suffix_start_with_end_leaf Plain(B): " << bt_suffix_start_with_end_leaf_size << std::endl;
    std::cout << "prefix_start_with_end_leaf Plain(B): " << bt_prefix_start_with_end_leaf_size << std::endl
              << std::endl;


    std::cout << std::endl << "JEA15 Compressed LEAF RANK" << std::endl;
    //std::cout << "Compressed jea15_first_level_prefix_leaf_ ranks (B): " << c_bt_jea15_first_level_prefix_leaf_ranks_size << std::endl;
    //std::cout << "Compressed jea15_first_level_cumulated_leaf_ranks (B): " << c_bt_jea15_first_level_cumulated_leaf_ranks_size << std::endl;
    std::cout << "Compressed jea15_leaf_ranks Size (B): " << c_bt_jea15_leaf_ranks_total_size << std::endl;
    //std::cout << "Compressed jea15_prefix_leaf_ranks Size (B): " << c_bt_jea15_prefix_leaf_ranks_total_size << std::endl;
    //std::cout << "Compressed jea15_cumulated_leaf_ranks Size (B): " << c_bt_jea15_cumulated_leaf_ranks_total_size << std::endl;
    std::cout << "Compressed jea15_first_leaf_ranks Size (B): " << c_bt_jea15_first_leaf_ranks_total_size << std::endl;
    std::cout << "Compressed jea15_second_leaf_ranks Size (B): " << c_bt_jea15_second_leaf_ranks_total_size << std::endl;
    //std::cout << "Compressed jea15_cumulated_leaf_ranks Size (B): " << c_bt_jea15_cumulated_leaf_ranks_total_size << std::endl;
    std::cout << "Compressed jea15_first_leaf_ranks Size (B): " << c_bt_jea15_first_leaf_ranks_total_size << std::endl;
    std::cout << "Compressed jea15_second_leaf_ranks Size (B): " << c_bt_jea15_second_leaf_ranks_total_size << std::endl;
    std::cout << "Compressed starts_with_end_leaf (B): " << c_bt_starts_with_end_leaf_size << std::endl;
    std::cout << "Compressed suffix_start_with_end_leaf (B): " << c_bt_suffix_start_with_end_leaf_size << std::endl;
    std::cout << "Compressed prefix_start_with_end_leaf (B): " << c_bt_prefix_start_with_end_leaf_size << std::endl
              << std::endl;

    //std::cout << "JEA15 LEAF RANK Paper Version Size(B): " << jea15_leaf_rank_paper_version_size << std::endl;
    std::cout << "JEA15 LEAF RANK Alternative Version Size(B): " << jea15_leaf_rank_alternative_version_size << std::endl;
    //std::cout << "JEA15 LEAF RANK Better Version Size(B): " << jea15_leaf_rank_better_version_size << std::endl << std::endl;

    //std::cout << "JEA15 Compressed LEAF RANK Paper Version Size(B): " << jea15_c_leaf_rank_paper_version_size << std::endl;
    std::cout << "JEA15 Compressed LEAF RANK Alternative Version Size(B): " << jea15_c_leaf_rank_alternative_version_size
              << std::endl;
    //std::cout << "JEA15 Compressed LEAF RANK Better Version Size(B): " << jea15_c_leaf_rank_better_version_size << std::endl<< std::endl;
    // JEA15 ENCODING



    int  bt_bv_select_1_size = sizeof(void*);
    for (sdsl::bit_vector::select_1_type* select: bt_bv_select_1_) {
        bt_bv_select_1_size += sdsl::size_in_bytes(*select);
    }

    int  c_bt_bv_select_1_size = sizeof(void*);
    for (sdsl::rrr_vector<63>::select_1_type* select: c_bt_bv_select_1_) {
        c_bt_bv_select_1_size += sdsl::size_in_bytes(*select);
    }

    int top_excess_size = sdsl::size_in_bytes(*top_excess_);
    int top_min_excess_size = sdsl::size_in_bytes(*top_min_excess_);
    int top_max_excess_size = sdsl::size_in_bytes(*top_max_excess_);
    int top_min_count_size = sdsl::size_in_bytes(*top_min_count_);

    int c_top_excess_size = sdsl::size_in_bytes(*c_top_excess_);
    int c_top_min_excess_size = sdsl::size_in_bytes(*c_top_min_excess_);
    int c_top_max_excess_size = sdsl::size_in_bytes(*c_top_max_excess_);
    int c_top_min_count_size = sdsl::size_in_bytes(*c_top_min_count_);


    int bt_excess_size = sizeof(void*);
    for (sdsl::int_vector<>* vector: bt_excess_) {
        bt_excess_size += sdsl::size_in_bytes(*vector);
    }
    int bt_excess_first_block_size = sizeof(void*);
    for (sdsl::int_vector<>* vector: bt_excess_first_block_) {
        bt_excess_first_block_size += sdsl::size_in_bytes(*vector);
    }
    int bt_excess_second_block_size = sizeof(void*);
    for (sdsl::int_vector<>* vector: bt_excess_second_block_) {
        bt_excess_second_block_size += sdsl::size_in_bytes(*vector);
    }

    int c_bt_excess_size = sizeof(void*);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector: c_bt_excess_) {
        c_bt_excess_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_excess_first_block_size = sizeof(void*);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector: c_bt_excess_first_block_) {
        c_bt_excess_first_block_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_excess_second_block_size = sizeof(void*);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector: c_bt_excess_second_block_) {
        c_bt_excess_second_block_size += sdsl::size_in_bytes(*vector);
    }


    int bt_prefix_min_excess_size = sizeof(void*);
    for (sdsl::int_vector<>* vector: bt_prefix_min_excess_) {
        bt_prefix_min_excess_size += sdsl::size_in_bytes(*vector);
    }
    int bt_prefix_min_excess_first_block_size = sizeof(void*);
    for (sdsl::int_vector<>* vector: bt_prefix_min_excess_first_block_) {
        bt_prefix_min_excess_first_block_size += sdsl::size_in_bytes(*vector);
    }
    int bt_prefix_min_excess_second_block_size = sizeof(void*);
    for (sdsl::int_vector<>* vector: bt_prefix_min_excess_second_block_) {
        bt_prefix_min_excess_second_block_size += sdsl::size_in_bytes(*vector);
    }
    int bt_prefix_min_excess_back_block_size = sizeof(void*);
    for (sdsl::int_vector<>* vector: bt_prefix_min_excess_back_block_) {
        bt_prefix_min_excess_back_block_size += sdsl::size_in_bytes(*vector);
    }



    int c_bt_prefix_min_excess_size = sizeof(void*);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector: c_bt_prefix_min_excess_) {
        c_bt_prefix_min_excess_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_prefix_min_excess_first_block_size = sizeof(void*);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector: c_bt_prefix_min_excess_first_block_) {
        c_bt_prefix_min_excess_first_block_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_prefix_min_excess_second_block_size = sizeof(void*);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector: c_bt_prefix_min_excess_second_block_) {
        c_bt_prefix_min_excess_second_block_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_prefix_min_excess_back_block_size = sizeof(void*);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector: c_bt_prefix_min_excess_back_block_) {
        c_bt_prefix_min_excess_back_block_size += sdsl::size_in_bytes(*vector);
    }



    int bt_prefix_max_excess_size = sizeof(void*);
    for (sdsl::int_vector<>* vector: bt_prefix_max_excess_) {
        bt_prefix_max_excess_size += sdsl::size_in_bytes(*vector);
    }
    int bt_prefix_max_excess_first_block_size = sizeof(void*);
    for (sdsl::int_vector<>* vector: bt_prefix_max_excess_first_block_) {
        bt_prefix_max_excess_first_block_size += sdsl::size_in_bytes(*vector);
    }
    int bt_prefix_max_excess_second_block_size = sizeof(void*);
    for (sdsl::int_vector<>* vector: bt_prefix_max_excess_second_block_) {
        bt_prefix_max_excess_second_block_size += sdsl::size_in_bytes(*vector);
    }
    int bt_prefix_max_excess_back_block_size = sizeof(void*);
    for (sdsl::int_vector<>* vector: bt_prefix_max_excess_back_block_) {
        bt_prefix_max_excess_back_block_size += sdsl::size_in_bytes(*vector);
    }

    int c_bt_prefix_max_excess_size = sizeof(void*);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector: c_bt_prefix_max_excess_) {
        c_bt_prefix_max_excess_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_prefix_max_excess_first_block_size = sizeof(void*);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector: c_bt_prefix_max_excess_first_block_) {
        c_bt_prefix_max_excess_first_block_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_prefix_max_excess_second_block_size = sizeof(void*);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector: c_bt_prefix_max_excess_second_block_) {
        c_bt_prefix_max_excess_second_block_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_prefix_max_excess_back_block_size = sizeof(void*);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector: c_bt_prefix_max_excess_back_block_) {
        c_bt_prefix_max_excess_back_block_size += sdsl::size_in_bytes(*vector);
    }



    int bt_min_count_size = sizeof(void*);
    for (sdsl::int_vector<>* vector: bt_min_count_) {
        bt_min_count_size += sdsl::size_in_bytes(*vector);
    }
    int bt_min_count_first_block_size = sizeof(void*);
    for (sdsl::int_vector<>* vector: bt_min_count_first_block_) {
        bt_min_count_first_block_size += sdsl::size_in_bytes(*vector);
    }
    int bt_min_count_second_block_size = sizeof(void*);
    for (sdsl::int_vector<>* vector: bt_min_count_second_block_) {
        bt_min_count_second_block_size += sdsl::size_in_bytes(*vector);
    }
    int bt_min_count_back_block_size = sizeof(void*);
    for (sdsl::int_vector<>* vector: bt_min_count_back_block_) {
        bt_min_count_back_block_size += sdsl::size_in_bytes(*vector);
    }

    int c_bt_min_count_size = sizeof(void*);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector: c_bt_min_count_) {
        c_bt_min_count_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_min_count_first_block_size = sizeof(void*);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector: c_bt_min_count_first_block_) {
        c_bt_min_count_first_block_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_min_count_second_block_size = sizeof(void*);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector: c_bt_min_count_second_block_) {
        c_bt_min_count_second_block_size += sdsl::size_in_bytes(*vector);
    }
    int c_bt_min_count_back_block_size = sizeof(void*);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* vector: c_bt_min_count_back_block_) {
        c_bt_min_count_back_block_size += sdsl::size_in_bytes(*vector);
    }


    std::cout << "select_1 on types for no recursion Size(B): " << bt_bv_select_1_size << std::endl;
    std::cout << "Compressed select_1 on types for no recursion Size(B): " << c_bt_bv_select_1_size << std::endl;
    std::cout << "TOP STRUCTURES" << std::endl;
    std::cout << "Top Excess Size(B): " << top_excess_size << std::endl;
    std::cout << "Top Min Excess Size(B): " << top_min_excess_size << std::endl;
    std::cout << "Top Max Excess Size(B): " << top_max_excess_size << std::endl;
    std::cout << "Top Min Count Size(B): " << top_min_count_size << std::endl;
    std::cout << "COMPRESSED TOP STRUCTURES" << std::endl;
    std::cout << "Compressed Top Excess Size(B): " << c_top_excess_size << std::endl;
    std::cout << "Compressed Top Min Excess Size(B): " << c_top_min_excess_size << std::endl;
    std::cout << "Compressed Top Max Excess Size(B): " << c_top_max_excess_size << std::endl;
    std::cout << "Compressed Top Min Count Size(B): " << c_top_min_count_size << std::endl;

    std::cout << "BOTTOM STRUCTURES" << std::endl;
    std::cout << "Bottom Excess Size(B): " << bottom_excess_size << std::endl;
    std::cout << "Bottom Min Excess Size(B): " << bottom_min_excess_size << std::endl;
    std::cout << "Bottom Max Excess Size(B): " << bottom_max_excess_size << std::endl;
    std::cout << "Bottom Min Count Size(B): " << bottom_min_count_size << std::endl;
    std::cout << "Bottom Excess 2 Size(B): " << bottom_excess_2_size << std::endl;
    std::cout << "Bottom Min Excess 2 Size(B): " << bottom_min_excess_2_size << std::endl;
    std::cout << "Bottom Max Excess 2 Size(B): " << bottom_max_excess_2_size << std::endl;
    std::cout << "Bottom Min Count 2 Size(B): " << bottom_min_count_2_size << std::endl;
    std::cout << "COMPRESSED BOTTOM STRUCTURES" << std::endl;
    std::cout << "Compressed Bottom Excess Size(B): " << c_bottom_excess_size << std::endl;
    std::cout << "Compressed Bottom Min Excess Size(B): " << c_bottom_min_excess_size << std::endl;
    std::cout << "Compressed Bottom Max Excess Size(B): " << c_bottom_max_excess_size << std::endl;
    std::cout << "Compressed Bottom Min Count Size(B): " << c_bottom_min_count_size << std::endl;


    std::cout << "EXCESS INFO" << std::endl;
    std::cout << "Excess Size(B): " << bt_excess_size<< std::endl;
    std::cout << "Min Excess Size(B): " << bt_prefix_min_excess_size<< std::endl;
    std::cout << "Max Excess Size(B): " << bt_prefix_max_excess_size<< std::endl;
    std::cout << "Min Count Size(B): " << bt_min_count_size<< std::endl;
    std::cout << "Excess First Block Size(B): " << bt_excess_first_block_size<< std::endl;
    std::cout << "Min Excess First Block Size(B): " << bt_prefix_min_excess_first_block_size<< std::endl;
    std::cout << "Max Excess First Block Size(B): " << bt_prefix_max_excess_first_block_size<< std::endl;
    std::cout << "Min Count First Block Size(B): " << bt_min_count_first_block_size<< std::endl;
    std::cout << "Excess Second Block Size(B): " << bt_excess_second_block_size<< std::endl;
    std::cout << "Min Excess Second Block Size(B): " << bt_prefix_min_excess_second_block_size<< std::endl;
    std::cout << "Max Excess Second Block Size(B): " << bt_prefix_max_excess_second_block_size<< std::endl;
    std::cout << "Min Count Second Block Size(B): " << bt_min_count_second_block_size<< std::endl;
    std::cout << "Min Excess Back Block Size(B): " << bt_prefix_min_excess_back_block_size<< std::endl;
    std::cout << "min_in_first_block_bv Size(B): " << bt_min_in_first_block_size<< std::endl;
    std::cout << "min_in_both_blocks_bv Size(B): " << bt_min_in_both_blocks_size<< std::endl;
    std::cout << "min_in_both_blocks_rank Size(B): " << bt_min_in_both_blocks_rank_size<< std::endl;
    std::cout << "Max Excess Back Block Size(B): " << bt_prefix_max_excess_back_block_size<< std::endl;
    std::cout << "max_in_first_block_bv Size(B): " << bt_max_in_first_block_size<< std::endl;
    std::cout << "max_in_both_blocks_bv Size(B): " << bt_max_in_both_blocks_size<< std::endl;
    std::cout << "max_in_both_blocks_rank Size(B): " << bt_max_in_both_blocks_rank_size<< std::endl;
    std::cout << "Min Count Back Block Size(B): " << bt_min_count_back_block_size<< std::endl;

    std::cout << "COMPRESSED EXCESS INFO" << std::endl;
    std::cout << "Compressed Excess Size(B): " << c_bt_excess_size<< std::endl;
    std::cout << "Compressed Min Excess Size(B): " << c_bt_prefix_min_excess_size<< std::endl;
    std::cout << "Compressed Max Excess Size(B): " << c_bt_prefix_max_excess_size<< std::endl;
    std::cout << "Compressed Min Count Size(B): " << c_bt_min_count_size<< std::endl;
    std::cout << "Compressed Excess First Block Size(B): " << c_bt_excess_first_block_size<< std::endl;
    std::cout << "Compressed Min Excess First Block Size(B): " << c_bt_prefix_min_excess_first_block_size<< std::endl;
    std::cout << "Compressed Max Excess First Block Size(B): " << c_bt_prefix_max_excess_first_block_size<< std::endl;
    std::cout << "Compressed Min Count First Block Size(B): " << c_bt_min_count_first_block_size<< std::endl;
    std::cout << "Compressed Excess Second Block Size(B): " << c_bt_excess_second_block_size<< std::endl;
    std::cout << "Compressed Min Excess Second Block Size(B): " << c_bt_prefix_min_excess_second_block_size<< std::endl;
    std::cout << "Compressed Max Excess Second Block Size(B): " << c_bt_prefix_max_excess_second_block_size<< std::endl;
    std::cout << "Compressed Min Count Second Block Size(B): " << c_bt_min_count_second_block_size<< std::endl;
    std::cout << "Compressed Min Excess Back Block Size(B): " << c_bt_prefix_min_excess_back_block_size<< std::endl;
    std::cout << "Compressed min_in_first_block_bv Size(B): " << c_bt_min_in_first_block_size<< std::endl;
    std::cout << "Compressed min_in_both_blocks_bv Size(B): " << c_bt_min_in_both_blocks_size<< std::endl;
    std::cout << "Compressed min_in_both_blocks_rank Size(B): " << c_bt_min_in_both_blocks_rank_size<< std::endl;
    std::cout << "Compressed Max Excess Back Block Size(B): " << c_bt_prefix_max_excess_back_block_size<< std::endl;
    std::cout << "Compressed max_in_first_block_bv Size(B): " << c_bt_max_in_first_block_size<< std::endl;
    std::cout << "Compressed max_in_both_blocks_bv Size(B): " << c_bt_max_in_both_blocks_size<< std::endl;
    std::cout << "Compressed max_in_both_blocks_rank Size(B): " << c_bt_max_in_both_blocks_rank_size<< std::endl;
    std::cout << "Compressed Min Count Back Block Size(B): " << c_bt_min_count_back_block_size<< std::endl;

}

int64_t encoded_excess(int64_t e) {
    bool sgn = e < 0;
    if (sgn) e = -e;
    return 2*e+((sgn)?1:0);
}

int64_t decoded_excess(int64_t e) {
    return (e % 2) ? -(e / 2) : (e / 2);
}
