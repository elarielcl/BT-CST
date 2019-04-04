//
// Created by sking32 on 5/22/18.
//
#include <padding_blocktree/PLeafBlock.h>
#include <padding_blocktree/PLazyInternalBlock.h>
#include <padding_blocktree/PInternalBlock.h>
#include <unordered_set>
#include <fstream>
#include "gtest/gtest.h"

#include "padding_blocktree/PBlockTree.h"

using ::testing::Combine;
using ::testing::Values;

class PBlockTreeWithoutCleanningFixture : public ::testing::TestWithParam<::testing::tuple<int, int, std::string>> {
protected:
    virtual void TearDown() {
    }

    virtual void SetUp() {
    }

public:
    PBlockTree* block_tree_;
    //BlockTree* block_tree_rs_;

    std::string input_;
    int r_;
    int max_leaf_length_;
    std::unordered_map<int,std::vector<int>> characters_; // Characters in the input and its select results

    PBlockTreeWithoutCleanningFixture() : ::testing::TestWithParam<::testing::tuple<int, int, std::string>>() {
        r_ = ::testing::get<0>(GetParam());
        max_leaf_length_ = ::testing::get<1>(GetParam());

        std::ifstream t(::testing::get<2>(GetParam()));
        std::stringstream buffer;
        buffer << t.rdbuf();
        input_= buffer.str();
        block_tree_ = new PBlockTree(input_, r_ , max_leaf_length_);
        block_tree_->process_back_pointers();

        /*
        block_tree_rs_ = new BlockTree(input_, r_ , max_leaf_length_);
        block_tree_rs_->process_back_pointers();
        block_tree_rs_->clean_unnecessary_expansions();

        std::unordered_set<int> characters;
        for (char c: input_)
            characters.insert(c);
        for (int c: characters) {
            characters_[c] = {};
            block_tree_rs_->add_rank_select_support(c);
        }

        for (int i = 0; i<input_.size(); ++i)
            characters_[input_[i]].push_back(i);
        */
    }

    virtual ~PBlockTreeWithoutCleanningFixture() {
        delete block_tree_;
//        delete block_tree_rs_;
    }
};

INSTANTIATE_TEST_CASE_P(BlockTreeConstruction,
                        PBlockTreeWithoutCleanningFixture,
                        Combine(Values(2),
                                Values(4),
                                Values("../../../tests/data/as", "../../../tests/data/dna", "../../../tests/data/dna.par"/*, "../../../tests/data/einstein"*/)));


// This test checks that back blocks don't point to themselves
TEST_P(PBlockTreeWithoutCleanningFixture, no_self_references_check) {
    std::vector<PBlock*> level = {block_tree_->root_block_};
    for (std::vector<PBlock*> level : block_tree_->levelwise_iterator()) {
        for (PBlock* b: level) {
            if (dynamic_cast<PBackBlock*>(b)) {
                EXPECT_NE(b->first_block_, b);
                if (b->second_block_ != nullptr) {
                    EXPECT_NE(b->second_block_, b);
                }
            }
        }
    }
}


// This test checks that back blocks don't point to back blocks
TEST_P(PBlockTreeWithoutCleanningFixture, no_double_pointer_check) {
    std::vector<PBlock*> level = {block_tree_->root_block_};
    for (std::vector<PBlock*> level : block_tree_->levelwise_iterator()) {
        for (PBlock* b: level) {
            if (dynamic_cast<PBackBlock*>(b)) {
                EXPECT_TRUE(dynamic_cast<PLazyInternalBlock*>(b->first_block_) || dynamic_cast<PInternalBlock*>(b->first_block_));
                if (b->second_block_ != nullptr) EXPECT_TRUE(dynamic_cast<PLazyInternalBlock*>(b->second_block_) || dynamic_cast<PInternalBlock*>(b->second_block_));
            }
        }
    }
}



// This test checks whether the back blocks points to first
// occurrences on the input string
TEST_P(PBlockTreeWithoutCleanningFixture, pointing_to_first_occurrence_check) {
    std::vector<PBlock*> level = {block_tree_->root_block_};
    for (std::vector<PBlock*> level : block_tree_->levelwise_iterator()) {
        for (PBlock* b: level) {
            if (dynamic_cast<PBackBlock*>(b)) {
                int i = input_.find(b->represented_string());
                EXPECT_EQ(b->first_block_->start_index_ + b->offset_ , i);
            }
        }
    }
}

// This test checks if the left and right flags are
// correctly set
TEST_P(PBlockTreeWithoutCleanningFixture, left_right_field_check) {
    std::vector<PBlock*> level = {block_tree_->root_block_};
    for (std::vector<PBlock*> level : block_tree_->levelwise_iterator()) {
        for (PBlock* b: level) {
            if (dynamic_cast<PBackBlock*>(b)) {
                EXPECT_TRUE(b->left_ && b->right_);
            } else {
                if (b->first_block_ != b) EXPECT_FALSE(b->left_ && b->right_ );
            }
        }
    }
}

// This test checks if the pointing_to_me field
// is correctly set
TEST_P(PBlockTreeWithoutCleanningFixture, pointing_to_me_field_check) {
    std::vector<PBlock*> level = {block_tree_->root_block_};
    for (std::vector<PBlock*> level : block_tree_->levelwise_iterator()) {
        for (PBlock* b: level) {
            if (dynamic_cast<PBackBlock*>(b)) {
                EXPECT_EQ(0, b->pointing_to_me_);
            }
        }
    }
}


// This test checks if the NO back pointer
// doesn't have reason to be there
TEST_P(PBlockTreeWithoutCleanningFixture, no_back_pointer_check) {
    std::vector<PBlock*> level = {block_tree_->root_block_};
    for (std::vector<PBlock*> level : block_tree_->levelwise_iterator()) {
        for (int i = 0; i < level.size(); ++i) {
            PBlock* b = level[i];
            if (dynamic_cast<PInternalBlock*>(b) || dynamic_cast<PLazyInternalBlock*>(b)) {
                int index = input_.find(b->represented_string());
                if (index < b->start_index_) {
                    bool check = false;
                    check |= (i != 0  && (input_.find(level[i-1]->represented_string()+b->represented_string()))== level[i-1]->start_index_);
                    check |= (i != level.size()-1  && (input_.find(b->represented_string()+level[i+1]->represented_string()))== b->start_index_);
                    check |=  (b->end_index_>= input_.size() && i == level.size()-1);
                    EXPECT_TRUE(check);
                }
            }
        }
    }
}

// This test checks if pointed blocks are consecutive
TEST_P(PBlockTreeWithoutCleanningFixture, text_consecutive_pointed_blocks_check) {
    std::vector<PBlock*> level = {block_tree_->root_block_};
    for (std::vector<PBlock*> level : block_tree_->levelwise_iterator()) {
        for (PBlock* b: level) {
            if (b->second_block_ != nullptr  && dynamic_cast<PBackBlock*>(b)) {
                EXPECT_EQ(b->first_block_->end_index_, b->second_block_->start_index_-1);
            }
        }
    }
}