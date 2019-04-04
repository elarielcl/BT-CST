#include <gtest/gtest.h>
#include <utils/libcdsBasics.h>
#include <mapper/GenericMapper.h>

using namespace cds_static;

TEST(GenericMapper, SimpleMapper){

    uchar input[10] = {3,9,12,0,5,43,2,2,14,30};

    GenericMapper<uchar> gm;
    for (auto i=0;i<10;i++){
        ASSERT_EQ(gm.map(input[i]),input[i]);
    }

    for (auto i=0;i<10;i++){
        ASSERT_EQ(gm.unmap(input[i]),input[i]);
    }
}

TEST(GenericMapper, Mappercont){

    uchar input[10] = {3,9,12,0,5,43,2,2,14,30};
    uchar *map = new uchar[44];
    map[0]=1;
    map[2]=2;
    map[3]=3;
    map[5]=4;
    map[9]=5;
    map[12]=6;
    map[14]=7;
    map[30]=8;
    map[43]=9;
    GenericMapperCont<uchar> gm(input,(size_t)10,BitSequenceBuilderRG(32));
    for (auto i=0;i<10;i++){
        ASSERT_EQ(gm.map(input[i]),map[input[i]]);
        ASSERT_EQ(gm.unmap(gm.map(input[i])),input[i]);
    }
}

TEST(GenericMapper, MappercontVSGeneric){

    uint input[10] = {3,9,12,0,5,43,2,2,14,30};
    uint *map = new uint[44];
    map[0]=1;
    map[2]=2;
    map[3]=3;
    map[5]=4;
    map[9]=5;
    map[12]=6;
    map[14]=7;
    map[30]=8;
    map[43]=9;
    GenericMapperCont<uint> gm(input,(size_t)10,BitSequenceBuilderRG(32));
    for (auto i=0;i<10;i++){
        ASSERT_EQ(gm.map(input[i]),map[input[i]]);
        ASSERT_EQ(gm.unmap(gm.map(input[i])),input[i]);
    }

    MapperCont mc(input,10UL,BitSequenceBuilderRG(32));
    for (auto i=0;i<10;i++){
        ASSERT_EQ(mc.map(input[i]),gm.map(input[i]));
        uint m = mc.map(input[i]);
        ASSERT_EQ(mc.unmap(m),gm.unmap(m));
    }
}