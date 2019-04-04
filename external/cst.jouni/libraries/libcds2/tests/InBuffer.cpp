#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <utils/IfstreamBuffer.h>
#include <utils/OutBuffer.h>

using namespace std;
using namespace cds_utils;

ulong fileLength = 1000000;//of ints
char *filename;

void setUp(){
    filename = new char[1024];
    strcpy(filename,"/tmp/inputFileTest.data");
    vector<uint> data(fileLength);
    ulong i=0;
    for (auto &x:data){
        x = i++;
    }
    ofstream out(filename);
    out.write((char*)&data[0], fileLength*sizeof(uint));
    out.close();
}

void tearDown(){
    if (remove(filename)!=0){
        perror("Error deleting file");
    }
    delete [] filename;
}

TEST(InBuffer, AccessBeforeEndFirstBlock){
    setUp();
    ulong bufferSize = 1000;
    ulong blockSize = 100;
    string input(filename);
    IfstreamBuffer<uint> it(bufferSize,input,blockSize);
    ASSERT_EQ(it[fileLength-1], 999999u);
    ASSERT_EQ(it[0], 0u);
    for (ulong i=0;i<bufferSize;i++){
        ASSERT_EQ(*it,i);
        ASSERT_EQ(it[i],i);
        ++it;
    }
    tearDown();
}

TEST(InBuffer, Access2Blocks){
    setUp();

    ulong bufferSize = 1000;
    ulong blockSize = 100;
    string input(filename);
    IfstreamBuffer<uint> it(bufferSize,input,blockSize);
    AllInMemoryBuffer<uint> it2(input);
    ulong i;
    for (i=0;i<fileLength;i++){
        ASSERT_EQ(*it,i);
        ASSERT_EQ(*it2,i);
        ASSERT_EQ(it[i],i);
        ASSERT_EQ(it2[i],i);
        ++it;
        ++it2;
    }
    tearDown();
}

TEST(InBuffer, AccessOperator){
    setUp();

    ulong bufferSize = 1000;
    ulong blockSize = 100;
    string input(filename);
    IfstreamBuffer<uint> it(bufferSize,input,blockSize);
    AllInMemoryBuffer<uint> it2(input);
    ulong i;
    for (i=0;i<fileLength;i++){
        ASSERT_EQ(it[i],i);
        ASSERT_EQ(it2[i],i);
    }
    tearDown();
}

TEST(InBuffer, TwoEdges){
    setUp();

    ulong bufferSize = 1000;
    long blockSize = 100;
    string input(filename);
    IfstreamBuffer<uint> it(bufferSize,input,blockSize);
    AllInMemoryBuffer<uint> it2(input);
    long i;
    for (i=0;i<fileLength;i++){
        ASSERT_EQ(it2[i], it[i]);
        if (i-blockSize>=0){
            ASSERT_EQ(it2[i-blockSize], it[i-blockSize]);
        }
    }
    tearDown();
}
TEST(InBuffer, RandomAccess){
    setUp();

    ulong bufferSize = 1000;
    ulong blockSize = 100;
    string input(filename);
    IfstreamBuffer<uint> it(bufferSize,input,blockSize);
    AllInMemoryBuffer<uint> it2(input);
    ulong i;

    for (i=0;i<fileLength;i++){
        ulong position = (ulong)((rand()*1.0/ RAND_MAX)*(fileLength-1));
        ASSERT_LT(position,fileLength);
        ASSERT_EQ(it[position],position);
        ASSERT_EQ(it2[position],position);
    }
    tearDown();
}
TEST(InBuffer, EndOperator){
//    setUp();
//
//    ulong bufferSize = 1000;
//    ulong blockSize = 100;
//    string input(filename);
//    IfstreamBuffer<uint> it(bufferSize,input,blockSize);
//    AllInMemoryBuffer<uint> it2(input);
//    ifstream in(input);
//    in.seekg(0, ios_base::end);
//    auto len = in.tellg();
//    in.seekg(0,ios_base::beg);
//    in.close();
//    ulong i=0;
//    ulong nints = len/sizeof(uint);
//    ASSERT_EQ(len%sizeof(uint),0);
//    while (!it.end()){
//       ASSERT_TRUE(i<=nints);
//        ++it2;
//        ++it;
//        i++;
//    }
//    i=0;
//    while (!it.end()){
//
//        ASSERT_TRUE(i<=nints);
//        it[i];
//        it2[i];
//        i++;
//    }
//    tearDown();
}

TEST(InBuffer, ReverseBuffer){
//    setUp();
//
//    ulong bufferSize = 1000;
//    ulong blockSize = 100;
//    string input(filename);
//    IfstreamBuffer<uint> it(bufferSize,input,blockSize);
//    ifstream in(input);
//    in.seekg(0, ios_base::end);
//    auto len = in.tellg();
//    in.seekg(0,ios_base::beg);
//    in.close();
//    ulong i=0;
//    ulong nints = len/sizeof(uint);
//    ASSERT_EQ(len%sizeof(uint),0);
//    while (!it.end()){
//        ASSERT_TRUE(i<=nints);
//        ++it;
//        i++;
//    }
//    i--;
//    --it;
//    while(!it.atBegin()){
//        if(*it!=i){
//            cerr << *it << endl;
//        }
//        ASSERT_EQ(*it, i);
//        i--;
//        --it;
//    }
//
//    tearDown();
}

TEST(InBuffer, ZigZagRead){
//    setUp();
//
//    ulong bufferSize = 1000;
//    ulong blockSize = 100;
//    string input(filename);
//    IfstreamBuffer<uint> it(bufferSize,input,blockSize);
//    ifstream in(input);
//    in.seekg(0, ios_base::end);
//    auto len = in.tellg();
//    in.seekg(0,ios_base::beg);
//    in.close();
//    long i=0;
//    ulong nints = len/sizeof(uint);
//    ASSERT_EQ(len%sizeof(uint),0);
//    while (!it.end()){
//        ASSERT_TRUE(i<=nints);
//        ++it;
//        i++;
//    }
//    i--;
//    --it;
//    //TODO
////    while(!it.atBegin()){
//        while(i>=0){
//            if(*it!=i){
//                cerr << *it << endl;
//            }
//            ASSERT_EQ(*it, i);
//            i--;
//            --it;
//        }
//    }
//    ASSERT_EQ(i,0);
//    for (uint j=0;j<1000;j++) {
//        while (i < nints / 2 + j) {
//            ASSERT_EQ(*it, i);
//            ++it;
//            i++;
//        }
//        while (i >= 0) {
//            if (i == 0)
//                cerr << endl;
//            ASSERT_EQ(*it, i);
//            --it;
//            i--;
//        }
//    }
//
//    for (int j=-1;j<1000;j++) {
//        while (i < bufferSize-blockSize + j) {
//            ASSERT_EQ(*it, i);
//            ++it;
//            i++;
//        }
//        while (i >= 0) {
//            if (i == 0)
//                cerr << endl;
//            ASSERT_EQ(*it, i);
//            --it;
//            i--;
//        }
//    }
//    tearDown();
}
TEST(InBuffer, Restart){
    setUp();

    ulong bufferSize = 1000;
    ulong blockSize = 100;
    string input(filename);
    IfstreamBuffer<uint> it(bufferSize,input,blockSize);
    AllInMemoryBuffer<uint> it2(input);
    ifstream in(input);
    in.seekg(0, ios_base::end);
    auto len = in.tellg();
    in.seekg(0,ios_base::beg);
    in.close();
    ulong i=0;
    ulong nints = len/sizeof(uint);
    ASSERT_EQ(len%sizeof(uint),0);
    while (!it.end()){
        ASSERT_TRUE(i<=nints);
        ASSERT_EQ(*it,*it2);
        ++it;
        ++it2;
        i++;
    }
    ASSERT_EQ(it.end(),it2.end());
    it.restart();
    it2.restart();
    ASSERT_EQ(it.end(), false);
    it[nints-1];
    it2[nints-1];
    ++it;
    ++it2;
    ASSERT_EQ(it.end(), true);
    ASSERT_EQ(it2.end(), true);
    tearDown();
}



TEST(OutBuffer, Write){
    char b[L_tmpnam];
    tmpnam(b);
    string outFile(b);
    long bufferSize = 1000;
    OutBuffer<uint> out(bufferSize,outFile);
    auto len=bufferSize*10;
    for (uint i=0;i<len;i++){
        out.push_back(i);
    }
    out.flush();
    ifstream in(outFile,ios_base::binary);
    in.seekg(0, ios_base::end);
    auto fileLen = in.tellg();
    in.seekg(0,ios_base::beg);
    ASSERT_EQ((ulong)0, (ulong)fileLen%sizeof(uint));
    ASSERT_EQ((ulong)len,(ulong)fileLen/sizeof(uint));
    uint *read = new uint[len];
    in.read((char*)read,fileLen);
    for (uint i=0;i<len;i++){
        ASSERT_EQ(read[i],i);
    }
    delete [] read;
    in.close();
}


