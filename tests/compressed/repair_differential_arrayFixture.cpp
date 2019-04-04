//
// Created by elarielcl on 01/10/18.
//


//#include <bits/stdc++.h>
#include <padding_blocktree/PLeafBlock.h>
#include <padding_blocktree/PLazyInternalBlock.h>
#include <padding_blocktree/PInternalBlock.h>
#include <unordered_set>
#include <fstream>
#include <compressed/PBTNONEDABTCST.h>
#include "gtest/gtest.h"
#include <lcsa/repair_differential_array.hpp>
#include "padding_blocktree/PBlockTree.h"

using ::testing::Combine;
using ::testing::Values;


class repair_differential_arrayFixture : public ::testing::TestWithParam<std::vector<int>> { // Some tests for this class encoded for an specific input banana. Also, this doesn't check topology specifics methods
protected:
    virtual void TearDown() {
        delete da_;
    }

    virtual void SetUp() {
        input_ = GetParam();

        da_ = new repair_differential_array::repair_differential_array(input_, 8);
        //cst_->print_statistics();

        //cst = new CST("/home/elarielcl/Thesis/Software/BlockTree/cmake-build-debug/in", "/home/elarielcl/Thesis/Software/BlockTree/cmake-build-debug/albertotest", trlcsa, 10, 0);

    }

public:

    std::vector<int> input_;
    repair_differential_array::repair_differential_array* da_;
    //CST* cst;

    repair_differential_arrayFixture() : ::testing::TestWithParam<std::vector<int>>() {
    }

    virtual ~repair_differential_arrayFixture() {
    }
};

INSTANTIATE_TEST_CASE_P(PBTCSTTest,
                        repair_differential_arrayFixture,
                        Values(std::vector<int>({0,1,0,1,0,1,0,0,0,0,0,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1,01,0,0,0,0,0,0,1,1,1,1,1,0,1,0,1,01,1,1,1,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,0,0,1,0,1,0,0,1,1,0,1,0,1})/*, "../../../tests/data/einstein"*/));



// This test checks equality between both array representations
TEST_P(repair_differential_arrayFixture, equality_check) {
    for (int i = 0; i < input_.size(); ++i) {
        EXPECT_EQ(input_[i], (*da_)[i]) << i << std::endl;
        int l = input_[i];
        if (l != (*da_)[i])
            (*da_)[i];
    }
}
