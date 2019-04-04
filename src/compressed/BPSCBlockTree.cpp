//
// Created by sking32 on 5/28/18.
//

#include <compressed/BPSCBlockTree.h>

BPSCBlockTree::BPSCBlockTree(BlockTree * bt) : CBlockTree(bt) {
    open_ = bt->open_;

    std::vector<Block*> first_level = {bt->root_block_};
    bool is_first_level = false;
    while (!is_first_level) {
        for (Block* b: first_level) {
            is_first_level = is_first_level || b->is_leaf();
        }
        if (is_first_level) break;
        first_level = bt->next_level(first_level);
    }

    int first_level_prefix_ranks_ = 0;
    sdsl::int_vector<>* first_level_prefix_leaf_ranks = new sdsl::int_vector<>(first_level.size());
    sdsl::int_vector<>* first_level_cumulated_leaf_ranks = new sdsl::int_vector<>(first_level.size());
    sdsl::int_vector<>* first_level_leaf_ranks = new sdsl::int_vector<>(first_level.size());

    for (int i = 0; i < first_level.size(); ++i) {
        (*first_level_prefix_leaf_ranks)[i] = first_level_prefix_ranks_;
        (*first_level_leaf_ranks)[i] = first_level[i]->leaf_rank_;
        first_level_prefix_ranks_ += first_level[i]->leaf_rank_;
        (*first_level_cumulated_leaf_ranks)[i] = first_level_prefix_ranks_;
    }

    sdsl::util::bit_compress(*(first_level_prefix_leaf_ranks));
    bt_first_level_prefix_leaf_ranks_ = first_level_prefix_leaf_ranks;
    c_bt_first_level_prefix_leaf_ranks_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*(first_level_prefix_leaf_ranks));

    sdsl::util::bit_compress(*(first_level_cumulated_leaf_ranks));
    bt_first_level_cumulated_leaf_ranks_ = first_level_cumulated_leaf_ranks;
    c_bt_first_level_cumulated_leaf_ranks_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*(first_level_cumulated_leaf_ranks));

    sdsl::util::bit_compress(*(first_level_leaf_ranks));
    bt_leaf_ranks_.push_back(first_level_leaf_ranks);
    c_bt_leaf_ranks_.push_back( new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*(first_level_leaf_ranks)));


    std::vector<Block*> current_level = first_level;
    std::vector<Block*> next_level = bt->next_level(first_level);

    while (next_level.size() != 0) {
        sdsl::bit_vector* current_level_starts_with_end_leaf = new sdsl::bit_vector(current_level.size() ,0);

        sdsl::int_vector<>* next_level_leaf_ranks = new sdsl::int_vector<>(next_level.size());
        sdsl::int_vector<>* next_level_prefix_leaf_ranks = new sdsl::int_vector<>(next_level.size());
        sdsl::int_vector<>* next_level_cumulated_leaf_ranks = new sdsl::int_vector<>(next_level.size());

        int number_of_leaves = 0;
        int current_length = current_level.front()->length();
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
            (*next_level_leaf_ranks)[i] = next_level[i]->leaf_rank_;
            (*next_level_prefix_leaf_ranks)[i] = next_level[i]->prefix_leaf_rank_;
            (*next_level_cumulated_leaf_ranks)[i] = next_level[i]->cumulated_leaf_rank_;
        }



        sdsl::bit_vector* current_level_suffix_start_with_end_leaf = new sdsl::bit_vector(number_of_leaves ,0);
        sdsl::bit_vector* current_level_prefix_start_with_end_leaf = new sdsl::bit_vector(number_of_leaves ,0);

        sdsl::int_vector<>* current_level_first_leaf_ranks = new sdsl::int_vector<>(number_of_leaves);
        sdsl::int_vector<>* current_level_second_leaf_ranks = new sdsl::int_vector<>(number_of_leaves);


        int j = 0;
        for (int i = 0; i < current_level.size(); ++i) {
            if (dynamic_cast<BackBlock*>(current_level[i])) {
                (*current_level_first_leaf_ranks)[j] = current_level[i]->first_leaf_rank_;
                (*current_level_second_leaf_ranks)[j] = current_level[i]->second_leaf_rank_;
                BackBlock* bb  = dynamic_cast<BackBlock*>(current_level[i]);
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
                ++j;
            }
        }

        sdsl::util::bit_compress(*(next_level_leaf_ranks));
        (bt_leaf_ranks_).push_back(next_level_leaf_ranks);
        (c_bt_leaf_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*next_level_leaf_ranks)));


        sdsl::util::bit_compress(*(next_level_prefix_leaf_ranks));
        (bt_prefix_leaf_ranks_).push_back(next_level_prefix_leaf_ranks);
        (c_bt_prefix_leaf_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*next_level_prefix_leaf_ranks)));


        sdsl::util::bit_compress(*(next_level_cumulated_leaf_ranks));
        (bt_cumulated_leaf_ranks_).push_back(next_level_cumulated_leaf_ranks);
        (c_bt_cumulated_leaf_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*next_level_cumulated_leaf_ranks)));


        sdsl::util::bit_compress(*(current_level_first_leaf_ranks));
        (bt_first_leaf_ranks_).push_back(current_level_first_leaf_ranks);
        (c_bt_first_leaf_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*current_level_first_leaf_ranks)));

        sdsl::util::bit_compress(*(current_level_second_leaf_ranks));
        (bt_second_leaf_ranks_).push_back(current_level_second_leaf_ranks);
        (c_bt_second_leaf_ranks_).push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>((*current_level_second_leaf_ranks)));




        bt_starts_with_end_leaf_.push_back(current_level_starts_with_end_leaf);
        c_bt_starts_with_end_leaf_.push_back(new sdsl::rrr_vector<63>(*bt_starts_with_end_leaf_.back()));

        bt_suffix_start_with_end_leaf_.push_back(current_level_suffix_start_with_end_leaf);
        c_bt_suffix_start_with_end_leaf_.push_back(new sdsl::rrr_vector<63>(*bt_suffix_start_with_end_leaf_.back()));

        bt_prefix_start_with_end_leaf_.push_back(current_level_prefix_start_with_end_leaf);
        c_bt_prefix_start_with_end_leaf_.push_back(new sdsl::rrr_vector<63>(*bt_prefix_start_with_end_leaf_.back()));

        current_level = next_level;
        next_level = bt->next_level(current_level);
    }


    std::vector<Block*> last_level = current_level;
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

}

BPSCBlockTree::~BPSCBlockTree() {
    delete bt_first_level_prefix_leaf_ranks_;
    delete bt_first_level_cumulated_leaf_ranks_;

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



    delete c_bt_first_level_prefix_leaf_ranks_;
    delete c_bt_first_level_cumulated_leaf_ranks_;

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
}

int BPSCBlockTree::excess(int i) {
    return 2*rank(open_, i) - i - 1;
}

int BPSCBlockTree::rank_10(int i) {

    int current_block = i/max_length_first_level_;
    int onesRank = (*(bt_bv_block_length_rank_[0]))(current_block+1);
    int zerosRank = current_block+1 - onesRank;
    int acc = i;
    int delta_ones = onesRank;
    int new_guess = current_block;
    while (onesRank*max_length_first_level_ + zerosRank*(max_length_first_level_ - 1) - 1 < i) {
        acc -= (new_guess+1)*max_length_first_level_ - (new_guess + 1 - delta_ones);
        new_guess = acc/max_length_first_level_;
        current_block += 1 + new_guess;
        delta_ones = -onesRank;
        onesRank = (*(bt_bv_block_length_rank_[0]))(current_block+1);
        delta_ones += onesRank;
        zerosRank = current_block+1 - onesRank;
    }
/*
    if  (j < i) {
        initial_block += (i-j-1)/max_length_first_level_;
        onesRank = (*(bt_bv_block_length_rank_[0]))(initial_block+1);
        zerosRank = initial_block+1 - onesRank;
        j = onesRank*max_length_first_level_ + zerosRank*(max_length_first_level_ - 1) - 1;
    }


    while (j < i) {
        ++initial_block;
        j += ((*(bt_bv_block_length_[0]))[initial_block]) ? max_length_first_level_ : max_length_first_level_ - 1;
    }
*/
    int current_length = ((*(bt_bv_block_length_[0]))[current_block]) ? max_length_first_level_ : max_length_first_level_ - 1;

    i -= onesRank*max_length_first_level_ + zerosRank*(max_length_first_level_ - 1) - current_length;
    int max_length_level = max_length_first_level_;
    int level = 0;

    int r = (*bt_first_level_prefix_leaf_ranks_)[current_block];

    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            int big_blocks = (current_length%r_ == 0) ? r_ : current_length%r_;
            current_length = (current_length%r_ == 0) ? current_length/r_ : current_length/r_ + 1;
            if (i < current_length*big_blocks) {
                int child_number = i/current_length;
                i -= child_number*current_length;
                current_block = (*bt_bv_rank_[level])(current_block)*r_ + child_number;
            } else {
                i -= current_length*big_blocks;
                --current_length;
                int child_number = big_blocks + i/current_length;
                i -= (child_number-big_blocks) * current_length;
                current_block = (*bt_bv_rank_[level])(current_block)*r_ + child_number;
            }
            ++level;
            r += (*bt_prefix_leaf_ranks_[level-1])[current_block];
            max_length_level = (max_length_level % r_) ? max_length_level/r_ +1 : max_length_level/r_;
        } else { // Case BackBlock
            int index = current_block  -(*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            int f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
            int s_condition = (*bt_suffix_start_with_end_leaf_[level])[index];
            if (f_condition && !s_condition) ++r;
            if (!f_condition && s_condition) --r;
            current_block = encoded_offset/max_length_level;
            current_length = ((*bt_bv_block_length_[level])[current_block]) ? max_length_level : max_length_level-1;

            i += encoded_offset%max_length_level;
            if (i >= current_length) {
                r += (*bt_second_leaf_ranks_[level])[index];
                ++current_block;
                i -= current_length;
                current_length = ((*bt_bv_block_length_[level])[current_block]) ? max_length_level : max_length_level-1;
                f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                if (f_condition && !s_condition) --r;
                if (!f_condition && s_condition) ++r;
            } else {
                r -= (*bt_first_leaf_ranks_[level])[index];
            }
        }
    }

    onesRank =  (*bt_bv_block_length_rank_[level])(current_block);
    zerosRank = current_block - onesRank;

    i  += (onesRank*max_length_level + zerosRank*(max_length_level-1));
    bool one_seen = (*bt_starts_with_end_leaf_[level])[current_block];
    for (int j = (onesRank*max_length_level + zerosRank*(max_length_level-1)); j <= i; ++j) {
        if ((*leaf_string_)[j] == mapping_[open_]) {
            one_seen = true;
        } else {
            if (one_seen) ++r;
            one_seen = false;
        }
    }

    return r;

}


int BPSCBlockTree::rank_10_alternative(int i) {

    int current_block = i/max_length_first_level_;
    int onesRank = (*(bt_bv_block_length_rank_[0]))(current_block+1);
    int zerosRank = current_block+1 - onesRank;
    int acc = i;
    int delta_ones = onesRank;
    int new_guess = current_block;
    while (onesRank*max_length_first_level_ + zerosRank*(max_length_first_level_ - 1) - 1 < i) {
        acc -= (new_guess+1)*max_length_first_level_ - (new_guess + 1 - delta_ones);
        new_guess = acc/max_length_first_level_;
        current_block += 1 + new_guess;
        delta_ones = -onesRank;
        onesRank = (*(bt_bv_block_length_rank_[0]))(current_block+1);
        delta_ones += onesRank;
        zerosRank = current_block+1 - onesRank;
    }/*
    if  (j < i) {
        initial_block += (i-j-1)/max_length_first_level_;
        onesRank = (*(bt_bv_block_length_rank_[0]))(initial_block+1);
        zerosRank = initial_block+1 - onesRank;
        j = onesRank*max_length_first_level_ + zerosRank*(max_length_first_level_ - 1) - 1;
    }


    while (j < i) {
        ++initial_block;
        j += ((*(bt_bv_block_length_[0]))[initial_block]) ? max_length_first_level_ : max_length_first_level_ - 1;
    }
*/
    int current_length = ((*(bt_bv_block_length_[0]))[current_block]) ? max_length_first_level_ : max_length_first_level_ - 1;
    i -= onesRank*max_length_first_level_ + zerosRank*(max_length_first_level_ - 1) - current_length;

    int max_length_level = max_length_first_level_;
    int level = 0;

    int r = (*bt_first_level_prefix_leaf_ranks_)[current_block];

    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            int big_blocks = (current_length%r_ == 0) ? r_ : current_length%r_;
            current_length = (current_length%r_ == 0) ? current_length/r_ : current_length/r_ + 1;
            int child_number = 0;
            if (i < current_length*big_blocks) {
                child_number = i/current_length;
                i -= child_number*current_length;
            } else {
                i -= current_length*big_blocks;
                --current_length;
                child_number = big_blocks + i/current_length;
                i -= (child_number-big_blocks) * current_length;
            }
            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            for (int child = firstChild; child < firstChild + child_number; ++child)
                r += (*bt_leaf_ranks_[level+1])[child];
            current_block = firstChild + child_number;
            ++level;
            max_length_level = (max_length_level % r_) ? max_length_level/r_ +1 : max_length_level/r_;
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            int f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
            int s_condition = (*bt_suffix_start_with_end_leaf_[level])[index];
            if (f_condition && !s_condition) ++r;
            if (!f_condition && s_condition) --r;

            current_block = encoded_offset/max_length_level;
            current_length = ((*bt_bv_block_length_[level])[current_block]) ? max_length_level : max_length_level-1;
            int offset = encoded_offset%max_length_level;
            if (!offset) {
                if (i >= current_length) {
                    r += (*bt_second_leaf_ranks_[level])[index];

                    ++current_block;
                    i -= current_length;
                    f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                    s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                    if (f_condition && !s_condition) --r;
                    if (!f_condition && s_condition) ++r;
                    current_length = ((*bt_bv_block_length_[level])[current_block]) ? max_length_level :
                                     max_length_level - 1;

                }
            } else {
                r += (*bt_second_leaf_ranks_[level])[index];
                i += offset;
                if (i >= current_length) {
                    ++current_block;
                    i -= current_length;
                    f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                    s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                    if (f_condition && !s_condition) --r;
                    if (!f_condition && s_condition) ++r;
                    current_length = ((*bt_bv_block_length_[level])[current_block]) ? max_length_level :
                                     max_length_level - 1;

                } else {
                    r -= (*bt_leaf_ranks_[level])[current_block];
                }
            }
        }
    }

    onesRank =  (*bt_bv_block_length_rank_[level])(current_block);
    zerosRank = current_block - onesRank;
    i  += (onesRank*max_length_level + zerosRank*(max_length_level-1));
    bool one_seen = (*bt_starts_with_end_leaf_[level])[current_block];
    for (int j = (onesRank*max_length_level + zerosRank*(max_length_level-1)); j <= i; ++j) {
        if ((*leaf_string_)[j] == mapping_[open_]) {
            one_seen = true;
        } else {
            if (one_seen) ++r;
            one_seen = false;
        }
    }

    return r;
}


int BPSCBlockTree::better_rank_10(int i) {

    int current_block = i/max_length_first_level_;
    int onesRank = (*(bt_bv_block_length_rank_[0]))(current_block+1);
    int zerosRank = current_block+1 - onesRank;
    int acc = i;
    int delta_ones = onesRank;
    int new_guess = current_block;
    while (onesRank*max_length_first_level_ + zerosRank*(max_length_first_level_ - 1) - 1 < i) {
        acc -= (new_guess+1)*max_length_first_level_ - (new_guess + 1 - delta_ones);
        new_guess = acc/max_length_first_level_;
        current_block += 1 + new_guess;
        delta_ones = -onesRank;
        onesRank = (*(bt_bv_block_length_rank_[0]))(current_block+1);
        delta_ones += onesRank;
        zerosRank = current_block+1 - onesRank;
    }
/*
    if  (j < i) {
        initial_block += (i-j-1)/max_length_first_level_;
        onesRank = (*(bt_bv_block_length_rank_[0]))(initial_block+1);
        zerosRank = initial_block+1 - onesRank;
        j = onesRank*max_length_first_level_ + zerosRank*(max_length_first_level_ - 1) - 1;
    }


    while (j < i) {
        ++initial_block;
        j += ((*(bt_bv_block_length_[0]))[initial_block]) ? max_length_first_level_ : max_length_first_level_ - 1;
    }
*/
    int current_length = ((*(bt_bv_block_length_[0]))[current_block]) ? max_length_first_level_ : max_length_first_level_ - 1;

    i -= onesRank*max_length_first_level_ + zerosRank*(max_length_first_level_ - 1) - current_length;

    int max_length_level = max_length_first_level_;
    int level = 0;

    int r = (current_block == 0) ? 0 : (*bt_first_level_cumulated_leaf_ranks_)[current_block-1];

    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            int big_blocks = (current_length%r_ == 0) ? r_ : current_length%r_;
            current_length = (current_length%r_ == 0) ? current_length/r_ : current_length/r_ + 1;
            if (i < current_length*big_blocks) { //why the difference with alternative? both do this code
                int child_number = i/current_length;
                i -= child_number*current_length;
                current_block = (*bt_bv_rank_[level])(current_block)*r_ + child_number;
            } else {
                i -= current_length*big_blocks;
                --current_length;
                int child_number = big_blocks + i/current_length;
                i -= (child_number-big_blocks) * current_length;
                current_block = (*bt_bv_rank_[level])(current_block)*r_ + child_number;
            }
            ++level;
            r += (current_block%r_ == 0) ? 0 : (*bt_cumulated_leaf_ranks_[level-1])[current_block-1];
            max_length_level = (max_length_level % r_) ? max_length_level/r_ +1 : max_length_level/r_;
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            int f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
            int s_condition = (*bt_suffix_start_with_end_leaf_[level])[index];
            if (f_condition && !s_condition) ++r;
            if (!f_condition && s_condition) --r;
            current_block = encoded_offset/max_length_level;
            current_length = ((*bt_bv_block_length_[level])[current_block]) ? max_length_level : max_length_level-1;
            int offset = encoded_offset%max_length_level;
            if (!offset) {
                if (i >= current_length) {
                    r += (*bt_second_leaf_ranks_[level])[index];

                    ++current_block;
                    i -= current_length;
                    f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                    s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                    if (f_condition && !s_condition) --r;
                    if (!f_condition && s_condition) ++r;
                    current_length = ((*bt_bv_block_length_[level])[current_block]) ? max_length_level :
                                     max_length_level - 1;

                }
            } else {
                i += offset;
                r += (*bt_second_leaf_ranks_[level])[index];
                if (i >= current_length) {
                    ++current_block;
                    i -= current_length;
                    f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                    s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                    if (f_condition && !s_condition) --r;
                    if (!f_condition && s_condition) ++r;
                    current_length = ((*bt_bv_block_length_[level])[current_block]) ? max_length_level :
                                     max_length_level - 1;
                } else {
                    if (level != 0 )r -= ((current_block%r_ == 0) ?  ((*bt_cumulated_leaf_ranks_[level-1])[current_block]) :  (((*bt_cumulated_leaf_ranks_[level-1])[current_block]) - ((*bt_cumulated_leaf_ranks_[level-1])[current_block-1])));
                    else r -= ((current_block == 0) ?  ((*bt_first_level_cumulated_leaf_ranks_)[current_block]) :  (((*bt_first_level_cumulated_leaf_ranks_)[current_block]) - ((*bt_first_level_cumulated_leaf_ranks_)[current_block-1])));
                }
            }
        }
    }

    onesRank =  (*bt_bv_block_length_rank_[level])(current_block);
    zerosRank = current_block - onesRank;

    i  += (onesRank*max_length_level + zerosRank*(max_length_level-1));
    bool one_seen = (*bt_starts_with_end_leaf_[level])[current_block];
    for (int j = (onesRank*max_length_level + zerosRank*(max_length_level-1)); j <= i; ++j) {
        if ((*leaf_string_)[j] == mapping_[open_]) {
            one_seen = true;
        } else {
            if (one_seen) ++r;
            one_seen = false;
        }
    }

    return r;
}

int BPSCBlockTree::select_10(int k) {

    int current_block = (k-1)/max_length_first_level_;

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

    int onesRank = (*(bt_bv_block_length_rank_[0]))(current_block);
    int zerosRank = current_block - onesRank;
    int s = onesRank*max_length_first_level_ + zerosRank*(max_length_first_level_ - 1);
    k -= (*bt_first_level_prefix_leaf_ranks_)[current_block];


    int current_length = ((*(bt_bv_block_length_[0]))[current_block]) ? max_length_first_level_ : max_length_first_level_ - 1;
    int max_length_level = max_length_first_level_;
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
                int f = (*prefix_ranks[level+1-1])[m];
                if (f < k) {
                    if (end_child - child == 1) {
                        if ((*prefix_ranks[level+1-1])[m+1] < k) {
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
            while ( child < firstChild + r_-1 && k > (*bt_prefix_leaf_ranks_[level+1-1])[child+1]) {
                ++child;
            }

            k -= (*bt_prefix_leaf_ranks_[level+1-1])[child];

            int big_blocks = (current_length%r_ == 0) ? r_ : current_length%r_;
            current_length = (current_length%r_ == 0) ? current_length/r_ : current_length/r_ + 1;
            current_block = child;
            if (child-firstChild < big_blocks) {
                s += (child-firstChild)*current_length;
            } else {
                s += big_blocks*current_length + ((child-firstChild)-big_blocks) * (current_length-1);
                --current_length;
            }
            ++level;
            max_length_level = (max_length_level % r_) ? max_length_level/r_ +1 : max_length_level/r_;
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            int f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
            int s_condition = (*bt_suffix_start_with_end_leaf_[level])[index];
            if (f_condition && !s_condition) --k;
            if (!f_condition && s_condition) ++k;
            current_block = encoded_offset/max_length_level;
            current_length = ((*bt_bv_block_length_[level])[current_block]) ? max_length_level : max_length_level-1;

            int second_rank = (*bt_second_leaf_ranks_[level])[index];
            if (k > second_rank) {
                s += current_length - encoded_offset%max_length_level;
                ++current_block;
                f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                if (f_condition && !s_condition) ++k;
                if (!f_condition && s_condition) --k;
                k-= second_rank;
                if (k == 0) return s-1;
                current_length = ((*bt_bv_block_length_[level])[current_block]) ? max_length_level : max_length_level-1;
            } else {
                if (k == 0) return s-1;
                s -= encoded_offset%max_length_level;
                k += (*bt_first_leaf_ranks_[level])[index];
            }
        }
    }

    onesRank =  (*bt_bv_block_length_rank_[level])(current_block);
    zerosRank = current_block - onesRank;

    bool one_seen = (*bt_starts_with_end_leaf_[level])[current_block];
    if (one_seen && k == 1) return s-1;
    for (int j = (onesRank*max_length_level + zerosRank*(max_length_level-1)); ; ++j) {
        if ((*leaf_string_)[j] == mapping_[open_]) {
            one_seen = true;
        } else {
            if (one_seen) {
                --k;
            }
            one_seen = false;
        }
        if (!k) return s + j - (onesRank*max_length_level + zerosRank*(max_length_level-1))-1;
    }
    return -1;

}


int BPSCBlockTree::select_10_alternative(int k) {

    int current_block = (k-1)/max_length_first_level_;

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

    int onesRank = (*(bt_bv_block_length_rank_[0]))(current_block);
    int zerosRank = current_block - onesRank;
    int s = onesRank*max_length_first_level_ + zerosRank*(max_length_first_level_ - 1);
    k = k-(*bt_first_level_prefix_leaf_ranks_)[current_block];


    int current_length = ((*(bt_bv_block_length_[0]))[current_block]) ? max_length_first_level_ : max_length_first_level_ - 1;
    int max_length_level = max_length_first_level_;
    int level = 0;

    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            int child = firstChild;
            int r = (*bt_leaf_ranks_[level+1])[child];
            while ( child < firstChild + r_ && k > r) {
                ++child;
                r+= (*bt_leaf_ranks_[level+1])[child];
            }
            k -= r - (*bt_leaf_ranks_[level+1])[child];
            int big_blocks = (current_length%r_ == 0) ? r_ : current_length%r_;
            current_length = (current_length%r_ == 0) ? current_length/r_ : current_length/r_ + 1;
            current_block = child;
            if (child-firstChild < big_blocks) {
                s += (child-firstChild)*current_length;
            } else {
                s += big_blocks*current_length + ((child-firstChild)-big_blocks) * (current_length-1);
                --current_length;
            }
            ++level;
            max_length_level = (max_length_level % r_) ? max_length_level/r_ +1 : max_length_level/r_;
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            int f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
            int s_condition = (*bt_suffix_start_with_end_leaf_[level])[index];
            if (f_condition && !s_condition) --k;
            if (!f_condition && s_condition) ++k;
            current_block = encoded_offset/max_length_level;
            current_length = ((*bt_bv_block_length_[level])[current_block]) ? max_length_level : max_length_level-1;

            int offset = encoded_offset%max_length_level;
            int second_rank = (*bt_second_leaf_ranks_[level])[index];
            if (!offset) {
                if (k > second_rank) {
                    s += current_length;
                    k -= second_rank;
                    ++current_block;
                    f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                    s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                    if (f_condition && !s_condition) ++k;
                    if (!f_condition && s_condition) --k;
                    if (k == 0) return s-1;
                    current_length = ((*bt_bv_block_length_[level])[current_block]) ? max_length_level :
                                     max_length_level - 1;
                }
                if (k == 0) return s-1;
            } else {
                if (k > second_rank) {
                    k -= second_rank;
                    s += current_length - offset;
                    ++current_block;
                    f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                    s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                    if (f_condition && !s_condition) ++k;
                    if (!f_condition && s_condition) --k;
                    if (k == 0) return s-1;
                    current_length = ((*bt_bv_block_length_[level])[current_block]) ? max_length_level :
                                     max_length_level - 1;
                } else {
                    if (k == 0) return s-1;
                    s -= offset;
                    k -= second_rank;
                    k += (*bt_leaf_ranks_[level])[current_block];
                }
            }
        }
    }

    onesRank =  (*bt_bv_block_length_rank_[level])(current_block);
    zerosRank = current_block - onesRank;

    bool one_seen = (*bt_starts_with_end_leaf_[level])[current_block];
    if (one_seen && k == 1) return s-1;
    for (int j = (onesRank*max_length_level + zerosRank*(max_length_level-1)); ; ++j) {
        if ((*leaf_string_)[j] == mapping_[open_]) {
            one_seen = true;
        } else {
            if (one_seen) {
                --k;
            }
            one_seen = false;
        }
        if (!k) return s + j - (onesRank*max_length_level + zerosRank*(max_length_level-1))-1;
    }
    return -1;
}

int BPSCBlockTree::better_select_10(int k) {

    int current_block = (k-1)/max_length_first_level_;

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

    int onesRank = (*(bt_bv_block_length_rank_[0]))(current_block);
    int zerosRank = current_block - onesRank;
    int s = onesRank*max_length_first_level_ + zerosRank*(max_length_first_level_ - 1);
    k -= (current_block == 0) ? 0 : (*bt_first_level_cumulated_leaf_ranks_)[current_block-1];


    int current_length = ((*(bt_bv_block_length_[0]))[current_block]) ? max_length_first_level_ : max_length_first_level_ - 1;
    int max_length_level = max_length_first_level_;
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
            while ( child < firstChild + r_-1 && k > (*bt_cumulated_leaf_ranks_[level+1-1])[child]) {
                ++child;
            }

            k -= (child%r_ == 0) ? 0 : (*bt_cumulated_leaf_ranks_[level+1-1])[child-1];
            int big_blocks = (current_length%r_ == 0) ? r_ : current_length%r_;
            current_length = (current_length%r_ == 0) ? current_length/r_ : current_length/r_ + 1;
            current_block = child;
            if (child-firstChild < big_blocks) {
                s += (child-firstChild)*current_length;
            } else {
                s += big_blocks*current_length + ((child-firstChild)-big_blocks) * (current_length-1);
                --current_length;
            }
            ++level;
            max_length_level = (max_length_level % r_) ? max_length_level/r_ +1 : max_length_level/r_;
        } else { // Case BackBlock
            int index = current_block -  (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            int f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
            int s_condition = (*bt_suffix_start_with_end_leaf_[level])[index];
            if (f_condition && !s_condition) --k;
            if (!f_condition && s_condition) ++k;
            current_block = encoded_offset/max_length_level;
            current_length = ((*bt_bv_block_length_[level])[current_block]) ? max_length_level : max_length_level-1;

            int offset = encoded_offset%max_length_level;
            int second_rank = (*bt_second_leaf_ranks_[level])[index];
            if (!offset) {
                if (k > second_rank) {
                    s += current_length;
                    k -= second_rank;
                    ++current_block;
                    f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                    s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                    if (f_condition && !s_condition) ++k;
                    if (!f_condition && s_condition) --k;
                    if (k == 0) return s-1;
                    current_length = ((*bt_bv_block_length_[level])[current_block]) ? max_length_level :
                                     max_length_level - 1;
                }
                if (k == 0) return s-1;
            } else {
                if (k > second_rank) {
                    k -= second_rank;
                    s += current_length - offset;
                    ++current_block;
                    f_condition = (*bt_starts_with_end_leaf_[level])[current_block];
                    s_condition = (*bt_prefix_start_with_end_leaf_[level])[index];
                    if (f_condition && !s_condition) ++k;
                    if (!f_condition && s_condition) --k;
                    if (k == 0) return s-1;
                    current_length = ((*bt_bv_block_length_[level])[current_block]) ? max_length_level :
                                     max_length_level - 1;
                } else {
                    if (k == 0) return s-1;
                    s -= offset;
                    k -= second_rank;
                    if (level != 0) k += ((current_block%r_ == 0) ?  ((*bt_cumulated_leaf_ranks_[level-1])[current_block]) :  (((*bt_cumulated_leaf_ranks_[level-1])[current_block]) - ((*bt_cumulated_leaf_ranks_[level-1])[current_block-1])) );
                    else k += ((current_block == 0) ?  ((*bt_first_level_cumulated_leaf_ranks_)[current_block]) :  (((*bt_first_level_cumulated_leaf_ranks_)[current_block]) - ((*bt_first_level_cumulated_leaf_ranks_)[current_block-1])));
                }
            }
        }
    }

    onesRank =  (*bt_bv_block_length_rank_[level])(current_block);
    zerosRank = current_block - onesRank;

    bool one_seen = (*bt_starts_with_end_leaf_[level])[current_block];
    if (one_seen && k == 1) return s-1;
    for (int j = (onesRank*max_length_level + zerosRank*(max_length_level-1)); ; ++j) {
        if ((*leaf_string_)[j] == mapping_[open_]) {
            one_seen = true;
        } else {
            if (one_seen) {
                --k;
            }
            one_seen = false;
        }
        if (!k) return s + j - (onesRank*max_length_level + zerosRank*(max_length_level-1))-1;
    }
    return -1;
}


void BPSCBlockTree::print_statistics() {
    CBlockTree::print_statistics();

    int bt_first_level_prefix_leaf_ranks_size = sdsl::size_in_bytes(*bt_first_level_prefix_leaf_ranks_);
    int bt_first_level_cumulated_leaf_ranks_size = sdsl::size_in_bytes(*bt_first_level_cumulated_leaf_ranks_);
    int c_bt_first_level_prefix_leaf_ranks_size = sdsl::size_in_bytes(*c_bt_first_level_prefix_leaf_ranks_);
    int c_bt_first_level_cumulated_leaf_ranks_size = sdsl::size_in_bytes(*c_bt_first_level_cumulated_leaf_ranks_);


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
}


