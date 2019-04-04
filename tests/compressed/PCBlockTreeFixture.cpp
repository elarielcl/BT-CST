//
// Created by sking32 on 5/23/18.
//

//
// Created by sking32 on 5/22/18.
//

//
// Created by sking32 on 3/11/18.
//

#include <padding_blocktree/PLeafBlock.h>
#include <padding_blocktree/PLazyInternalBlock.h>
#include <padding_blocktree/PInternalBlock.h>
#include <unordered_set>
#include <fstream>
#include <compressed/PCBlockTree.h>
#include "gtest/gtest.h"

#include "padding_blocktree/PBlockTree.h"

using ::testing::Combine;
using ::testing::Values;

typedef PBlockTree* CreateBlockTreeFunc(int, int, std::string);

PBlockTree* block_tree(int r, int max_leaf_length, std::string input) {

    PBlockTree* block_tree_ = new PBlockTree(input, r, max_leaf_length);
    block_tree_->process_back_pointers();
    block_tree_->clean_unnecessary_expansions();
    return block_tree_;
}

PBlockTree* block_tree_without_cleanning(int r, int max_leaf_length, std::string input) {
    PBlockTree* block_tree_ = new PBlockTree(input, r, max_leaf_length);
    block_tree_->process_back_pointers();
    return block_tree_;
}

PBlockTree* heuristic_concatenate_block_tree(int r, int max_leaf_length, std::string input) {
    PBlockTree* block_tree_ = new PBlockTree(input, r, max_leaf_length);
    block_tree_->process_back_pointers_heuristic_concatenate();
    return block_tree_;
}


PBlockTree* heuristic_block_tree(int r, int max_leaf_length, std::string input) {
    PBlockTree* block_tree_ = new PBlockTree(input, r, max_leaf_length);
    block_tree_->process_back_pointers_heuristic();
    return block_tree_;
}



PBlockTree* liberal_heuristic_block_tree(int r, int max_leaf_length, std::string input) {
    PBlockTree* block_tree_ = new PBlockTree(input, r, max_leaf_length);
    block_tree_->process_back_pointers_liberal_heuristic();
    return block_tree_;
}


PBlockTree* conservative_heuristic_block_tree(int r, int max_leaf_length, std::string input) {
    PBlockTree* block_tree_ = new PBlockTree(input, r, max_leaf_length);
    block_tree_->process_back_pointers_conservative_heuristic();
    return block_tree_;
}


PBlockTree* conservative_heuristic_3_block_tree(int r, int max_leaf_length, std::string input) {
    PBlockTree* block_tree_ = new PBlockTree(input, r, max_leaf_length);
    block_tree_->process_back_pointers_conservative_heuristic(3);
    return block_tree_;
}


PBlockTree* conservative_optimized_heuristic_block_tree(int r, int max_leaf_length, std::string input) {
    PBlockTree* block_tree_ = new PBlockTree(input, r, max_leaf_length);
    block_tree_->process_back_pointers_conservative_optimized_heuristic();
    return block_tree_;
}

class PCBlockTreeFixture : public ::testing::TestWithParam<::testing::tuple<int, int, std::string, CreateBlockTreeFunc*>> {
protected:
    virtual void TearDown() {
        delete block_tree_;
        delete block_tree_rs_;
        delete c_block_tree_;
        delete c_block_tree_rs_;
    }

    virtual void SetUp() {
        CreateBlockTreeFunc* create_blocktree = ::testing::get<3>(GetParam());
        r_ = ::testing::get<0>(GetParam());
        max_leaf_length_ = ::testing::get<1>(GetParam());

        std::ifstream t(::testing::get<2>(GetParam()));
        std::stringstream buffer;
        buffer << t.rdbuf();
        input_= buffer.str();
        block_tree_ = (*create_blocktree)(r_ , max_leaf_length_, input_);
        c_block_tree_ = new PCBlockTree(block_tree_);

        block_tree_rs_ = (*create_blocktree)(r_ , max_leaf_length_, input_);

        std::unordered_set<int> characters;
        for (char c: input_)
            characters.insert(c);
        for (int c: characters) {
            characters_[c] = {};
            block_tree_rs_->add_rank_select_support(c);
        }

        for (int i = 0; i<input_.size(); ++i)
            characters_[input_[i]].push_back(i);

        c_block_tree_rs_ = new PCBlockTree(block_tree_rs_);
    }

public:
    PBlockTree* block_tree_;
    PBlockTree* block_tree_rs_;

    PCBlockTree* c_block_tree_;
    PCBlockTree* c_block_tree_rs_;

    std::string input_;
    int r_;
    int max_leaf_length_;
    std::unordered_map<int,std::vector<int>> characters_; // Characters in the input and its select results

    PCBlockTreeFixture() : ::testing::TestWithParam<::testing::tuple<int, int, std::string, CreateBlockTreeFunc*>>() {
    }

    virtual ~PCBlockTreeFixture() {
    }
};

INSTANTIATE_TEST_CASE_P(PCBlockTreeTest,
                        PCBlockTreeFixture,
                        Combine(Values(2),
                                Values(4),
                                Values("../../../tests/data/as", "../../../tests/data/dna", "../../../tests/data/dna.par"/*, "../../../tests/data/einstein"*/),
                                Values(&block_tree, &block_tree_without_cleanning, &heuristic_concatenate_block_tree, &heuristic_block_tree,
                                       &liberal_heuristic_block_tree, &conservative_heuristic_block_tree, &conservative_heuristic_3_block_tree, &conservative_optimized_heuristic_block_tree)));

// This test checks if the fields r_, max_leaf_length_
// number_of_levels_, and number_of_blocks_first_level_
// are correct
TEST_P(PCBlockTreeFixture, general_fields_check) {
    EXPECT_EQ(c_block_tree_->r_, r_);
    EXPECT_EQ(c_block_tree_->leaf_length_, block_tree_->leaf_length_);
    auto iterator = block_tree_->levelwise_iterator();
    std::vector<PBlock*> level;
    bool contains_back_block = false;
    for (int i = 0; i < iterator.size(); ++i) {
        level = iterator[i];
        for (PBlock *b : level) {
            if (dynamic_cast<PBackBlock*>(b)) contains_back_block = true;
        }
        if (contains_back_block) break;
    }
    EXPECT_EQ(level.size(), c_block_tree_->number_of_blocks_first_level_);
    EXPECT_EQ(level.front()->height()+1, c_block_tree_->number_of_levels_);

}

// This test checks if the CBlockTree has the same
// structure that its correspondent BlockTree
// in particular the bt_bv field is checked
TEST_P(PCBlockTreeFixture, bt_bv_field_check) {
    auto iterator = block_tree_->levelwise_iterator();
    std::vector<PBlock*> level;
    bool contains_back_block = false;
    int i = 0;
    for (; i < iterator.size(); ++i) {
        level = iterator[i];
        for (PBlock *b : level) {
            if (dynamic_cast<PBackBlock*>(b)) contains_back_block = true;
        }
        if (contains_back_block) break;
    }

    for (int j = 0; j < c_block_tree_->number_of_levels_-1; ++j) {
        level = iterator[i + j];
        auto level_bt_bv = *(c_block_tree_->bt_bv_[j]);
        EXPECT_EQ(level.size(), level_bt_bv.size());
        for (int k = 0; k < level.size(); ++k) {
            PBlock *b = level[k];
            if (dynamic_cast<PBackBlock *>(b)) EXPECT_FALSE(level_bt_bv[k]);
            else
                EXPECT_TRUE(level_bt_bv[k]);
        }
    }
}


// This test checks if the CBlockTree has the same
// structure that its correspondent BlockTree
// in particular the bt_offsets field is checked
TEST_P(PCBlockTreeFixture, bt_offsets_field_check) {
    auto iterator = block_tree_->levelwise_iterator();
    std::vector<PBlock*> level;
    bool contains_back_block = false;
    int i = 0;
    for (; i < iterator.size(); ++i) {
        level = iterator[i];
        for (PBlock *b : level) {
            if (dynamic_cast<PBackBlock*>(b)) contains_back_block = true;
        }
        if (contains_back_block) break;
    }

    for (int j = 0; j < c_block_tree_->number_of_levels_-1; ++j) {
        level = iterator[i+j];
        auto level_bt_offsets = *(c_block_tree_->bt_offsets_[j]);

        int max_size_level = level.front()->length();
        int l = 0;
        for (PBlock* b: level) {
            if (dynamic_cast<PBackBlock*>(b)) {
                EXPECT_EQ(level_bt_offsets[l], max_size_level*b->first_block_->level_index_ + b->offset_);
                ++l;
            }
        }
        EXPECT_EQ(l, level_bt_offsets.size());
    }
}



// This test checks if the CBlockTree has the same
// structure that its correspondent BlockTree
// in particular the bt_leaf_string field is checked
TEST_P(PCBlockTreeFixture, bt_leaf_string_field_check) {
    auto iterator = block_tree_->levelwise_iterator();
    std::string leaf_string = "";
    for (PBlock* b : iterator.back()) {
        leaf_string += b->represented_string();
    }

    std::string leaf_c_string = "";
    for (int i : (*c_block_tree_->leaf_string_)) {
        leaf_c_string += (char)(*c_block_tree_->alphabet_)[i];
    }

    EXPECT_EQ(leaf_c_string, leaf_string);
}




// This test checks if the CBlockTree has the same
// structure that its correspondent BlockTree
// in particular the bt_first|second_ranks field are checked
TEST_P(PCBlockTreeFixture, bt_first_second_ranks_field_check) {
    auto iterator = block_tree_rs_->levelwise_iterator();
    std::vector<PBlock*> level;
    bool contains_back_block = false;
    int i = 0;
    for (; i < iterator.size(); ++i) {
        level = iterator[i];
        for (PBlock *b : level) {
            if (dynamic_cast<PBackBlock*>(b)) contains_back_block = true;
        }
        if (contains_back_block) break;
    }

    for (int j = 0; j < c_block_tree_->number_of_levels_-1; ++j) {
        level = iterator[i+j];
        for (auto pair : characters_) {
            int c = pair.first;
            auto level_bt_first_ranks = *(c_block_tree_rs_->bt_first_ranks_[c][j]);
            auto level_bt_second_ranks = *(c_block_tree_rs_->bt_second_ranks_[c][j]);

            int l = 0;
            for (PBlock *b: level) {
                if (dynamic_cast<PBackBlock *>(b)) {
                    EXPECT_EQ(level_bt_first_ranks[l], b->first_ranks_[c]) ;
                    EXPECT_EQ(level_bt_second_ranks[l], b->second_ranks_[c]) ;
                    ++l;
                }
            }
            EXPECT_EQ(l, level_bt_first_ranks.size());
            EXPECT_EQ(l, level_bt_second_ranks.size());
        }
    }
}

// This test checks if the CBlockTree has the same
// structure that its correspondent BlockTree
// in particular the bt_ranks_, bt_prefix_ranks_,
// bt_cumulated_ranks_ are checked
TEST_P(PCBlockTreeFixture, bt_bv_ranks_prefix_cumulated_check) {
    auto iterator = block_tree_rs_->levelwise_iterator();
    std::vector<PBlock*> level;
    bool contains_back_block = false;
    int i = 0;
    for (; i < iterator.size(); ++i) {
        level = iterator[i];
        for (PBlock *b : level) {
            if (dynamic_cast<PBackBlock*>(b)) contains_back_block = true;
        }
        if (contains_back_block) break;
    }


    for (auto pair : characters_) {
        int c = pair.first;
        level = iterator[i];
        auto level_bt_ranks = *(c_block_tree_rs_->bt_ranks_[c][0]);
        EXPECT_EQ(level.size(), level_bt_ranks.size());

        for (int k = 0; k < level.size(); ++k) {
            PBlock* b = level[k];
            EXPECT_EQ(b->ranks_[c], level_bt_ranks[k]);
        }
    }


    for (int j = 1; j < c_block_tree_->number_of_levels_; ++j) {
        for (auto pair : characters_) {
            int c = pair.first;
            level = iterator[i + j];
            auto level_bt_ranks = *(c_block_tree_rs_->bt_ranks_[c][j]);
            auto level_bt_prefix_ranks = *(c_block_tree_rs_->bt_prefix_ranks_[c][j-1]);
            auto level_bt_cumulated_ranks = *(c_block_tree_rs_->bt_cumulated_ranks_[c][j-1]) ;
            EXPECT_EQ(level.size(), level_bt_ranks.size());
            EXPECT_EQ(level.size(), level_bt_prefix_ranks.size());
            EXPECT_EQ(level.size(), level_bt_cumulated_ranks.size());

            for (int k = 0; k < level.size(); ++k) {
                PBlock* b = level[k];
                EXPECT_EQ(b->ranks_[c], level_bt_ranks[k]);
                EXPECT_EQ(b->prefix_ranks_[c], level_bt_prefix_ranks[k]);
                EXPECT_EQ(b->cumulated_ranks_[c], level_bt_cumulated_ranks[k]);
            }
        }
    }
}


// This test checks if the CBlockTree has the same
// structure that its correspondent BlockTree
// in particular the first level for bt_prefix_ranks_,
// bt_cumulated_ranks_ are checked
TEST_P(PCBlockTreeFixture, bt_bv_first_level_prefix_cumulated_ranks_check) {
    auto iterator = block_tree_rs_->levelwise_iterator();
    std::vector<PBlock*> level;
    bool contains_back_block = false;
    int i = 0;
    for (; i < iterator.size(); ++i) {
        level = iterator[i];
        for (PBlock *b : level) {
            if (dynamic_cast<PBackBlock*>(b)) contains_back_block = true;
        }
        if (contains_back_block) break;
    }


    for (auto pair : characters_) {
        int c = pair.first;
        level = iterator[i];
        auto first_level_bt_prefix_ranks = *(c_block_tree_rs_->bt_first_level_prefix_ranks_[c]);
        auto first_level_bt_cumulated_ranks = *(c_block_tree_rs_->bt_first_level_cumulated_ranks_[c]);
        int r  = 0;

        EXPECT_EQ(first_level_bt_prefix_ranks.size(), level.size());
        EXPECT_EQ(first_level_bt_cumulated_ranks.size(), level.size());
        for (int k = 0; k < level.size(); ++k) {
            EXPECT_EQ(r, first_level_bt_prefix_ranks[k]);
            r += level[k]->ranks_[c];
            EXPECT_EQ(r, first_level_bt_cumulated_ranks[k]);
        }
    }

}




// This test checks if the mapping and alphabet fields are correct
TEST_P(PCBlockTreeFixture, bt_mapping_alphabet_field_check) {
    EXPECT_EQ(c_block_tree_rs_->mapping_.size(), characters_.size());
    EXPECT_EQ(c_block_tree_rs_->alphabet_->size(), characters_.size());
    for (int i = 0; i < c_block_tree_->alphabet_->size(); ++i) {
        EXPECT_EQ(c_block_tree_rs_->mapping_[(*c_block_tree_->alphabet_)[i]], i);
    }
}


// This test checks if the CBlockTree represents its input
// string and if the access method is correct
TEST_P(PCBlockTreeFixture, input_integrity_or_access_check) {
    for (int i = 0; i < input_.size(); ++i)
        EXPECT_EQ(c_block_tree_->access(i), input_[i]);
}

// This test checks the rank method for every character
// and position in the input
TEST_P(PCBlockTreeFixture, ranks_check) {
    for (auto pair : characters_) {
        int c = pair.first;
        int r = 0;
        for (int i = 0; i < input_.size(); ++i) {
            if (input_[i] == c) ++r;
            EXPECT_EQ(c_block_tree_rs_->rank(c, i), r);
        }
    }
}


// This test checks the better_paper_rank method for every character
// and position in the input
TEST_P(PCBlockTreeFixture, better_paper_ranks_check) {
    for (auto pair : characters_) {
        int c = pair.first;
        int r = 0;
        for (int i = 0; i < input_.size(); ++i) {
            if (input_[i] == c) ++r;
            EXPECT_EQ(c_block_tree_rs_->better_paper_rank(c, i), r);
        }
    }
}

// This test checks the rank_alternative method for every character
// and position in the input
TEST_P(PCBlockTreeFixture, ranks_alternative_check) {
    for (auto pair : characters_) {
        int c  = pair.first;
        int r = 0;
        for (int i = 0; i < input_.size(); ++i) {
            if (input_[i] == c) ++r;
            EXPECT_EQ(c_block_tree_rs_->rank_alternative(c, i), r);
        }
    }
}

// This test checks the optimized_rank_alternative method for every character
// and position in the input
TEST_P(PCBlockTreeFixture, optimized_ranks_alternative_check) {
    for (auto pair : characters_) {
        int c  = pair.first;
        int r = 0;
        for (int i = 0; i < input_.size(); ++i) {
            if (input_[i] == c) ++r;
            EXPECT_EQ(c_block_tree_rs_->optimized_rank_alternative(c, i), r);
        }
    }
}

// This test checks the c_optimized_rank_alternative method for every character
// and position in the input
TEST_P(PCBlockTreeFixture, c_optimized_ranks_alternative_check) {
for (auto pair : characters_) {
int c  = pair.first;
int r = 0;
for (int i = 0; i < input_.size(); ++i) {
if (input_[i] == c) ++r;
EXPECT_EQ(c_block_tree_rs_->c_optimized_rank_alternative(c, i), r);
}
}
}

// This test checks the nockack_rank_alternative method for every character
// and position in the input
TEST_P(PCBlockTreeFixture, noback_ranks_alternative_check) {
    for (auto pair : characters_) {
        int c  = pair.first;
        int r = 0;
        for (int i = 0; i < input_.size(); ++i) {
            if (input_[i] == c) ++r;
            EXPECT_EQ(c_block_tree_rs_->noback_rank_alternative(c, i), r);
        }
    }
}

// This test checks the c_nockack_rank_alternative method for every character
// and position in the input
TEST_P(PCBlockTreeFixture, c_noback_ranks_alternative_check) {
    for (auto pair : characters_) {
        int c  = pair.first;
        int r = 0;
        for (int i = 0; i < input_.size(); ++i) {
            if (input_[i] == c) ++r;
            EXPECT_EQ(c_block_tree_rs_->c_noback_rank_alternative(c, i), r);
        }
    }
}


// This test checks the better_rank method for every character
// and position in the input
TEST_P(PCBlockTreeFixture, better_ranks_check) {
    for (auto pair : characters_) {
        int c  = pair.first;
        int r = 0;
        for (int i = 0; i < input_.size(); ++i) {
            if (input_[i] == c) ++r;
            EXPECT_EQ(c_block_tree_rs_->better_rank(c, i), r);
        }
    }
}

// This test checks the select method for every character
// and rank
TEST_P(PCBlockTreeFixture, selects_check) {
    for (auto pair : characters_) {
        int c  = pair.first;
        for (int j = 1; j<=pair.second.size(); ++j)
            EXPECT_EQ(c_block_tree_rs_->select(c, j), pair.second[j-1]);
    }
}

// This test checks the better_paper_select method for every character
// and rank
TEST_P(PCBlockTreeFixture, better_paper_selects_check) {
    for (auto pair : characters_) {
        int c  = pair.first;
        for (int j = 1; j<=pair.second.size(); ++j) {
            EXPECT_EQ(c_block_tree_rs_->better_paper_select(c, j), pair.second[j - 1]);
        }
    }
}


// This test checks the select_alternative method for every character
// and rank
TEST_P(PCBlockTreeFixture, selects_alternative_check) {
    for (auto pair : characters_) {
        int c  = pair.first;
        for (int j = 1; j<=pair.second.size(); ++j)
            EXPECT_EQ(c_block_tree_rs_->select_alternative(c, j), pair.second[j-1]);
    }
}

// This test checks the c_noback_select_alternative method for every character
// and rank
TEST_P(PCBlockTreeFixture, c_noback_selects_alternative_check) {
    for (auto pair : characters_) {
        int c  = pair.first;
        for (int j = 1; j<=pair.second.size(); ++j)
            EXPECT_EQ(c_block_tree_rs_->c_noback_select_alternative(c, j), pair.second[j-1]) << c << " " << j;
    }
}

// This test checks the noback_select_alternative method for every character
// and rank
TEST_P(PCBlockTreeFixture, noback_selects_alternative_check) {
    for (auto pair : characters_) {
        int c  = pair.first;
        for (int j = 1; j<=pair.second.size(); ++j)
            EXPECT_EQ(c_block_tree_rs_->noback_select_alternative(c, j), pair.second[j-1]) << c << " " << j;
    }
}

// This test checks the better_select method for every character
// and rank
TEST_P(PCBlockTreeFixture, better_selects_check) {
    for (auto pair : characters_) {
        int c  = pair.first;
        for (int j = 1; j<=pair.second.size(); ++j)
            EXPECT_EQ(c_block_tree_rs_->better_select(c, j), pair.second[j-1]) << c << " " << j;
    }
}