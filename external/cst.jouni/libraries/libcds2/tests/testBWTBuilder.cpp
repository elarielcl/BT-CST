#include <gtest/gtest.h>
#include <text_indexes/BWTBuilder.h>
using namespace cds_static;


TEST(BWT_BUILDER_CHAR, SA_CORRECT){
    char input[256] = "alabar_a_la_alabarda";
    uint SA_TEST[21]={20,6,11,8,19,10,7,2,14,0,12,4,16,3,15,18,9,1,13,5,17};
    size_t n = 20;
    uint *SA;
    uint *bwt = BWTBuilder::buildBWT(input,n,SA,false);

    for (uint i=0;i<=n;i++){
        ASSERT_EQ(SA_TEST[i], SA[i]);
    }

    delete [] bwt;
    delete [] SA;
}


TEST(BWT_BUILDER_CHAR, BWT_CORRECT){
    char input[256] = "alabar_a_la_alabarda";
    uint BWT_TEST[21]={2,6,2,2,4,5,1,5,5,0,1,3,3,2,2,6,1,2,2,2,2};
    size_t n = 20;
    uint *SA;
    uint *bwt = BWTBuilder::buildBWT(input,n,SA,false);

    for (uint i=0;i<=n;i++){
        ASSERT_EQ(BWT_TEST[i], bwt[i]);
    }

    delete [] bwt;
    delete [] SA;
}

TEST(BWT_BUILDER_INTS, SA_CORRECT_INTS){

    uint input[21] = {2,5,2,3,2,6,1,2,1,5,2,1,2,5,2,3,2,6,4,2};
    uint SA_TEST[21]={20,6,11,8,19,10,7,2,14,0,12,4,16,3,15,18,9,1,13,5,17};
    size_t n = 20;
    uint *SA;
    uint *bwt = BWTBuilder::buildBWT(input,n,SA,false);

    for (uint i=0;i<=n;i++){
        ASSERT_EQ(SA_TEST[i], SA[i]);
    }

    delete [] bwt;
    delete [] SA;
}


TEST(BWT_BUILDER_INTS, BWT_CORRECT_INTS){
    uint input[21] = {2,5,2,3,2,6,1,2,1,5,2,1,2,5,2,3,2,6,4,2};
    uint BWT_TEST[21]={2,6,2,2,4,5,1,5,5,0,1,3,3,2,2,6,1,2,2,2,2};
    size_t n = 20;
    uint *SA;
    uint *bwt = BWTBuilder::buildBWT(input,n,SA,false);

    for (uint i=0;i<=n;i++){
        ASSERT_EQ(BWT_TEST[i], bwt[i]);
    }

    delete [] bwt;
    delete [] SA;
}