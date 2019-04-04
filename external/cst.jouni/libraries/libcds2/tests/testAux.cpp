#include <sequence/Sequence.h>
#include <utils/libcdsBasics.h>
#include <cassert>

using namespace cds_static;
using namespace cds_utils;

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

void testAccess(uint *input, Sequence *seq, size_t len){
    for (size_t i=0;i<len;i++){
        assert(seq->access(i)==input[i]);
    }
}

void testRank(uint *input, Sequence *seq, size_t len, uint sigma){
    uint *occs = new uint[sigma+1];
    for (uint i=0;i<=sigma;i++){
        occs[i]=0;
    }
    for (size_t i=0;i<len;i++){
        occs[input[i]]++;
        assert(seq->rank(input[i],i)==occs[input[i]]);
    }
}

void testSelect(uint *input, Sequence *seq, size_t len, uint sigma){
    uint *occs = new uint[sigma+1];
    for (uint i=0;i<=sigma;i++){
        occs[i]=0;
    }
    for (size_t i=0;i<len;i++){
        occs[input[i]]++;
        assert(seq->select(input[i],occs[input[i]])==i);
    }
}


int main(int argc, char **argv) {
    size_t len = 1000000;
    uint sigma = 10000;
    uint *input = generateSequence(len, sigma);
    uint sampleC = 6;
    uint sampleR = 0;
    uint ss_rate = 4;
    uint cut = 5;
    uint nFirstClasses = 2;
    SequenceBuilder *seqClasses = new SequenceBuilderSequenceRepairSN(new BitSequenceBuilderRRR(32),
            new MapperNone(), new PermutationBuilderMRRR(5, new BitSequenceBuilderRG(10)), sampleC, sampleR, ss_rate);

    SequenceBuilder *seqFirstClasses = new SequenceBuilderSequenceRepairSN(new BitSequenceBuilderRRR(32),
            new MapperNone(), new PermutationBuilderMRRR(5, new BitSequenceBuilderRG(10)), sampleC, sampleR, ss_rate);

    SequenceBuilder *seqOffsets = new SequenceBuilderGMR(new BitSequenceBuilderRG(32), new SequenceBuilderGMRChunk(new BitSequenceBuilderRG(32), new PermutationBuilderMRRR(32, new BitSequenceBuilderRG(32))));

    Sequence *seq = new SequenceAlphPartRep(input, len, cut, nFirstClasses, seqClasses, seqOffsets, seqFirstClasses, (SequenceBuilder*)nullptr, 1);

    testAccess(input, seq, len);
    testRank(input, seq, len, sigma);
    testSelect(input, seq, len, sigma);
    cerr << "Test OK!" << endl;
}

//int main(int argc, char **argv) {
//    size_t len = 1000000;
//    uint sigma = 10000;
//    uint *input = generateSequence(len, sigma);
//    uint sampleC = 6;
//    uint sampleR = 0;
//    uint ss_rate = 4;
//    uint cut = 5;
//    uint nFirstClasses = 2;
//    SequenceBuilder *seqClasses = new SequenceBuilderSequenceRepairSN(new BitSequenceBuilderRRR(32),
//            new MapperNone(), new PermutationBuilderMRRR(5, new BitSequenceBuilderRG(10)), sampleC, sampleR, ss_rate);
//
//    SequenceBuilder *seqFirstClasses = new SequenceBuilderSequenceRepairSN(new BitSequenceBuilderRRR(32),
//            new MapperNone(), new PermutationBuilderMRRR(5, new BitSequenceBuilderRG(10)), sampleC, sampleR, ss_rate);
//
//    SequenceBuilder *seqOffsets = new SequenceBuilderGMR(new BitSequenceBuilderRG(32), new SequenceBuilderGMRChunk(new BitSequenceBuilderRG(32), new PermutationBuilderMRRR(32, new BitSequenceBuilderRG(32))));
//
//    Sequence *seq = new SequenceAlphPartRep(input, len, cut, nFirstClasses, seqClasses, seqOffsets, seqFirstClasses);
//
//    testAccess(input, seq, len);
//    testRank(input, seq, len, sigma);
//    testSelect(input, seq, len, sigma);
//    cerr << "Test OK!" << endl;
//}