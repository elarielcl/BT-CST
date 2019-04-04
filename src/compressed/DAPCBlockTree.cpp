//
// Created by sking32 on 7/31/18.
//

#include <compressed/DAPCBlockTree.h>
#include <iostream>
#include <unordered_set>
#include <algorithm>

DAPCBlockTree::DAPCBlockTree(PBlockTree * bt) : r_(bt->r_), leaf_length_(bt->leaf_length_), winput_(bt->winput_) {
    std::vector<PBlock*> first_level = {bt->root_block_};
    bool is_first_level = false;
    while (!is_first_level) {
        for (PBlock* b: first_level) {
            is_first_level = is_first_level || b->is_leaf();
        }
        if (is_first_level) break;
        first_level = bt->next_level(first_level);
    }

    int64_t first_level_prefix_sum_ = 0;
    sdsl::int_vector<>* first_level_prefix_sum = new sdsl::int_vector<>(first_level.size());
    sdsl::int_vector<>* first_level_cumulated_sum = new sdsl::int_vector<>(first_level.size());
    sdsl::int_vector<>* first_level_sum = new sdsl::int_vector<>(first_level.size());

    for (int i = 0; i < first_level.size(); ++i) {
        (*first_level_prefix_sum)[i] = encoded_sum(first_level_prefix_sum_);

        (*first_level_sum)[i] = encoded_sum(first_level[i]->sum_);
        first_level_prefix_sum_ += first_level[i]->sum_;

        (*first_level_cumulated_sum)[i] = encoded_sum(first_level_prefix_sum_);
    }

    sdsl::util::bit_compress(*first_level_prefix_sum);
    bt_first_level_prefix_sum_ = first_level_prefix_sum;
    c_bt_first_level_prefix_sum_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*first_level_prefix_sum);

    sdsl::util::bit_compress(*first_level_cumulated_sum);
    bt_first_level_cumulated_sum_ = first_level_cumulated_sum;
    c_bt_first_level_cumulated_sum_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*first_level_cumulated_sum);

    sdsl::util::bit_compress(*first_level_sum);
    bt_sum_.push_back(first_level_sum);
    c_bt_sum_.push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*first_level_sum));


    first_level_length_ = first_level[0]->length();
    number_of_blocks_first_level_ = first_level.size();
    number_of_levels_ = 0;

    std::vector<PBlock*> current_level = first_level;
    std::vector<PBlock*> next_level = bt->next_level(first_level);

    while (next_level.size() != 0) {
        sdsl::bit_vector* current_level_bv = new sdsl::bit_vector(current_level.size() ,0);

        sdsl::int_vector<>* next_level_sum = new sdsl::int_vector<>(next_level.size());
        sdsl::int_vector<>* next_level_prefix_sum = new sdsl::int_vector<>(next_level.size());
        sdsl::int_vector<>* next_level_cumulated_sum = new sdsl::int_vector<>(next_level.size());

        int number_of_leaves = 0;
        int current_length = current_level.front()->length();
        for (int i = 0; i < current_level.size(); ++i) {
            current_level[i]->level_index_ = i;

            if (current_level[i]->is_leaf()) {
                (*current_level_bv)[i] = 0;
                ++number_of_leaves;
            }
            else {
                (*current_level_bv)[i] = 1;
            }
        }

        for (int i = 0; i < next_level.size(); ++i) {
            (*next_level_sum)[i] = encoded_sum(next_level[i]->sum_);
            (*next_level_prefix_sum)[i] = encoded_sum(next_level[i]->prefix_sum_);
            (*next_level_cumulated_sum)[i] = encoded_sum(next_level[i]->cumulated_sum_);
        }



        sdsl::int_vector<>* current_level_offsets = new sdsl::int_vector<>(number_of_leaves);

        sdsl::int_vector<>* current_level_first_sum;
        sdsl::int_vector<>* current_level_second_sum;

        current_level_first_sum = new sdsl::int_vector<>(number_of_leaves);
        current_level_second_sum = new sdsl::int_vector<>(number_of_leaves);


        int j = 0;
        for (int i = 0; i < current_level.size(); ++i) {
            if (!(*current_level_bv)[i]) {
                (*current_level_first_sum)[j] = encoded_sum(current_level[i]->first_sum_);
                (*current_level_second_sum)[j] = encoded_sum(current_level[i]->second_sum_);

                (*current_level_offsets)[j++] = current_level[i]->first_block_->level_index_ * current_length + current_level[i]->offset_;
            }
        }

        sdsl::util::bit_compress(*current_level_offsets);
        bt_offsets_.push_back(current_level_offsets);
        c_bt_offsets_.push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*current_level_offsets));

        sdsl::util::bit_compress(*next_level_sum);
        bt_sum_.push_back(next_level_sum);
        c_bt_sum_.push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*next_level_sum));

        sdsl::util::bit_compress(*next_level_prefix_sum);
        bt_prefix_sum_.push_back(next_level_prefix_sum);
        c_bt_prefix_sum_.push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*next_level_prefix_sum));

        sdsl::util::bit_compress(*next_level_cumulated_sum);
        bt_cumulated_sum_.push_back(next_level_cumulated_sum);
        c_bt_cumulated_sum_.push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*next_level_cumulated_sum));

        sdsl::util::bit_compress(*current_level_first_sum);
        bt_first_sum_.push_back(current_level_first_sum);
        c_bt_first_sum_.push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*current_level_first_sum));

        sdsl::util::bit_compress(*current_level_second_sum);
        bt_second_sum_.push_back(current_level_second_sum);
        c_bt_second_sum_.push_back(new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*current_level_second_sum));





        bt_bv_.push_back(current_level_bv);
        sdsl::rank_support_v<1>* current_level_bv_rank = new sdsl::rank_support_v<1>(current_level_bv);
        bt_bv_rank_.push_back(current_level_bv_rank);

        c_bt_bv_.push_back(new sdsl::rrr_vector<63>(*bt_bv_.back()));
        c_bt_bv_rank_.push_back(new sdsl::rrr_vector<63>::rank_1_type(c_bt_bv_.back()));

        current_level = next_level;
        next_level = bt->next_level(current_level);
        ++number_of_levels_;
    }

    ++number_of_levels_;

    std::vector<PBlock*> last_level = current_level;

    std::basic_string<int64_t> leaf_string;
    std::basic_string<int64_t> sum_leaf_string;
    for (PBlock* b: last_level) {
        std::basic_string<int64_t> wstring = b->represented_wstring();
        leaf_string += wstring;
        int64_t sum = 0;
        for (int64_t s : wstring) {
            sum += s;
            sum_leaf_string += sum;
        }
    }

    /*
    std::unordered_set<char> alphabet;
    for (char c : leaf_string) {
        alphabet.insert(c);
    }
    alphabet_ = new sdsl::int_vector<>(alphabet.size());
    int counter = 0;
    for (char c : alphabet){
        mapping_[c] = counter;
        (*alphabet_)[counter++] = c;
    }
    sdsl::util::bit_compress(*alphabet_);
    */

    leaf_string_ = new sdsl::int_vector<>(leaf_string.size());
    sum_leaf_string_ = new sdsl::int_vector<>(sum_leaf_string.size());

    for (int i = 0; i<leaf_string_->size(); ++i) {
        (*leaf_string_)[i] = encoded_sum(leaf_string[i]);
    }

    for (int i = 0; i<sum_leaf_string_->size(); ++i) {
        (*sum_leaf_string_)[i] = encoded_sum(sum_leaf_string[i]);
    }


    // ESTO DEJE IMPLEMENTADO PARA LUEGO TESTEAR EN TURING :)

    std::set<uint64_t> leaf_string_set;
    for (int i = 0; i<leaf_string_->size(); ++i) {
        leaf_string_set.insert((*leaf_string_)[i]);
    }
    std::vector<uint64_t> leaf_string_array;
    for (int64_t e : leaf_string_set) {
        leaf_string_array.push_back(e);
    }

    std::sort(leaf_string_array.begin(), leaf_string_array.end());

    leaf_string_map_ = new sdsl::sd_vector<>(leaf_string_array.begin(), leaf_string_array.end());
    leaf_string_map_m_rank_support_ = new sdsl::rank_support_sd<1>();
    leaf_string_map_m_select_support_ = new sdsl::select_support_sd<1>();
    sdsl::util::init_support(*leaf_string_map_m_rank_support_, leaf_string_map_);
    sdsl::util::init_support(*leaf_string_map_m_select_support_, leaf_string_map_);

    mapped_leaf_string_ = new sdsl::int_vector<>(leaf_string_->size());
    for (int i = 0; i<leaf_string_->size(); ++i) {
        (*mapped_leaf_string_)[i] = (*leaf_string_map_m_rank_support_)((*leaf_string_)[i]+1);
    }
    sdsl::util::bit_compress(*mapped_leaf_string_);
    c_mapped_leaf_string_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*mapped_leaf_string_);

    // ESTO DEJE IMPLEMENTADO PARA LUEGO TESTEAR EN TURING :)

    // ESTO DEJE IMPLEMENTADO PARA LUEGO TESTEAR EN TURING :)

    std::set<uint64_t> sum_leaf_string_set;
    for (int i = 0; i<sum_leaf_string_->size(); ++i) {
        sum_leaf_string_set.insert((*sum_leaf_string_)[i]);
    }
    std::vector<uint64_t> sum_leaf_string_array;
    for (int64_t e : sum_leaf_string_set) {
        sum_leaf_string_array.push_back(e);
    }

    std::sort(sum_leaf_string_array.begin(), sum_leaf_string_array.end());

    sum_leaf_string_map_ = new sdsl::sd_vector<>(sum_leaf_string_array.begin(), sum_leaf_string_array.end());
    sum_leaf_string_map_m_rank_support_ = new sdsl::rank_support_sd<1>();
    sum_leaf_string_map_m_select_support_ = new sdsl::select_support_sd<1>();
    sdsl::util::init_support(*sum_leaf_string_map_m_rank_support_, sum_leaf_string_map_);
    sdsl::util::init_support(*sum_leaf_string_map_m_select_support_, sum_leaf_string_map_);

    sum_mapped_leaf_string_ = new sdsl::int_vector<>(sum_leaf_string_->size());
    for (int i = 0; i<sum_leaf_string_->size(); ++i) {
        (*sum_mapped_leaf_string_)[i] = (*sum_leaf_string_map_m_rank_support_)((*sum_leaf_string_)[i]+1);
    }
    sdsl::util::bit_compress(*sum_mapped_leaf_string_);
    c_sum_mapped_leaf_string_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*sum_mapped_leaf_string_);
    // ESTO DEJE IMPLEMENTADO PARA LUEGO TESTEAR EN TURING :)

    /*
    leaf_bv_ = new sdsl::bit_vector(leaf_string_->size());
    for (int i = 0; i<leaf_string_->size(); ++i) {
        if (mapping_[leaf_string[i]]) (*leaf_bv_)[i] = 1;
        else (*leaf_bv_)[i] = 0;
    }
    leaf_bv_rank_ = new sdsl::rank_support_v<1>(leaf_bv_);
    c_leaf_bv_ = new sdsl::hyb_vector<>(*leaf_bv_);
    c_leaf_bv_rank_ = new sdsl::hyb_vector<>::rank_1_type(c_leaf_bv_);
    */

    sdsl::util::bit_compress(*leaf_string_);
    c_leaf_string_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*leaf_string_);

    sdsl::util::bit_compress(*sum_leaf_string_);
    c_sum_leaf_string_ = new sdsl::dac_vector_dp<sdsl::rrr_vector<>>(*sum_leaf_string_);

    /*
    leaf_string_wt_ = new sdsl::wt_blcd<>();
    construct_im(*leaf_string_wt_, leaf_string, 1);
     */
}

DAPCBlockTree::~DAPCBlockTree() {

    for (sdsl::bit_vector* bv : bt_bv_)
        delete bv;
    for (sdsl::rank_support_v<1>* rank : bt_bv_rank_)
        delete rank;


    for (sdsl::int_vector<>* offsets : bt_offsets_)
        delete offsets;

    delete leaf_string_;
    delete sum_leaf_string_;
    delete mapped_leaf_string_;
    delete sum_mapped_leaf_string_;

    //delete alphabet_;
    //delete leaf_bv_;
    //delete leaf_bv_rank_;
    //delete c_leaf_bv_;
    //delete c_leaf_bv_rank_;

    delete bt_first_level_prefix_sum_;
    delete bt_first_level_cumulated_sum_;

    for (sdsl::int_vector<>* sum: bt_prefix_sum_) {
        delete sum;
    }

    for (sdsl::int_vector<>* sum: bt_cumulated_sum_) {
        delete sum;
    }

    for (sdsl::int_vector<>* sum : bt_sum_) {
        delete sum;
    }

    for (sdsl::int_vector<>* sum : bt_first_sum_) {
        delete sum;
    }
    for (sdsl::int_vector<>* sum : bt_second_sum_) {
        delete sum;
    }


    // Compressed
    for (sdsl::rrr_vector<63>* bv : c_bt_bv_)
        delete bv;
    for (sdsl::rrr_vector<63>::rank_1_type* rank : c_bt_bv_rank_)
        delete rank;

    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* offsets : c_bt_offsets_)
        delete offsets;

    delete c_leaf_string_;
    delete c_sum_leaf_string_;
    delete c_mapped_leaf_string_;
    delete c_sum_mapped_leaf_string_;

    delete c_bt_first_level_prefix_sum_;
    delete c_bt_first_level_cumulated_sum_;

    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* sum : c_bt_prefix_sum_) {
        delete sum;
    }

    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* sum : c_bt_cumulated_sum_) {
        delete sum;
    }

    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* sum : c_bt_sum_) {
        delete sum;
    }

    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* sum : c_bt_first_sum_) {
        delete sum;
    }

    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>*sum : c_bt_second_sum_) {
        delete sum;
    }

//    delete leaf_string_wt_;
}

int64_t DAPCBlockTree::access(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i -= current_block*first_level_length_;
    int level = 0;
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;
            current_block = (*bt_bv_rank_[level])(current_block)*r_ + child_number;
            ++level;
        } else { // Case BackBlock
            int encoded_offset = (*bt_offsets_[level])[current_block-(*bt_bv_rank_[level])(current_block+1)];
            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            if (i >= current_length) {
                ++current_block;
                i -= current_length;
            }
        }
    }
    return (*leaf_string_)[i+current_block*current_length];
}



int64_t DAPCBlockTree::differential_access(int i) {
    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i -= current_block*current_length;
    int level = 0;
    int64_t r = decoded_sum((*bt_first_level_prefix_sum_)[current_block]);

    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;
            current_block = (*bt_bv_rank_[level])(current_block)*r_ + child_number;
            ++level;
            r += decoded_sum((*bt_prefix_sum_[level-1])[current_block]);
        } else { // Case BackBlock
            int index = current_block-(*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            if (i >= current_length) {
                r += decoded_sum((*bt_second_sum_[level])[index]);
                ++current_block;
                i -= current_length;
            } else {
                r -= decoded_sum((*bt_first_sum_[level])[index]);
            }
        }
    }

    i  += current_block*current_length;

    for (int j = current_block*current_length; j <= i; ++j) {
        r += decoded_sum((*leaf_string_)[j]);
    }

    return r;
    /*
   int qq = (onesRank*max_length_level + zerosRank*(max_length_level-1));
   int rr = (onesRank*max_length_level + zerosRank*(max_length_level-1)) + i;

   return r + leaf_string_wt_->rank(rr+1, c) - leaf_string_wt_->rank(qq, c);
    */
}


int64_t DAPCBlockTree::sdifferential_access(int i) {
    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i -= current_block*current_length;
    int level = 0;
    int64_t r = decoded_sum((*bt_first_level_prefix_sum_)[current_block]);

    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;
            current_block = (*bt_bv_rank_[level])(current_block)*r_ + child_number;
            ++level;
            r += decoded_sum((*bt_prefix_sum_[level-1])[current_block]);
        } else { // Case BackBlock
            int index = current_block-(*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            if (i >= current_length) {
                r += decoded_sum((*bt_second_sum_[level])[index]);
                ++current_block;
                i -= current_length;
            } else {
                r -= decoded_sum((*bt_first_sum_[level])[index]);
            }
        }
    }

    i  += current_block*current_length;
    r += decoded_sum((*sum_leaf_string_)[i]);

    return r;
    /*
   int qq = (onesRank*max_length_level + zerosRank*(max_length_level-1));
   int rr = (onesRank*max_length_level + zerosRank*(max_length_level-1)) + i;

   return r + leaf_string_wt_->rank(rr+1, c) - leaf_string_wt_->rank(qq, c);
    */
}

int64_t DAPCBlockTree::differential_access_alternative(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i = i-current_block*current_length;
    int level = 0;

    int64_t r = decoded_sum((*bt_first_level_prefix_sum_)[current_block]);
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;

            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            for (int child = firstChild; child < firstChild + child_number; ++child)
                r += decoded_sum((*bt_sum_[level+1])[child]);
            current_block = firstChild + child_number;
            ++level;
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            r += decoded_sum((*bt_second_sum_[level])[index]);
            if (i >= current_length) {
                ++current_block;
                i -= current_length;
            } else {
                r -= decoded_sum((*bt_sum_[level])[current_block]);
            }

        }
    }

    i  += current_block*current_length;
    for (int j = current_block*current_length; j <= i; ++j) {
        r += decoded_sum((*leaf_string_)[j]);
    }

    return r;
}


int64_t DAPCBlockTree::mapped_differential_access_alternative(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i = i-current_block*current_length;
    int level = 0;

    int64_t r = decoded_sum((*bt_first_level_prefix_sum_)[current_block]);
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;

            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            for (int child = firstChild; child < firstChild + child_number; ++child)
                r += decoded_sum((*bt_sum_[level+1])[child]);
            current_block = firstChild + child_number;
            ++level;
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            r += decoded_sum((*bt_second_sum_[level])[index]);
            if (i >= current_length) {
                ++current_block;
                i -= current_length;
            } else {
                r -= decoded_sum((*bt_sum_[level])[current_block]);
            }

        }
    }

    i  += current_block*current_length;
    for (int j = current_block*current_length; j <= i; ++j) {
        r += decoded_sum((*leaf_string_map_m_select_support_)((*mapped_leaf_string_)[j]));
    }

    return r;
}


int64_t DAPCBlockTree::sdifferential_access_alternative(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i = i-current_block*current_length;
    int level = 0;

    int64_t r = decoded_sum((*bt_first_level_prefix_sum_)[current_block]);
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;

            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            for (int child = firstChild; child < firstChild + child_number; ++child)
                r += decoded_sum((*bt_sum_[level+1])[child]);
            current_block = firstChild + child_number;
            ++level;
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            r += decoded_sum((*bt_second_sum_[level])[index]);
            if (i >= current_length) {
                ++current_block;
                i -= current_length;
            } else {
                r -= decoded_sum((*bt_sum_[level])[current_block]);
            }

        }
    }

    i  += current_block*current_length;
    r += decoded_sum((*sum_leaf_string_)[i]);

    return r;
}


int64_t DAPCBlockTree::mapped_sdifferential_access_alternative(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i = i-current_block*current_length;
    int level = 0;

    int64_t r = decoded_sum((*bt_first_level_prefix_sum_)[current_block]);
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;

            int firstChild = (*bt_bv_rank_[level])(current_block)*r_;
            for (int child = firstChild; child < firstChild + child_number; ++child)
                r += decoded_sum((*bt_sum_[level+1])[child]);
            current_block = firstChild + child_number;
            ++level;
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            r += decoded_sum((*bt_second_sum_[level])[index]);
            if (i >= current_length) {
                ++current_block;
                i -= current_length;
            } else {
                r -= decoded_sum((*bt_sum_[level])[current_block]);
            }

        }
    }

    i  += current_block*current_length;
    r += decoded_sum((*sum_leaf_string_map_m_select_support_)((*sum_mapped_leaf_string_)[i]));

    return r;
}



int64_t DAPCBlockTree::better_differential_access(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;

    i -= current_block*current_length;
    int level = 0;

    int64_t r = (current_block == 0) ? 0 : decoded_sum((*bt_first_level_cumulated_sum_)[current_block-1]);
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;
            current_block = (*bt_bv_rank_[level])(current_block)*r_ + child_number;
            ++level;
            r += (current_block%r_ == 0) ? 0 : decoded_sum((*bt_cumulated_sum_[level-1])[current_block-1]);
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;

            i += encoded_offset%current_length;
            r += decoded_sum((*bt_second_sum_[level])[index]);
            if (i  >= current_length) {
                ++current_block;
                i -= current_length;
            } else {
                if (level != 0 )r -= (current_block%r_ == 0) ?  decoded_sum((*bt_cumulated_sum_[level-1])[current_block]) :  (decoded_sum((*bt_cumulated_sum_[level-1])[current_block]) - decoded_sum((*bt_cumulated_sum_[level-1])[current_block-1]));
                else r -= (current_block == 0) ?  decoded_sum((*bt_first_level_cumulated_sum_)[current_block]) : (decoded_sum((*bt_first_level_cumulated_sum_)[current_block])-decoded_sum((*bt_first_level_cumulated_sum_)[current_block-1]));
            }

        }
    }

    i  += current_block*current_length;
    for (int j = current_block*current_length; j <= i; ++j) {
        r += decoded_sum((*leaf_string_)[j]);
    }

    return r;
    /*
   int qq = (onesRank*max_length_level + zerosRank*(max_length_level-1));
   int rr = (onesRank*max_length_level + zerosRank*(max_length_level-1)) + i;

   return r + leaf_string_wt_->rank(rr+1, c) - leaf_string_wt_->rank(qq, c);
    */
}



int64_t DAPCBlockTree::sbetter_differential_access(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;

    i -= current_block*current_length;
    int level = 0;

    int64_t r = (current_block == 0) ? 0 : decoded_sum((*bt_first_level_cumulated_sum_)[current_block-1]);
    while (level < number_of_levels_-1) {
        if ((*bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;
            current_block = (*bt_bv_rank_[level])(current_block)*r_ + child_number;
            ++level;
            r += (current_block%r_ == 0) ? 0 : decoded_sum((*bt_cumulated_sum_[level-1])[current_block-1]);
        } else { // Case BackBlock
            int index = current_block - (*bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;

            i += encoded_offset%current_length;
            r += decoded_sum((*bt_second_sum_[level])[index]);
            if (i  >= current_length) {
                ++current_block;
                i -= current_length;
            } else {
                if (level != 0 )r -= (current_block%r_ == 0) ?  decoded_sum((*bt_cumulated_sum_[level-1])[current_block]) :  (decoded_sum((*bt_cumulated_sum_[level-1])[current_block]) - decoded_sum((*bt_cumulated_sum_[level-1])[current_block-1]));
                else r -= (current_block == 0) ?  decoded_sum((*bt_first_level_cumulated_sum_)[current_block]) : (decoded_sum((*bt_first_level_cumulated_sum_)[current_block])-decoded_sum((*bt_first_level_cumulated_sum_)[current_block-1]));
            }

        }
    }

    i  += current_block*current_length;
    r += decoded_sum((*sum_leaf_string_)[i]);

    return r;
    /*
   int qq = (onesRank*max_length_level + zerosRank*(max_length_level-1));
   int rr = (onesRank*max_length_level + zerosRank*(max_length_level-1)) + i;

   return r + leaf_string_wt_->rank(rr+1, c) - leaf_string_wt_->rank(qq, c);
    */
}


int64_t DAPCBlockTree::caccess(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i -= current_block*first_level_length_;
    int level = 0;
    while (level < number_of_levels_-1) {
        if ((*c_bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;
            current_block = (*c_bt_bv_rank_[level])(current_block)*r_ + child_number;
            ++level;
        } else { // Case BackBlock
            int encoded_offset = (*c_bt_offsets_[level])[current_block-(*c_bt_bv_rank_[level])(current_block+1)];
            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            if (i >= current_length) {
                ++current_block;
                i -= current_length;
            }
        }
    }
    return (*c_leaf_string_)[i+current_block*current_length];
}



int64_t DAPCBlockTree::cdifferential_access(int i) {
    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i -= current_block*current_length;
    int level = 0;
    int64_t r = decoded_sum((*c_bt_first_level_prefix_sum_)[current_block]);

    while (level < number_of_levels_-1) {
        if ((*c_bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;
            current_block = (*c_bt_bv_rank_[level])(current_block)*r_ + child_number;
            ++level;
            r += decoded_sum((*c_bt_prefix_sum_[level-1])[current_block]);
        } else { // Case BackBlock
            int index = current_block-(*c_bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*c_bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            if (i >= current_length) {
                r += decoded_sum((*c_bt_second_sum_[level])[index]);
                ++current_block;
                i -= current_length;
            } else {
                r -= decoded_sum((*c_bt_first_sum_[level])[index]);
            }
        }
    }

    i  += current_block*current_length;

    for (int j = current_block*current_length; j <= i; ++j) {
        r += decoded_sum((*c_leaf_string_)[j]);
    }

    return r;
    /*
   int qq = (onesRank*max_length_level + zerosRank*(max_length_level-1));
   int rr = (onesRank*max_length_level + zerosRank*(max_length_level-1)) + i;

   return r + leaf_string_wt_->rank(rr+1, c) - leaf_string_wt_->rank(qq, c);
    */
}


int64_t DAPCBlockTree::csdifferential_access(int i) {
    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i -= current_block*current_length;
    int level = 0;
    int64_t r = decoded_sum((*c_bt_first_level_prefix_sum_)[current_block]);

    while (level < number_of_levels_-1) {
        if ((*c_bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;
            current_block = (*c_bt_bv_rank_[level])(current_block)*r_ + child_number;
            ++level;
            r += decoded_sum((*c_bt_prefix_sum_[level-1])[current_block]);
        } else { // Case BackBlock
            int index = current_block-(*c_bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*c_bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            if (i >= current_length) {
                r += decoded_sum((*c_bt_second_sum_[level])[index]);
                ++current_block;
                i -= current_length;
            } else {
                r -= decoded_sum((*c_bt_first_sum_[level])[index]);
            }
        }
    }

    i  += current_block*current_length;
    r += decoded_sum((*c_sum_leaf_string_)[i]);

    return r;
    /*
   int qq = (onesRank*max_length_level + zerosRank*(max_length_level-1));
   int rr = (onesRank*max_length_level + zerosRank*(max_length_level-1)) + i;

   return r + leaf_string_wt_->rank(rr+1, c) - leaf_string_wt_->rank(qq, c);
    */
}

int64_t DAPCBlockTree::cdifferential_access_alternative(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i = i-current_block*current_length;
    int level = 0;

    int64_t r = decoded_sum((*c_bt_first_level_prefix_sum_)[current_block]);
    while (level < number_of_levels_-1) {
        if ((*c_bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;

            int firstChild = (*c_bt_bv_rank_[level])(current_block)*r_;
            for (int child = firstChild; child < firstChild + child_number; ++child)
                r += decoded_sum((*c_bt_sum_[level+1])[child]);
            current_block = firstChild + child_number;
            ++level;
        } else { // Case BackBlock
            int index = current_block - (*c_bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*c_bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            r += decoded_sum((*c_bt_second_sum_[level])[index]);
            if (i >= current_length) {
                ++current_block;
                i -= current_length;
            } else {
                r -= decoded_sum((*c_bt_sum_[level])[current_block]);
            }

        }
    }

    i  += current_block*current_length;
    for (int j = current_block*current_length; j <= i; ++j) {
        r += decoded_sum((*c_leaf_string_)[j]);
    }

    return r;
}




int64_t DAPCBlockTree::mapped_cdifferential_access_alternative(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i = i-current_block*current_length;
    int level = 0;

    int64_t r = decoded_sum((*c_bt_first_level_prefix_sum_)[current_block]);
    while (level < number_of_levels_-1) {
        if ((*c_bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;

            int firstChild = (*c_bt_bv_rank_[level])(current_block)*r_;
            for (int child = firstChild; child < firstChild + child_number; ++child)
                r += decoded_sum((*c_bt_sum_[level+1])[child]);
            current_block = firstChild + child_number;
            ++level;
        } else { // Case BackBlock
            int index = current_block - (*c_bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*c_bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            r += decoded_sum((*c_bt_second_sum_[level])[index]);
            if (i >= current_length) {
                ++current_block;
                i -= current_length;
            } else {
                r -= decoded_sum((*c_bt_sum_[level])[current_block]);
            }

        }
    }

    i  += current_block*current_length;
    for (int j = current_block*current_length; j <= i; ++j) {
        r += decoded_sum((*leaf_string_map_m_select_support_)((*c_mapped_leaf_string_)[j]));
    }

    return r;
}


int64_t DAPCBlockTree::csdifferential_access_alternative(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i = i-current_block*current_length;
    int level = 0;

    int64_t r = decoded_sum((*c_bt_first_level_prefix_sum_)[current_block]);
    while (level < number_of_levels_-1) {
        if ((*c_bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;

            int firstChild = (*c_bt_bv_rank_[level])(current_block)*r_;
            for (int child = firstChild; child < firstChild + child_number; ++child)
                r += decoded_sum((*c_bt_sum_[level+1])[child]);
            current_block = firstChild + child_number;
            ++level;
        } else { // Case BackBlock
            int index = current_block - (*c_bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*c_bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            r += decoded_sum((*c_bt_second_sum_[level])[index]);
            if (i >= current_length) {
                ++current_block;
                i -= current_length;
            } else {
                r -= decoded_sum((*c_bt_sum_[level])[current_block]);
            }

        }
    }

    i  += current_block*current_length;
    r += decoded_sum((*c_sum_leaf_string_)[i]);

    return r;
}


int64_t DAPCBlockTree::mapped_csdifferential_access_alternative(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;
    i = i-current_block*current_length;
    int level = 0;

    int64_t r = decoded_sum((*c_bt_first_level_prefix_sum_)[current_block]);
    while (level < number_of_levels_-1) {
        if ((*c_bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;

            int firstChild = (*c_bt_bv_rank_[level])(current_block)*r_;
            for (int child = firstChild; child < firstChild + child_number; ++child)
                r += decoded_sum((*c_bt_sum_[level+1])[child]);
            current_block = firstChild + child_number;
            ++level;
        } else { // Case BackBlock
            int index = current_block - (*c_bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*c_bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;
            i += encoded_offset%current_length;
            r += decoded_sum((*c_bt_second_sum_[level])[index]);
            if (i >= current_length) {
                ++current_block;
                i -= current_length;
            } else {
                r -= decoded_sum((*c_bt_sum_[level])[current_block]);
            }

        }
    }

    i  += current_block*current_length;
    r += decoded_sum((*sum_leaf_string_map_m_select_support_)((*c_sum_mapped_leaf_string_)[i]));

    return r;
}


int64_t DAPCBlockTree::cbetter_differential_access(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;

    i -= current_block*current_length;
    int level = 0;

    int64_t r = (current_block == 0) ? 0 : decoded_sum((*c_bt_first_level_cumulated_sum_)[current_block-1]);
    while (level < number_of_levels_-1) {
        if ((*c_bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;
            current_block = (*c_bt_bv_rank_[level])(current_block)*r_ + child_number;
            ++level;
            r += (current_block%r_ == 0) ? 0 : decoded_sum((*c_bt_cumulated_sum_[level-1])[current_block-1]);
        } else { // Case BackBlock
            int index = current_block - (*c_bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*c_bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;

            i += encoded_offset%current_length;
            r += decoded_sum((*c_bt_second_sum_[level])[index]);
            if (i  >= current_length) {
                ++current_block;
                i -= current_length;
            } else {
                if (level != 0 )r -= (current_block%r_ == 0) ?  decoded_sum((*c_bt_cumulated_sum_[level-1])[current_block]) :  (decoded_sum((*c_bt_cumulated_sum_[level-1])[current_block]) - decoded_sum((*c_bt_cumulated_sum_[level-1])[current_block-1]));
                else r -= (current_block == 0) ?  decoded_sum((*c_bt_first_level_cumulated_sum_)[current_block]) : (decoded_sum((*c_bt_first_level_cumulated_sum_)[current_block])-decoded_sum((*c_bt_first_level_cumulated_sum_)[current_block-1]));
            }

        }
    }

    i  += current_block*current_length;
    for (int j = current_block*current_length; j <= i; ++j) {
        r += decoded_sum((*c_leaf_string_)[j]);
    }

    return r;
    /*
   int qq = (onesRank*max_length_level + zerosRank*(max_length_level-1));
   int rr = (onesRank*max_length_level + zerosRank*(max_length_level-1)) + i;

   return r + leaf_string_wt_->rank(rr+1, c) - leaf_string_wt_->rank(qq, c);
    */
}



int64_t DAPCBlockTree::csbetter_differential_access(int i) {

    int current_block = i/first_level_length_;
    int current_length = first_level_length_;

    i -= current_block*current_length;
    int level = 0;

    int64_t r = (current_block == 0) ? 0 : decoded_sum((*c_bt_first_level_cumulated_sum_)[current_block-1]);
    while (level < number_of_levels_-1) {
        if ((*c_bt_bv_[level])[current_block]) { // Case InternalBlock
            current_length /= r_;
            int child_number = i/current_length;
            i -= child_number*current_length;
            current_block = (*c_bt_bv_rank_[level])(current_block)*r_ + child_number;
            ++level;
            r += (current_block%r_ == 0) ? 0 : decoded_sum((*c_bt_cumulated_sum_[level-1])[current_block-1]);
        } else { // Case BackBlock
            int index = current_block - (*c_bt_bv_rank_[level])(current_block+1);
            int encoded_offset = (*c_bt_offsets_[level])[index];
            current_block = encoded_offset/current_length;

            i += encoded_offset%current_length;
            r += decoded_sum((*c_bt_second_sum_[level])[index]);
            if (i  >= current_length) {
                ++current_block;
                i -= current_length;
            } else {
                if (level != 0 )r -= (current_block%r_ == 0) ?  decoded_sum((*c_bt_cumulated_sum_[level-1])[current_block]) :  (decoded_sum((*c_bt_cumulated_sum_[level-1])[current_block]) - decoded_sum((*c_bt_cumulated_sum_[level-1])[current_block-1]));
                else r -= (current_block == 0) ?  decoded_sum((*c_bt_first_level_cumulated_sum_)[current_block]) : (decoded_sum((*c_bt_first_level_cumulated_sum_)[current_block])-decoded_sum((*c_bt_first_level_cumulated_sum_)[current_block-1]));
            }

        }
    }

    i  += current_block*current_length;
    r += decoded_sum((*c_sum_leaf_string_)[i]);

    return r;
    /*
   int qq = (onesRank*max_length_level + zerosRank*(max_length_level-1));
   int rr = (onesRank*max_length_level + zerosRank*(max_length_level-1)) + i;

   return r + leaf_string_wt_->rank(rr+1, c) - leaf_string_wt_->rank(qq, c);
    */
}




void DAPCBlockTree::print_statistics() {
    int leaf_string_size = sdsl::size_in_bytes(*leaf_string_);
    int c_leaf_string_size = sdsl::size_in_bytes(*c_leaf_string_);
    int sum_leaf_string_size = sdsl::size_in_bytes(*sum_leaf_string_);
    int c_sum_leaf_string_size = sdsl::size_in_bytes(*c_sum_leaf_string_);


    int mapped_leaf_string_size = sdsl::size_in_bytes(*mapped_leaf_string_);
    int c_mapped_leaf_string_size = sdsl::size_in_bytes(*c_mapped_leaf_string_);
    int sum_mapped_leaf_string_size = sdsl::size_in_bytes(*sum_mapped_leaf_string_);
    int c_sum_mapped_leaf_string_size = sdsl::size_in_bytes(*c_sum_mapped_leaf_string_);

    int map_size = sdsl::size_in_bytes(*leaf_string_map_);
    int map_sum_size = sdsl::size_in_bytes(*sum_leaf_string_map_);
//    int wt_leaf_string_size = sdsl::size_in_bytes(*leaf_string_wt_);

/*
    int alphabet_size = sdsl::size_in_bytes(*alphabet_);
    int mapping_size = sizeof(int) * 256;

    int leaf_bv_size = sdsl::size_in_bytes(*leaf_bv_);
    int leaf_bv_rank_size = sdsl::size_in_bytes(*leaf_bv_rank_);
    int c_leaf_bv_size = sdsl::size_in_bytes(*c_leaf_bv_);
    int c_leaf_bv_rank_size = sdsl::size_in_bytes(*c_leaf_bv_rank_);
*/

    int bt_bv_size = sizeof(void*);
    for (sdsl::bit_vector* bv : bt_bv_) {
        bt_bv_size += sdsl::size_in_bytes(*bv);
    }
    int c_bt_bv_size = sizeof(void*);
    for (sdsl::rrr_vector<63>* bv : c_bt_bv_) {
        c_bt_bv_size += sdsl::size_in_bytes(*bv);
    }


    int bt_bv_rank_size = sizeof(void*);
    for (sdsl::rank_support_v<1>* bvr : bt_bv_rank_) {
        bt_bv_rank_size += sdsl::size_in_bytes(*bvr);
    }
    int c_bt_bv_rank_size = sizeof(void*);
    for (sdsl::rrr_vector<63>::rank_1_type* bvr : c_bt_bv_rank_) {
        c_bt_bv_rank_size += sdsl::size_in_bytes(*bvr);
    }


    int bt_offsets_size = sizeof(void*);
    for (sdsl::int_vector<>* offsets: bt_offsets_) {
        bt_offsets_size += sdsl::size_in_bytes(*offsets);
    }
    int c_bt_offsets_size = sizeof(void*);
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* offsets: c_bt_offsets_) {
        c_bt_offsets_size += sdsl::size_in_bytes(*offsets);
    }



    int bt_sum_total_size = (bt_sum_.size()+1) * sizeof(void*);
    int bt_sum_first_level_size = 0;
    int bt_last_level_sum_total_size = 0;

    bt_last_level_sum_total_size += sdsl::size_in_bytes(*(bt_sum_.back()));
    bt_sum_first_level_size += sdsl::size_in_bytes(*(bt_sum_[0]));
    int size = 0;
    for (sdsl::int_vector<>* sum: bt_sum_) {
        size += sdsl::size_in_bytes(*sum);
    }
    bt_sum_total_size += size;

    int c_bt_sum_total_size = (c_bt_sum_.size()+1) * sizeof(void*);
    int c_bt_sum_first_level_size = 0;
    int c_bt_last_level_sum_total_size = 0;

    c_bt_last_level_sum_total_size += sdsl::size_in_bytes(*(c_bt_sum_.back()));
    c_bt_sum_first_level_size += sdsl::size_in_bytes(*(c_bt_sum_[0]));
    size = 0;
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* sum: c_bt_sum_) {
        size += sdsl::size_in_bytes(*sum);
    }
    c_bt_sum_total_size += size;



    int bt_prefix_sum_total_size = (bt_prefix_sum_.size()+1) * sizeof(void*);
    int bt_prefix_sum_first_level_size = 0;
    bt_prefix_sum_first_level_size += sdsl::size_in_bytes(*bt_first_level_prefix_sum_);
    int bt_last_level_prefix_sum_total_size = 0;

    bt_last_level_prefix_sum_total_size += sdsl::size_in_bytes(*(bt_prefix_sum_.back()));
    size = 0;
    for (sdsl::int_vector<>* sum: bt_prefix_sum_) {
        size += sdsl::size_in_bytes(*sum);
    }
    bt_prefix_sum_total_size += size;

    int c_bt_prefix_sum_total_size = (c_bt_prefix_sum_.size()+1) * sizeof(void*);
    int c_bt_prefix_sum_first_level_size = 0;
    c_bt_prefix_sum_first_level_size += sdsl::size_in_bytes(*(c_bt_first_level_prefix_sum_));

    int c_bt_last_level_prefix_sum_total_size = 0;

    c_bt_last_level_prefix_sum_total_size += sdsl::size_in_bytes(*(c_bt_prefix_sum_.back()));
    size = 0;
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* sum: c_bt_prefix_sum_) {
        size += sdsl::size_in_bytes(*sum);
    }
    c_bt_prefix_sum_total_size += size;


    int bt_cumulated_sum_total_size = (bt_cumulated_sum_.size()+1) * sizeof(void*);
    int bt_cumulated_sum_first_level_size = 0;

    bt_cumulated_sum_first_level_size += sdsl::size_in_bytes(*(bt_first_level_cumulated_sum_));

    int bt_last_level_cumulated_sum_total_size = 0;

    bt_last_level_cumulated_sum_total_size += sdsl::size_in_bytes(*(bt_cumulated_sum_.back()));
    size = 0;
    for (sdsl::int_vector<>* sum: bt_cumulated_sum_) {
        size += sdsl::size_in_bytes(*sum);
    }
    bt_cumulated_sum_total_size += size;

    int c_bt_cumulated_sum_total_size = (c_bt_cumulated_sum_.size()+1) * sizeof(void*);
    int c_bt_cumulated_sum_first_level_size = 0;

    c_bt_cumulated_sum_first_level_size += sdsl::size_in_bytes(*(c_bt_first_level_cumulated_sum_));

    int c_bt_last_level_cumulated_sum_total_size = 0;

    c_bt_last_level_cumulated_sum_total_size += sdsl::size_in_bytes(*(c_bt_cumulated_sum_.back()));
    size = 0;
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* sum: c_bt_cumulated_sum_) {
        size += sdsl::size_in_bytes(*sum);
    }
    c_bt_cumulated_sum_total_size += size;



    int bt_first_sum_total_size = (bt_first_sum_.size()+1) * sizeof(void*);

    size = 0;
    for (sdsl::int_vector<>* sum: bt_first_sum_) {
        size += sdsl::size_in_bytes(*sum);
    }
    bt_first_sum_total_size += size;

    int c_bt_first_sum_total_size = (c_bt_first_sum_.size()+1) * sizeof(void*);

    size = 0;
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* sum: c_bt_first_sum_) {
        size += sdsl::size_in_bytes(*sum);
    }
    c_bt_first_sum_total_size += size;



    int bt_second_sum_total_size = (bt_second_sum_.size()+1) * sizeof(void*);

    size = 0;
    for (sdsl::int_vector<>* sum: bt_second_sum_) {
        size += sdsl::size_in_bytes(*sum);
    }
    bt_second_sum_total_size += size;

    int c_bt_second_sum_total_size = (c_bt_second_sum_.size()+1) * sizeof(void*);


    size = 0;
    for (sdsl::dac_vector_dp<sdsl::rrr_vector<>>* sum: c_bt_second_sum_) {
        size += sdsl::size_in_bytes(*sum);
    }
    c_bt_second_sum_total_size += size;



    int partial_total_size = bt_bv_size+ bt_bv_rank_size+ bt_offsets_size + leaf_string_size;
    int c_partial_total_size = c_bt_bv_size+ c_bt_bv_rank_size+ c_bt_offsets_size + c_leaf_string_size;

    std::cout << "Bit Vectors Size(B): " << bt_bv_size << std::endl;
    std::cout << "Bit Vectors Rank Size(B): " << bt_bv_rank_size << std::endl;
    std::cout << "Offsets Size Plain(B): " << bt_offsets_size << std::endl;
    std::cout << "Leaves String Size(B): " << leaf_string_size << std::endl;
    std::cout << "Sum Leaves String Size(B): " << sum_leaf_string_size << std::endl;
    std::cout << "Mapped Leaves String Size(B): " << mapped_leaf_string_size << std::endl;
    std::cout << "Mapped Sum Leaves String Size(B): " << sum_mapped_leaf_string_size << std::endl;
    std::cout << "Map Size(B): " << map_size << std::endl;
    std::cout << "Map Sum Size(B): " << map_sum_size << std::endl;
    std::cout << "Partial Total Size(B): " << partial_total_size << std::endl;


    std::cout << "Compressed Bit Vectors Size(B): " << c_bt_bv_size << std::endl;
    std::cout << "Compressed Bit Vectors Rank Size(B): " << c_bt_bv_rank_size << std::endl;
    std::cout << "Compressed Offsets Size Plain(B): " << c_bt_offsets_size << std::endl;
    std::cout << "Compressed Leaves String Size(B): " << c_leaf_string_size << std::endl;
    std::cout << "Compressed Sum Leaves String Size(B): " << c_sum_leaf_string_size << std::endl;
    std::cout << "Compressed Mapped Leaves String Size(B): " << c_mapped_leaf_string_size << std::endl;
    std::cout << "Compressed Mapped Sum Leaves String Size(B): " << c_sum_mapped_leaf_string_size << std::endl;
    std::cout << "Compressed Partial Total Size(B): " << c_partial_total_size << std::endl;
//    std::cout << "WT Leaves String Size(B):" << wt_leaf_string_size << std::endl;


    std::cout << std::endl << "SUM" << std::endl;
    std::cout << "first_level_prefix_sum Plain(B): " << bt_prefix_sum_first_level_size << std::endl;
    std::cout << "first_level_cumulated_sum Plain(B): " << bt_cumulated_sum_first_level_size << std::endl;
    std::cout << "sum Size Plain(B): " << bt_sum_total_size << std::endl;
    std::cout << "prefix_sum Size Plain(B): " << bt_prefix_sum_total_size << std::endl;
    std::cout << "cumulated_sum Size Plain(B): " << bt_cumulated_sum_total_size << std::endl;
    std::cout << "first_sum Size Plain(B): " << bt_first_sum_total_size << std::endl;
    std::cout << "second_sum Size Plain(B): " << bt_second_sum_total_size << std::endl << std::endl;

    std::cout << std::endl << "Compressed SUM" << std::endl;
    std::cout << "Compressed first_level_prefix_sum(B): " << c_bt_prefix_sum_first_level_size << std::endl;
    std::cout << "Compressed first_level_cumulated_sum(B): " << c_bt_cumulated_sum_first_level_size << std::endl;
    std::cout << "Compressed sum Size(B): " << c_bt_sum_total_size << std::endl;
    std::cout << "Compressed prefix_sum Size(B): " << c_bt_prefix_sum_total_size << std::endl;
    std::cout << "Compressed cumulated_sum Size(B): " << c_bt_cumulated_sum_total_size << std::endl;
    std::cout << "Compressed first_sum Size(B): " << c_bt_first_sum_total_size << std::endl;
    std::cout << "Compressed second_sum Size(B): " << c_bt_second_sum_total_size << std::endl << std::endl;


    int sum_paper_version_size = bt_prefix_sum_first_level_size + bt_first_sum_total_size + bt_second_sum_total_size + bt_prefix_sum_total_size;
    int sum_alternative_version_size = bt_second_sum_total_size + bt_sum_total_size + bt_prefix_sum_first_level_size;
    int sum_better_version_size = bt_second_sum_total_size + bt_cumulated_sum_first_level_size + bt_cumulated_sum_total_size;
    std::cout << "SUM Paper Version Size(B): " << sum_paper_version_size << std::endl;
    std::cout << "SUM Alternative Version Size(B): " << sum_alternative_version_size << std::endl;
    std::cout << "SUM Better Version Size(B): " << sum_better_version_size << std::endl << std::endl;

    int c_sum_paper_version_size = c_bt_prefix_sum_first_level_size + c_bt_first_sum_total_size + c_bt_second_sum_total_size + c_bt_prefix_sum_total_size;
    int c_sum_alternative_version_size = c_bt_second_sum_total_size + c_bt_sum_total_size + c_bt_prefix_sum_first_level_size;
    int c_sum_better_version_size = c_bt_second_sum_total_size + c_bt_cumulated_sum_first_level_size + c_bt_cumulated_sum_total_size;
    std::cout << "Compressed SUM Paper Version Size(B): " << c_sum_paper_version_size << std::endl;
    std::cout << "Compressed SUM Alternative Version Size(B): " << c_sum_alternative_version_size << std::endl;
    std::cout << "Compressed SUM Better Version Size(B): " << c_sum_better_version_size << std::endl << std::endl;

    std::cout << "TOTAL" << std::endl;
    std::cout << "Total Size (with paper sum)(B): " << sum_paper_version_size + partial_total_size << std::endl;
    std::cout << "Total Size (with alternative sum)(B): " << sum_alternative_version_size + partial_total_size  << std::endl;
    std::cout << "Total Size (with better sum)(B): " << sum_better_version_size + partial_total_size << std::endl << std::endl;

    std::cout << "Compressed WITHOUT SUM COMPRESSION" << std::endl;
    std::cout << "Compressed Total Size (with paper sum)(B): " << sum_paper_version_size + c_partial_total_size << std::endl;
    std::cout << "Compressed Total Size (with alternative sum)(B): " << sum_alternative_version_size + c_partial_total_size << std::endl;
    std::cout << "Compressed Total Size (with better sum)(B): " << sum_better_version_size + c_partial_total_size << std::endl << std::endl;

    std::cout << "Compressed TOTAL" << std::endl;
    std::cout << "Compressed Total Size (with paper sum)(B): " << c_sum_paper_version_size + c_partial_total_size << std::endl;
    std::cout << "Compressed Total Size (with alternative sum)(B): " << c_sum_alternative_version_size + c_partial_total_size << std::endl;
    std::cout << "Compressed Total Size (with better sum)(B): " << c_sum_better_version_size + c_partial_total_size << std::endl << std::endl;

/*
    std::cout << "PERFORMANCE" << std::endl;


    bool ok = true;
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int i = 0; i < input_.size(); ++i) {
        ok = ok && (access(i) == ((int)input_[i]));
    }
    if (!ok) std::cout << "Error" << std::endl;

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    int user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    int system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total Access Time(us): " << user_time+system_time << std::endl;



    std::unordered_map<int,int> ranks;
    for (auto pair : bt_ranks_)
        ranks[pair.first] = 0;

    ok = true;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < input_.size(); ++i) {
        ranks[input_[i]] = ranks[input_[i]] + 1;
        ok = ok && (rank(input_[i], i) == ranks[input_[i]]);
    }
    if (!ok) std::cout << "Error" << std::endl;

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total rank Time (us): " << user_time+system_time << std::endl;



    for (auto pair : bt_ranks_)
        ranks[pair.first] = 0;

    ok = true;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int i = 0; i<input_.size(); ++i) {
        ranks[input_[i]] = ranks[input_[i]] + 1;
        ok = ok && (select(input_[i], ranks[input_[i]]) == i);
    }
    if (!ok) std::cout << "Error" << std::endl;

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total select Time (us): " << user_time+system_time << std::endl;




    for (auto pair : bt_ranks_)
        ranks[pair.first] = 0;

    ok = true;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < input_.size(); ++i) {
        ranks[input_[i]] = ranks[input_[i]] + 1;
        ok = ok && (rank_alternative(input_[i], i) == ranks[input_[i]]);
    }
    if (!ok) std::cout << "Error" << std::endl;

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total rank_alternative Time (us): " << user_time+system_time << std::endl;



    for (auto pair : bt_ranks_)
        ranks[pair.first] = 0;

    ok = true;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int i = 0; i<input_.size(); ++i) {
        ranks[input_[i]] = ranks[input_[i]] + 1;
        ok = ok && (select_alternative(input_[i], ranks[input_[i]]) == i);
    }
    if (!ok) std::cout << "Error" << std::endl;

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total select_alternative Time (us): " << user_time+system_time << std::endl;


    for (auto pair : bt_ranks_)
        ranks[pair.first] = 0;

    ok = true;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < input_.size(); ++i) {
        ranks[input_[i]] = ranks[input_[i]] + 1;
        ok = ok && (better_rank(input_[i], i) == ranks[input_[i]]);
    }
    if (!ok) std::cout << "Error" << std::endl;

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total better_rank Time (us): " << user_time+system_time << std::endl;



    for (auto pair : bt_ranks_)
        ranks[pair.first] = 0;

    ok = true;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int i = 0; i<input_.size(); ++i) {
        ranks[input_[i]] = ranks[input_[i]] + 1;
        ok = ok && (better_select(input_[i], ranks[input_[i]]) == i);
    }
    if (!ok) std::cout << "Error" << std::endl;

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total better_select Time (us): " << user_time+system_time << std::endl;
*/
}

int64_t encoded_sum(int64_t sum) {
    bool sgn = sum < 0;
    if (sgn) sum = -sum;
    return 2*sum+((sgn)?1:0);
}

int64_t decoded_sum(int64_t sum) {
    return (sum%2) ? -(sum/2) : (sum/2);
}