#include <gtest/gtest.h>
#include <text_indexes/fmi.h>
using namespace cds_static;

TEST(FMI,testFMI){
    //char input [20] = "alabar_a_la_alabarda$";
    uint in[21] = {2,5,2,3,2,6,1,2,1,5,2,1,2,5,2,3,2,6,4,2,0};

    SequenceBuilder *sb = new SequenceBuilderWaveletMatrix(new BitSequenceBuilderRRR(32),new MapperNone());
    fmi *_fmi = new fmi(in,21,sb,false);
    _fmi->build_index();

}