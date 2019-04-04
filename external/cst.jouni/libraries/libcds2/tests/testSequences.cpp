#include <gtest/gtest.h>
#include <sequence/Sequence.h>
#include <utils/libcdsBasics.h>
#include <utils/timeMeasure.h>
//#include <sequence/MultiAryCoders.h>
#include <sequence/MultiArityWTHelper.h>

using namespace cds_static;
using namespace cds_utils;

class MultiAryWTTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        len = 30;
        max_v = 13;
        seq = new uint[len];
        for (uint i=0;i<len;i++)
            seq[i] = i%(max_v+1);

        seq_large = new uint[len_large];
        max_v_large = 1015;
        for (uint i=0;i<len_large;i++)
            seq_large[i] = static_cast<uint>(static_cast<float>(rand())/RAND_MAX *max_v_large);
        MapperCont mc(seq_large,len_large,BitSequenceBuilderRG(32));
        for (uint i=0;i<len_large;i++)
            seq_large[i] = mc.map(seq_large[i])-1;
    }

    virtual void TearDown() {
        delete [] seq;
        delete [] seq_large;
    }

    uint *seq;
    size_t len;
    uint max_v;
    uint max_v_large;
    uint *seq_large;
    size_t len_large;
};

void saveLoadSequence(Sequence *&seq){
    ofstream fp("tmp.bin");
    seq->save(fp);
    fp.close();
    delete seq;
    ifstream in("tmp.bin");
    seq=Sequence::load(in);
    in.close();
}

template <typename T, class Leaves> void saveLoadBG(BlockGraph<T,Leaves> *&seq){
    ofstream fp("tmp.bin");
    seq->save(fp);
    fp.close();
    delete seq;
    ifstream in("tmp.bin");
    seq=BlockGraph<T,Leaves>::load(in);
    in.close();
}
uint *loadSequence(const char *filename, long &len){
    ifstream in(filename,ios_base::binary|ios_base::in);
    in.seekg(0, ios_base::end);
    len = in.tellg();
    in.seekg(0, ios_base::beg);
    char *input = new char[len];
    in.read(input, len);
    in.close();
    uint *inp = new uint[len];
    for (auto i=0;i<len;i++){
        inp[i]=(uint)input[i];
    }
    delete [] input;
    MapperCont mp(inp,(size_t)len,BitSequenceBuilderRG(32));
    for (auto i=0;i<len;i++){
        inp[i] = mp.map(inp[i])-1;
    }

    return inp;
}

template<typename T> T *loadSequenceNoMap(const char *filename, long &len){
    ifstream in(filename,ios_base::binary|ios_base::in);
    in.seekg(0, ios_base::end);
    len = in.tellg();
    in.seekg(0, ios_base::beg);
    assert(len%sizeof(T)==0);
    len = len / sizeof(T);
    T *input = new T[len];
    in.read((char*)input, len*sizeof(T));
    in.close();
    return input;
}

uint *generateSequence(size_t len, uint sigma){

    srand(0);
    uint *seq = new uint[len];
    for (size_t i=0;i<=sigma;i++){
        seq[i]=i;
    }
    for (size_t i=sigma+1;i<len;i++){
        seq[i]=rand()*1.0/ RAND_MAX * sigma;
    }
    return seq;
}

template<typename T, class Leaves = Sequence > void testAccess(T *input, Leaves &seq, size_t len){
    for (size_t i=0;i<len;i++){
        ASSERT_EQ(seq.access(i),input[i]);
    }
}
template<typename T, typename Leaves> void testAccessBG(T *input, BlockGraph<T,Leaves> &seq, size_t len){
    for (size_t i=0;i<len;i++){
        ASSERT_EQ(seq[i],input[i]);
    }
}

template<typename T>  void testRank(T *input, Sequence &seq, size_t len){
    vector<ulong> occs(256,0);
    for (size_t i=0;i<len;i++){
        occs[input[i]]++;
        seq.rank(input[i],i);
        ASSERT_EQ(seq.rank(input[i],i),occs[input[i]]);
    }
}

template<typename T>  void testRankBG(T *input, SequenceBlockGraph<T,Sequence> &seq, size_t len){
    vector<ulong> occs(256,0);
    for (size_t i=0;i<len;i++){
        occs[input[i]]++;
        seq.rank(input[i],i);
        ASSERT_EQ(seq.rank(input[i],i),occs[input[i]]);
    }
}

template<typename T> void testSelect(T *input, Sequence &seq, size_t len){

    vector<ulong> occs(len,0);
    for (size_t i=0;i<len;i++){
        occs[input[i]]++;
        ASSERT_EQ(seq.select(input[i],occs[input[i]]),i);
    }
}

TEST(Sequence, DISABLED_WaveletMatrix){

    size_t len = 100000;
    uint sigma = 2048;
    uint *input = generateSequence(len,sigma);
    WaveletMatrix seq(input,len,new BitSequenceBuilderRG(32), new MapperNone(),false);
    testAccess<uint,Sequence>(input,seq,len);
//    testRank<uint>(input, seq, len);
    testSelect<uint>(input,seq, len);
}

TEST_F(MultiAryWTTest, MultiAriWT_fixed_length_build){
    uint chunk_length = 2;

    MultiArityCoder *coder = new MultiArityCoderFixed(chunk_length,max_v);

    ASSERT_EQ(coder->get_max_level(0),2);

    SequenceBuilder *seq_builder = new SequenceBuilderWaveletMatrix(new BitSequenceBuilderRRR(32), new MapperNone());
    Sequence *wt = new MultiArityWT(seq,len,coder,seq_builder);
    MultiArityWTHelper *helper = new MultiArityWTHelper();
    EXPECT_EQ(helper->getDepth(*(const MultiArityWT*)wt),2);

    vector<uint> node_id;
    node_id.push_back(0);
    const Sequence *seq_node_internal = helper->getSequence(*(const MultiArityWT*)wt,node_id);
    for (uint i=0;i<len;i++){
        EXPECT_EQ(seq_node_internal->access(i),seq[i]&3U);
    }

    const vector<MultiArityNode*> *ch = helper->getChildren(*(const MultiArityWT*)wt,node_id);
    ASSERT_EQ(ch->size(),4);
    for (const auto &it: (*ch)){
        ASSERT_TRUE(dynamic_cast<MultiArityInternalNode*>(it)!=nullptr);
    }
    node_id.push_back(0);

    ch = helper->getChildren(*(const MultiArityWT*)wt,node_id);
    ASSERT_EQ(ch->size(),4);
    for (const auto &it: (*ch)){
        ASSERT_TRUE(dynamic_cast<MultiArityLeafNode*>(it)!=nullptr);
    }
    node_id[1]=1;
    ch = helper->getChildren(*(const MultiArityWT*)wt,node_id);
    ASSERT_EQ(ch->size(),4);
    node_id[1]=2;
    ch = helper->getChildren(*(const MultiArityWT*)wt,node_id);
    ASSERT_EQ(ch->size(),4);
    node_id[1]=3;
    ch = helper->getChildren(*(const MultiArityWT*)wt,node_id);
    ASSERT_EQ(ch->size(),4);

    //access
    testAccess<uint>(seq,*(Sequence*)wt,len);
    //rank
    testRank<uint>(seq,*(Sequence*)wt,len);
    //select
    testSelect<uint>(seq,*(Sequence*)wt,len);
}


TEST_F(MultiAryWTTest, MultiAriWT_fixed_length_large_seq){
    uint chunk_length = 4;

    MultiArityCoder *coder = new MultiArityCoderFixed(chunk_length,max_v_large);

    SequenceBuilder *seq_builder = new SequenceBuilderWaveletMatrix(new BitSequenceBuilderRRR(32), new MapperNone());
    Sequence *wt = new MultiArityWT(seq_large,len_large,coder,seq_builder);
    MultiArityWTHelper *helper = new MultiArityWTHelper();
    EXPECT_EQ(helper->getDepth(*(const MultiArityWT*)wt),2);

    //access
    testAccess<uint>(seq_large,*(Sequence*)wt,len_large);
    //rank
    testRank<uint>(seq_large,*(Sequence*)wt,len_large);
    //select
    testSelect<uint>(seq_large,*(Sequence*)wt,len_large);
}
//TEST(BlockGraph, DISABLED_IndexBlockGraphInfluenza1MBUchar){
//    long len;
//    uchar *input;
//    string inFile = "/home/alberto/research/data/influenza.1MB";
//    input = loadSequenceNoMap<uchar>(inFile.c_str(), len);
//    uint nLevels, arity;
//    nLevels = 4;
//    arity = 32;
//    uint bl = 4;
//    // StrategySplitBlock *st = new StrategySplitBlockTopDown(nLevels,arity);
//    StrategySplitBlock *st = new StrategySplitBlockBottomUp(nLevels,bl);
//    BlockGraphIndex<uchar,Sequence> *bg = new BlockGraphIndex<uchar,Sequence>(st);
//    bg->build(inFile,false);
//    delete bg;
//    //cerr << bg->getSize()*8.0 / bg->getLenght() << endl;
//    //long nqueries = min(len,1000000l);
////    testAccessBG<uchar>(input,*bg,nqueries);
//    delete [] input;
//}
//
//
//TEST(BlockGraph, DISABLED_FMIndexBlockGraphInfluenza1MBUchar){
//    long len;
//    uchar *input;
//    string inFile = "/home/alberto/research/data/influenza.1MB";
//    input = loadSequenceNoMap<uchar>(inFile.c_str(), len);
//    uint nLevels, arity;
//    nLevels = 2;
//    arity = 32;
//    uint bl = 16;
//    // StrategySplitBlock *st = new StrategySplitBlockTopDown(nLevels,arity);
//    StrategySplitBlock *st = new StrategySplitBlockBottomUp(nLevels,bl);
//    BlockGraphIndex<uchar,Sequence> *bg = new BlockGraphIndex<uchar,Sequence>(st);
//    string outFileName = "/home/alberto/research/data/influenza.1MB.kernels";
//    bg->setSaveKernels(outFileName);
//    bg->build(inFile,false);
//    //cerr << bg->getSize()*8.0 / bg->getLenght() << endl;
//    //long nqueries = min(len,1000000l);
////    testAccessBG<uchar>(input,*bg,nqueries);
//    //delete [] input;
//}
//
//TEST(BlockGraph, BlockGraphInfluenza1MBUchar){
//    long len;
//    uchar *input;
//    string inFile = "/home/alberto/research/data/influenza.1MB";
//    input = loadSequenceNoMap<uchar>(inFile.c_str(), len);
//    uint nLevels, arity;
//    nLevels = 4;
//    arity = 32;
//    uint bl = 8;
//   // StrategySplitBlock *st = new StrategySplitBlockTopDown(nLevels,arity);
//    StrategySplitBlock *st = new StrategySplitBlockBottomUp(nLevels,bl);
//    BlockGraph<uchar,DirectAccess> *bg = new BlockGraph<uchar,DirectAccess>(st);
//    bg->build(inFile,false);
//    cerr << bg->getSize()*8.0 / bg->getLenght() << endl;
//    long nqueries = min(len,1000000l);
//    testAccessBG<uchar, DirectAccess>(input,*bg,nqueries);
//    delete [] input;
//}
//
//TEST(BlockGraph, SequenceBlockGraphTemplateUchar){
//    long len;
//    uint *input;
//    uint sigma=-1;
//    string inFile = "/home/alberto/research/data/influenza.1MB.bin";
//    input = loadSequenceNoMap<uint>(inFile.c_str(), len);
//    uint nLevels, arity, bl;
//    nLevels = 4;
//    arity = 32;
//    bl = 4;
//    StrategySplitBlock *st = new StrategySplitBlockBottomUp(nLevels,bl);
//    SequenceBlockGraph<uint,Sequence> *bg = new SequenceBlockGraph<uint,Sequence>(st);
//    bg->build(inFile,false);
//    long nqueries = min(len,1000000l);
//
//    testAccess<uint>(input,*((Sequence*)bg),nqueries);
//    cerr << "Access OK!"<< endl;
//
//    testRank<uint>(input, *((Sequence*)bg), nqueries);
//    cerr << "Rank OK!"<< endl;
//    testSelect<uint>(input, *((Sequence*)bg), nqueries);
//    cerr << "Select OK!"<< endl;
//
//
//    delete [] input;
//}

TEST(Sequence, DISABLED_AlphPartRepRec){

//    size_t len = 1000000;
//    uint sigma = 10000;
//    uint *input = generateSequence(len,sigma);
//    uint sampleC = 6;
//    uint sampleR = 0;
//    uint ss_rate = 4;
//    uint cut = 5;
//    uint nFirstClasses = 2;
//    SequenceBuilder *seqClasses = new SequenceBuilderSequenceRepairSN(new BitSequenceBuilderRRR(32),
//            new MapperNone(),new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)),sampleC,sampleR,ss_rate);
//    SequenceBuilder *seqClasses2 = new SequenceBuilderWaveletMatrix(new BitSequenceBuilderRG(32), new MapperNone());
//
//    SequenceBuilder *sc[2];
//    sc[0] = seqClasses;
//    sc[1] = seqClasses2;
//
//    SequenceBuilder *seqFirstClasses = new SequenceBuilderSequenceRepairSN(new BitSequenceBuilderRRR(32),
//            new MapperNone(),new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)),sampleC,sampleR,ss_rate);
//    SequenceBuilder *seqFirstClasses2 = new SequenceBuilderWaveletMatrix(new BitSequenceBuilderRG(32), new MapperNone());
//
//    SequenceBuilder *sfc[2];
//    sfc[0]=seqFirstClasses;
//    sfc[1]=seqFirstClasses;
//
//    SequenceBuilder *seqOffsets = new SequenceBuilderGMR(new BitSequenceBuilderRG(32),new SequenceBuilderGMRChunk(new BitSequenceBuilderRG(32), new PermutationBuilderMRRR(32,new BitSequenceBuilderRG(32))));
//    SequenceBuilder *so[2];
//    so[0] = seqOffsets;
//    so[1] = seqOffsets;
//
//    Sequence *seq = new SequenceAlphPartRep(input,len, cut,nFirstClasses,
//                sc,so,sfc,seqFirstClasses,seqClasses2,10);

//    testAccess(input,seq,len);
//    testRank(input, seq, len, sigma);
//    testSelect(input, seq, len, sigma);
}