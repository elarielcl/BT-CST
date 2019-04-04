#include<gtest/gtest.h>

#include <cstdlib>
#include <vector>
#include <utils/SparseSuffixSort.h>

using namespace cds_utils;

TEST(SSS, SortEmpty){
//    ulong len = 5;
//    vector<int> input = {1,3,5,6,7};
//    vector<CellSort> p;
//
//    SparseSuffixSort<int,vector<int>> s(input,len);
//    vector<CellSort> pPrev = p;
//    s.sparseStringSort(p);
//    ASSERT_EQ(p.size(), pPrev.size());
//    ASSERT_EQ(p.size(), 0);
}

//TEST(SSS, SortSingle){
//    ulong len = 5;
//    vector<int> input = {1,3,5,6,7};
//    vector<CellSort> p;
//
//    p.push_back(CellSort(1UL, 1UL));
//    SparseSuffixSort<int,vector<int>> s(input,len);
//    vector<CellSort> pPrev = p;
//    s.sparseStringSort(p);
//    ASSERT_EQ(p.size(), pPrev.size());
//    ASSERT_EQ(p.size(), 1);
//    ASSERT_EQ(p[0].ini, pPrev[0].ini);
//}
//
//TEST(SSS, SortTwoSingleAlreadySorted){
//    ulong len = 5;
//    vector<int> input = {1,3,5,6,7};
//    vector<CellSort> p;
//    vector<CellSort> p2;
//    p.push_back(CellSort(1UL, 1UL));
//    p.push_back(CellSort(2UL, 2UL));
//    SparseSuffixSort<int,vector<int>> s(input,len);
//    vector<CellSort> pPrev = p;
//    s.sparseStringSort(p);
//    ASSERT_EQ(p.size(), pPrev.size());
//    ASSERT_EQ(p.size(), 2);
//    ASSERT_EQ(p[0].ini, pPrev[0].ini);
//    ASSERT_EQ(p[1].ini, pPrev[1].ini);
//}
//
//TEST(SSS, SortTwoSingleNotYetSorted){
//    ulong len = 5;
//    vector<int> input = {1,3,5,6,7};
//    vector<CellSort> p;
//    vector<CellSort> p2;
//    p.push_back(CellSort(2UL, 2UL));
//    p.push_back(CellSort(1UL, 1UL));
//    SparseSuffixSort<int,vector<int>> s(input,len);
//    vector<CellSort> pPrev = p;
//    s.sparseStringSort(p);
//    ASSERT_EQ(p.size(), pPrev.size());
//    ASSERT_EQ(p.size(), 2);
//    ASSERT_EQ(p[0].ini, pPrev[1].ini);
//    ASSERT_EQ(p[1].ini, pPrev[0].ini);
//    ASSERT_LE(input[p[0].ini], input[pPrev[1].end]);
//}
//
//
//TEST(SSS, SortLength2AlreadySorted){
//    ulong len = 5;
//    vector<int> input = {1,3,5,6,7};
//    vector<CellSort> p;
//    vector<CellSort> p2;
//    p.push_back(CellSort(0UL, 1UL));
//    p.push_back(CellSort(2UL, 3UL));
//    SparseSuffixSort<int,vector<int>> s(input,len);
//    vector<CellSort> pPrev = p;
//    s.sparseStringSort(p);
//    ASSERT_EQ(p.size(), pPrev.size());
//    ASSERT_EQ(p.size(), 2);
//    for (uint i=0;i<2;i++)
//            ASSERT_EQ(input[p[0].ini], input[pPrev[0].ini]);
//    for (uint i=0;i<2-1;i++)
//            ASSERT_LE(input[p[i].ini], input[p[i+1].ini]);
//}
//
//TEST(SSS, SortLength2AlreadySortedBackward){
//    ulong len = 5;
//    vector<int> input = {1,3,5,6,7};
//    vector<CellSort> p;
//    vector<CellSort> p2;
//    p.push_back(CellSort(3UL, 2UL));
//    p.push_back(CellSort(1UL, 0UL));
//    SparseSuffixSort<int,vector<int>> s(input,len);
//    vector<CellSort> pPrev = p;
//    s.sparseStringSort(p);
//    ASSERT_EQ(p.size(), pPrev.size());
//    ASSERT_EQ(p.size(), 2);
//    for (uint i=0;i<2-1;i++)
//            ASSERT_LE(input[p[i].ini], input[p[i+1].ini]);
//
//}
//
//TEST(SSS, SortLengthAllForward){
//    ulong len = 5;
//    vector<int> input = {1,6,5,3,7};
//    vector<CellSort> p;
//    vector<CellSort> p2;
//    for (ulong i=0;i<len;i++)
//        p.push_back(CellSort(i, i));
//    SparseSuffixSort<int,vector<int>> s(input,len);
//    vector<CellSort> pPrev = p;
//    s.sparseStringSort(p);
//    ASSERT_EQ(p.size(), pPrev.size());
//    ASSERT_EQ(p.size(), 5);
//    for (uint i=0;i<len-1;i++)
//            ASSERT_LE(input[p[i].ini], input[p[i+1].ini]);
//
//}
//
//TEST(SSS, SortLengthLen2AllForward){
//    ulong len = 6;
//    vector<int> input = {1,6,3,4,3,4,2};
//    vector<CellSort> p;
//    vector<CellSort> p2;
//    for (ulong i=0;i<len/2;i++)
//        p.push_back(CellSort(i*2, i*2+1));
//    SparseSuffixSort<int,vector<int>> s(input,len);
//    vector<CellSort> pPrev = p;
//    s.sparseStringSort(p);
//    ASSERT_EQ(p.size(), pPrev.size());
//    ASSERT_EQ(p.size(), len/2);
//    for (uint i=0;i<len/2-1;i++)
//            ASSERT_LE(input[p[i].ini], input[p[i+1].ini]);
//
//}
//
//
//TEST(SSS, SortLengthLenIrregularSegments){
//    ulong len = 12;
//    vector<int> input = {1,6,3,4,3,4,2,5,2,10,8,0};
//    vector<CellSort> p;
//    vector<CellSort> p2;
//    p.push_back(CellSort(3, 7));
//    p.push_back(CellSort(1,2));
//    p.push_back(CellSort(7,1));
//    p.push_back(CellSort(10,10));
//    p.push_back(CellSort(11,8));
//    SparseSuffixSort<int,vector<int>> s(input,len);
//    vector<CellSort> pPrev = p;
//    s.sparseStringSort(p);
//    ASSERT_EQ(p.size(), pPrev.size());
//    for (uint i=0;i<p.size()-1;i++)
//            ASSERT_LE(input[p[i].ini], input[p[i+1].ini]);
//
//}
//
//
//TEST(SSS, SparseSS){
//    ulong len = 12;
//    vector<int> input = {3,0,2,1,4};
//    vector<CellSort> p;
//    vector<CellSort> p2;
//    p.push_back(CellSort(0,0,0));
//    p.push_back(CellSort(1,1,1));
//    p.push_back(CellSort(2,2,2));
//    p.push_back(CellSort(3,3,3));
//    p.push_back(CellSort(4,4,4));
//    SparseSuffixSort<int,vector<int>> s(input,len);
//    vector<CellSort> pPrev = p;
//    s.sparseStringSort(p);
//    vector<long> sa;
//    s.sparseSuffixSort(p,sa);
//
//}
//
//TEST(SSS, SparseALABAR){
//
//    vector<int> input = {1,3,1,2,1,4};
//    ulong len = input.size();
//    vector<CellSort> p;
//    vector<CellSort> p2;
//    for (ulong i=0;i<len;i++)
//        p.push_back(CellSort(i,i,(long)i));
//
//    SparseSuffixSort<int,vector<int>> s(input,len);
//    vector<CellSort> pPrev = p;
//    s.sparseStringSort(p);
////    ASSERT_EQ(p[0].ini, 6);
////    ASSERT_TRUE(p[1].ini==0 ||p[1].ini==2 ||p[1].ini==4);
////    ASSERT_TRUE(p[2].ini==0 ||p[2].ini==2 ||p[2].ini==4);
////    ASSERT_TRUE(p[3].ini==0 ||p[3].ini==2 ||p[3].ini==4);
////    ASSERT_EQ(p[4].ini, 3);
////    ASSERT_EQ(p[5].ini, 1);
////    ASSERT_EQ(p[6].ini, 5);
//    vector<long> sa;
//    s.sparseSuffixSort(p,sa);
//    vector<long> sa_res = {6L,2L,0L,4L,3L,1L,5L};
//    ASSERT_EQ(sa.size(), sa_res.size());
//    for (auto iter1=sa_res.begin(),iter2=sa.begin();iter1!=sa_res.end();++iter1,++iter2){
//        ASSERT_EQ(*iter1, *iter2);
//    }
//}
//
//TEST(SSS, SparseALABAR_CHAR){
//
//    vector<unsigned char> input = {'1','3','1','2','1','4'};
//    ulong len = input.size();
//    vector<CellSort> p;
//    vector<CellSort> p2;
//    for (ulong i=0;i<len;i++)
//        p.push_back(CellSort(i,i,(long)i));
//
//    SparseSuffixSort<unsigned char,vector<unsigned char>> s(input,len);
//    vector<CellSort> pPrev = p;
//    s.sparseStringSort(p);
////    ASSERT_EQ(p[0].ini, 6);
////    ASSERT_TRUE(p[1].ini==0 ||p[1].ini==2 ||p[1].ini==4);
////    ASSERT_TRUE(p[2].ini==0 ||p[2].ini==2 ||p[2].ini==4);
////    ASSERT_TRUE(p[3].ini==0 ||p[3].ini==2 ||p[3].ini==4);
////    ASSERT_EQ(p[4].ini, 3);
////    ASSERT_EQ(p[5].ini, 1);
////    ASSERT_EQ(p[6].ini, 5);
//    vector<long> sa;
//    s.sparseSuffixSort(p,sa);
//    vector<long> sa_res = {6L,2L,0L,4L,3L,1L,5L};
//    ASSERT_EQ(sa.size(), sa_res.size());
//    for (auto iter1=sa_res.begin(),iter2=sa.begin();iter1!=sa_res.end();++iter1,++iter2){
//        ASSERT_EQ(*iter1, *iter2);
//    }
//}