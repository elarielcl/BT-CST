//
// Created by sking32 on 5/22/18.
//
#include <paper_blocktree/LeafBlock.h>
#include <paper_blocktree/LazyInternalBlock.h>
#include <paper_blocktree/InternalBlock.h>
#include <unordered_set>
#include <fstream>
#include "gtest/gtest.h"

#include "paper_blocktree/BlockTree.h"

using ::testing::Combine;
using ::testing::Values;

class BlockTreeFixture : public ::testing::TestWithParam<::testing::tuple<int, int, std::string>> {
protected:
    virtual void TearDown() {
    }

    virtual void SetUp() {
    }

    public:
    BlockTree* block_tree_;
    //BlockTree* block_tree_rs_;

    std::string input_;
    int r_;
    int max_leaf_length_;
    std::unordered_map<int,std::vector<int>> characters_; // Characters in the input and its select results

    BlockTreeFixture() : ::testing::TestWithParam<::testing::tuple<int, int, std::string>>() {
        r_ = ::testing::get<0>(GetParam());
        max_leaf_length_ = ::testing::get<1>(GetParam());

        std::ifstream t(::testing::get<2>(GetParam()));
        std::stringstream buffer;
        buffer << t.rdbuf();
        input_= buffer.str();
        block_tree_ = new BlockTree(input_, r_ , max_leaf_length_);
        block_tree_->process_back_pointers();
        block_tree_->clean_unnecessary_expansions();

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

    virtual ~BlockTreeFixture() {
        delete block_tree_;
//        delete block_tree_rs_;
    }
};

INSTANTIATE_TEST_CASE_P(BlockTreeConstruction,
                        BlockTreeFixture,
                        Combine(Values(2),
                                Values(4),
                                Values("../../../tests/data/as", "../../../tests/data/dna", "../../../tests/data/dna.par"/*, "../../../tests/data/einstein"*/)));


// This test checks that back blocks don't point to themselves
TEST_P(BlockTreeFixture, no_self_references_check) {
    std::vector<Block*> level = {block_tree_->root_block_};
    for (std::vector<Block*> level : block_tree_->levelwise_iterator()) {
        for (Block* b: level) {
            if (dynamic_cast<BackBlock*>(b)) {
                EXPECT_NE(b->first_block_, b);
                if (b->second_block_ != nullptr) {
                    EXPECT_NE(b->second_block_, b);
                }
            }
        }
    }
}


// This test checks that back blocks don't point to back blocks
TEST_P(BlockTreeFixture, no_double_pointer_check) {
    std::vector<Block*> level = {block_tree_->root_block_};
    for (std::vector<Block*> level : block_tree_->levelwise_iterator()) {
        for (Block* b: level) {
            if (dynamic_cast<BackBlock*>(b)) {
                EXPECT_TRUE(dynamic_cast<LazyInternalBlock*>(b->first_block_) || dynamic_cast<InternalBlock*>(b->first_block_));
                if (b->second_block_ != nullptr) EXPECT_TRUE(dynamic_cast<LazyInternalBlock*>(b->second_block_) || dynamic_cast<InternalBlock*>(b->second_block_));
            }
        }
    }
}



// This test checks whether the back blocks points to first
// occurrences on the input string
TEST_P(BlockTreeFixture, pointing_to_first_occurrence_check) {
    std::vector<Block*> level = {block_tree_->root_block_};
    for (std::vector<Block*> level : block_tree_->levelwise_iterator()) {
        for (Block* b: level) {
            if (dynamic_cast<BackBlock*>(b)) {
                int i = input_.find(b->represented_string());
                EXPECT_EQ(b->first_block_->start_index_ + b->offset_ , i);
            }
        }
    }
}

// This test checks if the left and right flags are
// correctly set
TEST_P(BlockTreeFixture, left_right_field_check) {
    std::vector<Block*> level = {block_tree_->root_block_};
    for (std::vector<Block*> level : block_tree_->levelwise_iterator()) {
        for (Block* b: level) {
            if (dynamic_cast<BackBlock*>(b)) {
                EXPECT_TRUE(b->left_ && b->right_);
            } else {
                EXPECT_FALSE(b->left_ && b->right_);
            }
        }
    }
}

// This test checks if the pointing_to_me field
// is correctly set
TEST_P(BlockTreeFixture, pointing_to_me_field_check) {
    std::vector<Block*> level = {block_tree_->root_block_};
    for (std::vector<Block*> level : block_tree_->levelwise_iterator()) {
        for (Block* b: level) {
            if (dynamic_cast<BackBlock*>(b)) {
                EXPECT_EQ(0, b->pointing_to_me_);
            }
        }
    }
}


bool contains_necessary_expansion(Block* b) {
    std::vector<Block*> level = b->children_;
    while (level.size() != 0) {
        std::vector<Block*> next_level;
        for (Block* b : level) {
            if (b->pointing_to_me_ != 0) return true;
            for (Block *child : b->children_)
                next_level.push_back(child);
        }
        level = next_level;
    }
    return false;
}

// This test checks if there is no unnecessary expansions
TEST_P(BlockTreeFixture, no_unnecessary_expansions_check) {
    std::vector<Block*> level = {block_tree_->root_block_};
    for (std::vector<Block*> level : block_tree_->levelwise_iterator()) {
        for (Block* b: level) {
            if (dynamic_cast<InternalBlock*>(b) || dynamic_cast<LazyInternalBlock*>(b)) {
                EXPECT_TRUE(b->pointing_to_me_ != 0 || b->first_occurrence_level_index_ == b->level_index_ ||
                contains_necessary_expansion(b));
            }
        }
    }
}

// This test checks if the NO back pointer
// doesn't have reason to be there
TEST_P(BlockTreeFixture, no_back_pointer_check) {
    std::vector<Block*> level = {block_tree_->root_block_};
    for (std::vector<Block*> level : block_tree_->levelwise_iterator()) {
        for (int i = 0; i < level.size(); ++i) {
            Block* b = level[i];
            if (dynamic_cast<InternalBlock*>(b) || dynamic_cast<LazyInternalBlock*>(b)) {
                int index = input_.find(b->represented_string());
                if (index < b->start_index_) {
                    bool check = false;
                    check |= (i != 0  && (input_.find(level[i-1]->represented_string()+b->represented_string()))== level[i-1]->start_index_);
                    check |= (i != level.size()-1  && (input_.find(b->represented_string()+level[i+1]->represented_string()))== b->start_index_);
                    EXPECT_TRUE(check);
                }
            }
        }
    }
}

// This test checks if pointed blocks are consecutive
TEST_P(BlockTreeFixture, text_consecutive_pointed_blocks_check) {
    std::vector<Block*> level = {block_tree_->root_block_};
    for (std::vector<Block*> level : block_tree_->levelwise_iterator()) {
        for (Block* b: level) {
            if (b->second_block_ != nullptr  && dynamic_cast<BackBlock*>(b)) {
                EXPECT_EQ(b->first_block_->end_index_, b->second_block_->start_index_-1);
            }
        }
    }
}