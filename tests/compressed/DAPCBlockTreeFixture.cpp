
#include <unordered_set>
#include <fstream>
#include <compressed/DAPCBlockTree.h>
#include "gtest/gtest.h"
#include <sdsl/csa_bitcompressed.hpp>
#include "padding_blocktree/PBlockTree.h"

using ::testing::Combine;
using ::testing::Values;

typedef PBlockTree* CreateBlockTreeFunc(int, int, std::basic_string<int64_t>);

PBlockTree* block_tree(int r, int max_leaf_length, std::basic_string<int64_t> input) {

    PBlockTree* block_tree_ = new PBlockTree(input, r, max_leaf_length);
    block_tree_->wprocess_back_pointers();
    block_tree_->wclean_unnecessary_expansions();
    return block_tree_;
}

PBlockTree* block_tree_without_cleanning(int r, int max_leaf_length, std::basic_string<int64_t> input) {
    PBlockTree* block_tree_ = new PBlockTree(input, r, max_leaf_length);
    block_tree_->wprocess_back_pointers();
    return block_tree_;
}

PBlockTree* heuristic_concatenate_block_tree(int r, int max_leaf_length, std::basic_string<int64_t> input) {
    PBlockTree* block_tree_ = new PBlockTree(input, r, max_leaf_length);
    block_tree_->wprocess_back_pointers_heuristic_concatenate();
    return block_tree_;
}


PBlockTree* heuristic_block_tree(int r, int max_leaf_length, std::basic_string<int64_t> input) {
    PBlockTree* block_tree_ = new PBlockTree(input, r, max_leaf_length);
    block_tree_->wprocess_back_pointers_heuristic();
    return block_tree_;
}



PBlockTree* liberal_heuristic_block_tree(int r, int max_leaf_length, std::basic_string<int64_t> input) {
    PBlockTree* block_tree_ = new PBlockTree(input, r, max_leaf_length);
    block_tree_->wprocess_back_pointers_liberal_heuristic();
    return block_tree_;
}


PBlockTree* conservative_heuristic_block_tree(int r, int max_leaf_length, std::basic_string<int64_t> input) {
    PBlockTree* block_tree_ = new PBlockTree(input, r, max_leaf_length);
    block_tree_->wprocess_back_pointers_conservative_heuristic();
    return block_tree_;
}


PBlockTree* conservative_heuristic_3_block_tree(int r, int max_leaf_length, std::basic_string<int64_t> input) {
    PBlockTree* block_tree_ = new PBlockTree(input, r, max_leaf_length);
    block_tree_->wprocess_back_pointers_conservative_heuristic(3);
    return block_tree_;
}


PBlockTree* conservative_optimized_heuristic_block_tree(int r, int max_leaf_length, std::basic_string<int64_t> input) {
    PBlockTree* block_tree_ = new PBlockTree(input, r, max_leaf_length);
    block_tree_->wprocess_back_pointers_conservative_optimized_heuristic();
    return block_tree_;
}

class DAPCBlockTreeFixture : public ::testing::TestWithParam<::testing::tuple<int, int, std::string, CreateBlockTreeFunc*>> {
protected:
    virtual void TearDown() {
        delete block_tree_;
        delete c_block_tree_;
    }

    virtual void SetUp() {
        CreateBlockTreeFunc* create_blocktree = ::testing::get<3>(GetParam());
        r_ = ::testing::get<0>(GetParam());
        max_leaf_length_ = ::testing::get<1>(GetParam());

        std::ifstream t(::testing::get<2>(GetParam()));
        std::stringstream buffer;
        buffer << t.rdbuf();
        input_= buffer.str();


        csa_ = new sdsl::csa_bitcompressed<>();
        sdsl::construct_im((*csa_), input_, 1);
        std::basic_string<int64_t> dsa;
        dsa += (*csa_)[0];
        for (int i = 0; i < (*csa_).size()-1; ++i) {
            dsa += (*csa_)[i+1]-(*csa_)[i];
        }

        block_tree_ = (*create_blocktree)(r_ , max_leaf_length_, dsa);
        block_tree_->add_differential_access_support();
        c_block_tree_ = new DAPCBlockTree(block_tree_);

    }

public:
    PBlockTree* block_tree_;

    DAPCBlockTree* c_block_tree_;

    sdsl::csa_bitcompressed<>* csa_;
    std::string input_;
    int r_;
    int max_leaf_length_;

    int final_ = 100;
    DAPCBlockTreeFixture() : ::testing::TestWithParam<::testing::tuple<int, int, std::string, CreateBlockTreeFunc*>>() {
    }

    virtual ~DAPCBlockTreeFixture() {
    }
};

INSTANTIATE_TEST_CASE_P(DAPCBlockTreeTest,
                        DAPCBlockTreeFixture,
                        Combine(Values(2),
                                Values(4),
                                Values("../../../tests/data/as", "../../../tests/data/dna", "../../../tests/data/dna.par"/*, "../../../tests/data/einstein"*/),
                                Values(&block_tree, &block_tree_without_cleanning, &heuristic_concatenate_block_tree, &heuristic_block_tree,
                                       &liberal_heuristic_block_tree, &conservative_heuristic_block_tree, &conservative_heuristic_3_block_tree, &conservative_optimized_heuristic_block_tree)));


// This test checks whether the differential_access_alternative
// method works properly
TEST_P(DAPCBlockTreeFixture, differential_access_alternative_check) {
    for (int i = 0; i < final_; ++i)
        EXPECT_EQ(c_block_tree_->differential_access_alternative(i), (*csa_)[i]);
}


// This test checks whether the mapped_differential_access_alternative
// method works properly
TEST_P(DAPCBlockTreeFixture, mapped_differential_access_alternative_check) {
    for (int i = 0; i < final_; ++i)
        EXPECT_EQ(c_block_tree_->mapped_differential_access_alternative(i), (*csa_)[i]);
}



// This test checks whether the sdifferential_access_alternative
// method works properly
TEST_P(DAPCBlockTreeFixture, sdifferential_access_alternative_check) {
    for (int i = 0; i < final_; ++i)
        EXPECT_EQ(c_block_tree_->sdifferential_access_alternative(i), (*csa_)[i]);
}



// This test checks whether the mapped_sdifferential_access_alternative
// method works properly
TEST_P(DAPCBlockTreeFixture, mapped_sdifferential_access_alternative_check) {
    for (int i = 0; i < final_; ++i)
        EXPECT_EQ(c_block_tree_->mapped_sdifferential_access_alternative(i), (*csa_)[i]);
}



// This test checks whether the cdifferential_access_alternative
// method works properly
TEST_P(DAPCBlockTreeFixture, cdifferential_access_alternative_check) {
    for (int i = 0; i < final_; ++i)
        EXPECT_EQ(c_block_tree_->cdifferential_access_alternative(i), (*csa_)[i]);
}

// This test checks whether the mapped_cdifferential_access_alternative
// method works properly
TEST_P(DAPCBlockTreeFixture, mapped_cdifferential_access_alternative_check) {
    for (int i = 0; i < final_; ++i)
        EXPECT_EQ(c_block_tree_->mapped_cdifferential_access_alternative(i), (*csa_)[i]);
}



// This test checks whether the mapped_csdifferential_access_alternative
// method works properly
TEST_P(DAPCBlockTreeFixture, mapped_csdifferential_access_alternative_check) {
    for (int i = 0; i < final_; ++i)
        EXPECT_EQ(c_block_tree_->mapped_csdifferential_access_alternative(i), (*csa_)[i]);
}
