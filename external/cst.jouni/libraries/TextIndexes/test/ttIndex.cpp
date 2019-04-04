#include <gtest/gtest.h>
#include <sequence/Sequence.h>
#include <TextIndex.h>
#include <TextIndexRLFMI.h>
#include <rlfmi/TextIndexRLMIHelper.h>
#include <TextIndexHelper.h>
#include <TextIndexBuilder.h>

using namespace TextIndexes;

class TextIndexTest : public ::testing::Test {

protected:
    void SetUp(){
        n=21;
        input = new uchar[n+1];
        strcpy((char*)input,"alabar_a_la_alabarda0");
        input_sorted = {'0','_','_','_','a','a','a','a','a','a','a','a','a','b','b','d','l','l','l','r','r'};
        bwt  = {'a','r','a','a','d','l','_','l','l','0','_','b','b','a','a','r','_','a','a','a','a'};
        cbwt = {'a','r','a','d','l','_','l','0','_','b','a','r','_','a'};
        A    = {20,6,11,8,19,10,7,2,14,0,12,4,16,3,15,18,9,1,13,5,17};
        psi  = {9,6,10,16,0,2,3,13,14,17,18,19,20,11,12,4,5,7,8,1,15};
        inv_a ={9,17,7,13,11,19,1,6,3,16,5,2,10,18,8,14,12,20,15,4,0};
        vector<uint> iuc = {2,5,2,3,2,6,1,2,1,5,2,1,2,5,2,3,2,6,4,2,0};

        input_uint_cont = new uint[n];
        input_uint = new uint[n];

        for (uint i=0;i<n;i++){
            input_uint[i] = (uint)input[i];
        }

        max_v = 0;
        for (const auto it:iuc) {
            input_uint_cont[0] = it;
            max_v = max(max_v,it);
            input_uint_cont++;
        }
        input_uint_cont-=n;
        assert(input_uint_cont[0]==iuc[0]);
    }

    void TearDown(){
        delete [] input;
        delete [] input_uint;
        delete [] input_uint_cont;
    }


    bool internalTest(TextIndex *ti){
        for (auto i=0UL;i<n;i++){
            EXPECT_EQ(bwt[i],ti->getBWT(i));
        }
        for (auto i=0UL;i<n;i++){
            EXPECT_EQ(psi[i],ti->getPsi(i));
        }

        for (auto i=0UL;i<n;i++){
            EXPECT_EQ(A[i],ti->getSA(i));
        }
        for (auto i=0UL;i<20;i++){
            EXPECT_EQ(inv_a[i],ti->getISA(i));
        }
        for (auto i=0UL;i<n;i++){
            EXPECT_EQ(input_sorted[i],ti->getT(i));
        }
    }

    ulong n;
    vector<uchar> bwt;
    vector<uchar> cbwt;
    vector<uint> A;
    vector<uint> psi;
    vector<uint> inv_a;
    vector<uchar> input_sorted;
    vector<uint> LF;
    uchar *input;
    uint max_v;
    uint *input_uint;
    uint *input_uint_cont;
};

TEST_F(TextIndexTest,FMI){


    cds_static::SequenceBuilderWaveletMatrix sb(new cds_static::BitSequenceBuilderRRR(32),new cds_static::MapperNone());
    TextIndexes::TextIndex *ti = new TextIndexes::TextIndexFMI(input,n,&sb,4,4,false);

    for (ulong i=0;i<n;i++){
        EXPECT_EQ(bwt[i],ti->getBWT(i));
    }
    for (ulong i=0;i<n;i++){
        EXPECT_EQ(psi[i],ti->getPsi(i));
    }

    for (ulong i=0;i<n;i++){
        EXPECT_EQ(A[i],ti->getSA(i));
    }
    for (ulong i=0;i<n;i++){
        EXPECT_EQ(input_sorted[i],ti->getT(i));
    }
    for (ulong i=0;i<20;i++){
        EXPECT_EQ(inv_a[i],ti->getISA(i));
    }
    for (ulong i=0;i<n;i++){
        EXPECT_EQ(i,ti->getISA(ti->getSA(i)));
    }
    delete ti;
}

TEST_F(TextIndexTest,FMI_Builder){


    cds_static::SequenceBuilderWaveletMatrix sb(new cds_static::BitSequenceBuilderRRR(32),new cds_static::MapperNone());
    TextIndexes::TextIndexBuilder *tib = new TextIndexes::TextIndexBuilderFMI(&sb,4,4,false);

    TextIndexes::TextIndex *ti = tib->build(input,n);

    for (ulong i=0;i<n;i++){
        EXPECT_EQ(bwt[i],ti->getBWT(i));
    }
    for (ulong i=0;i<n;i++){
        EXPECT_EQ(psi[i],ti->getPsi(i));
    }

    for (ulong i=0;i<n;i++){
        EXPECT_EQ(A[i],ti->getSA(i));
    }
    for (ulong i=0;i<n;i++){
        EXPECT_EQ(input_sorted[i],ti->getT(i));
    }
    for (ulong i=0;i<20;i++){
        EXPECT_EQ(inv_a[i],ti->getISA(i));
    }
    for (ulong i=0;i<n;i++){
        EXPECT_EQ(i,ti->getISA(ti->getSA(i)));
    }
    delete ti;
    delete tib;
}

TEST_F(TextIndexTest,RLCSA){

    TextIndexes::TextIndex *ti = new TextIndexes::TextIndexRLCSA(input,(ulong)n,8,8,false,false);

    for (ulong i=1;i<n;i++){
        EXPECT_EQ(psi[i],ti->getPsi(i));
    }

    for (ulong i=0;i<n;i++){
        EXPECT_EQ(A[i],ti->getSA(i));
    }

    for (ulong i=0;i<n;i++){
        EXPECT_EQ(input_sorted[i],ti->getT(i));
    }

    delete ti;
}

TEST_F(TextIndexTest,RLCSA_Builder){

    TextIndexes::TextIndexBuilder *tib = new TextIndexes::TextIndexBuilderRLCSA(8,8,false,false);
    TextIndexes::TextIndex *ti = tib->build(input,n);

    for (ulong i=1;i<n;i++){
        EXPECT_EQ(psi[i],ti->getPsi(i));
    }

    for (ulong i=0;i<n;i++){
        EXPECT_EQ(A[i],ti->getSA(i));
    }

    for (ulong i=0;i<n;i++){
        EXPECT_EQ(input_sorted[i],ti->getT(i));
    }

    delete ti;
    delete tib;
}
TEST_F(TextIndexTest,MapToContHelper){

    vector<uint> dic;
    vector<uchar> invDic;
    TextIndexHelper::mapToCont(n,256,input_uint,dic,invDic);
    for (uint i=0;i<n;i++){
        EXPECT_EQ(input[i],invDic[input_uint[i]]);
    }
    for (uint i=0;i<n;i++){
        EXPECT_EQ(input_uint[i],dic[invDic[input_uint[i]]]);
    }
}

TEST_F(TextIndexTest,TextIndexHelper_buildsa){

    int *ia, *p;
    uint *a;
    TextIndexRLFMIHelper::build_sa(input_uint_cont, n, ia, a,p);

    for (ulong i=1UL;i<n;i++){
        EXPECT_EQ(ia[i],inv_a[i]);
    }

    for (ulong i=0UL;i<n;i++){
        EXPECT_EQ(a[i],A[i]);
    }

    for (ulong i=1UL;i<n;i++){
        EXPECT_EQ(p[i],psi[i]);
    }

    delete [] a;
    delete [] ia;
    delete [] p;

}


TEST_F(TextIndexTest,TextIndexHelper_build_bwt){
    cds_static::SequenceBuilderWaveletMatrix sb(new cds_static::BitSequenceBuilderRRR(33),new cds_static::MapperNone());
    cds_static::BitSequenceBuilderRRR bsb(33);

    vector<uint> dic;
    vector<uchar> invDic;
    TextIndexHelper::mapToCont(n,256,input_uint,dic,invDic);

    int *ia, *p;
    uint *a;
    TextIndexRLFMIHelper::build_sa(input_uint, n, ia, a,p);

    vector<uint> C;
    vector<uint> bwt_vector;
    TextIndexRLFMIHelper::build_BWT(input_uint,n,max_v,a,bwt_vector,C);
    for (uint i=0;i<n;i++){
        EXPECT_EQ(invDic[bwt_vector[i]],bwt[i]);
    }


    BitSequence *runs,*inv_runs;
    Sequence *compressed_bwt;
    TextIndexRLFMIHelper::build_RLBWT(bwt_vector, C, sb, bsb, compressed_bwt,runs, inv_runs);

    ASSERT_EQ(runs->rank1(runs->getLength()-1),cbwt.size());
    ASSERT_TRUE(runs->access(0));
    uint current;
    for (ulong i=0;i<runs->getLength();i++){
        if (runs->access(i)){
            current = compressed_bwt->access(runs->rank1(i)-1);
        }
        EXPECT_EQ(invDic[current],bwt[i]);
    }
    delete [] a;
    delete [] ia;
    delete [] p;
    delete runs;
    delete inv_runs;
    delete compressed_bwt;
}

TEST_F(TextIndexTest,RLFMI){
    cds_static::SequenceBuilderWaveletMatrix sb(new cds_static::BitSequenceBuilderRRR(32),new cds_static::MapperNone());
    cds_static:BitSequenceBuilderRRR bsb(33);
    uint samplepos = 5;
    uint sampleSuff = 5;
    TextIndexes::TextIndex *ti = new TextIndexes::TextIndexRLFMI(input,(ulong)n,&sb,&bsb,samplepos,sampleSuff,false);

    for (auto i=0UL;i<n;i++){
        EXPECT_EQ(bwt[i],ti->getBWT(i));
    }
    for (auto i=0UL;i<n;i++){
        EXPECT_EQ(psi[i],ti->getPsi(i));
    }

    for (auto i=0UL;i<n;i++){
        EXPECT_EQ(A[i],ti->getSA(i));
    }
        for (auto i=0UL;i<20;i++){
        EXPECT_EQ(inv_a[i],ti->getISA(i));
    }
    for (auto i=0UL;i<n;i++){
        EXPECT_EQ(input_sorted[i],ti->getT(i));
    }
//    for (auto i=0UL;i<20;i++){
//        EXPECT_EQ(inv_a[i],ti->getISA(i));
//    }
//    for (auto i=0UL;i<n;i++){
//        EXPECT_EQ(i,ti->getISA(ti->getSA(i)));
//    }
    delete ti;
}

TEST_F(TextIndexTest,RLFMI_LOAD_SAVE){
    cds_static::SequenceBuilderWaveletMatrix sb(new cds_static::BitSequenceBuilderRRR(32),new cds_static::MapperNone());
    cds_static:BitSequenceBuilderRRR bsb(33);
    uint samplepos = 5;
    uint sampleSuff = 5;
    TextIndexes::TextIndex *ti = new TextIndexes::TextIndexRLFMI(input,(ulong)n,&sb,&bsb,samplepos,sampleSuff,false);

    internalTest(ti);

    string filename = "tmp.bin";
    ti->save(const_cast<char*>(filename.c_str()));
    delete ti;
    ti = TextIndexes::TextIndex::load(const_cast<char*>(filename.c_str()));
    ASSERT_TRUE(ti!=nullptr);
    internalTest(ti);

    delete ti;
}


TEST_F(TextIndexTest,RLFMI_Builder){
    cds_static::SequenceBuilderWaveletMatrix sb(new cds_static::BitSequenceBuilderRRR(32),new cds_static::MapperNone());
    cds_static:BitSequenceBuilderRRR bsb(33);
    uint samplepos = 5;
    uint sampleSuff = 5;
    TextIndexes::TextIndexBuilder *tib = new TextIndexBuilderRLFMI(&sb,&bsb,samplepos,sampleSuff,false);
    TextIndexes::TextIndex *ti = tib->build(input,n);//= new TextIndexes::TextIndexRLFMI(input,(ulong)n,&sb,&bsb,samplepos,sampleSuff,false);

    for (auto i=0UL;i<n;i++){
        EXPECT_EQ(bwt[i],ti->getBWT(i));
    }
    for (auto i=0UL;i<n;i++){
        EXPECT_EQ(psi[i],ti->getPsi(i));
    }

    for (auto i=0UL;i<n;i++){
        EXPECT_EQ(A[i],ti->getSA(i));
    }
    for (auto i=0UL;i<20;i++){
        EXPECT_EQ(inv_a[i],ti->getISA(i));
    }
    for (auto i=0UL;i<n;i++){
        EXPECT_EQ(input_sorted[i],ti->getT(i));
    }
//    for (auto i=0UL;i<20;i++){
//        EXPECT_EQ(inv_a[i],ti->getISA(i));
//    }
//    for (auto i=0UL;i<n;i++){
//        EXPECT_EQ(i,ti->getISA(ti->getSA(i)));
//    }
    delete ti;
}