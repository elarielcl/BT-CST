cmake_minimum_required(VERSION 2.8.4)
project(libcds2)

# Project options
set(PROJECT_VENDOR "Alberto Ordóñez")
set(PROJECT_CONTACT "alberto.ordonez@udc.es")
set(PROJECT_DESCRIPTION "libcds y derivados")
set(CMAKE_PROJECT_NAME "libcds in libcds")
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_INSTALL_PREFIX /home/alberto/software)
set(CMAKE_COMPILER_IS_GNUCXX)

# Flags
set(CMAKE_CXX_FLAGS " -Wall -m64 -Wunused-variable")
set(CMAKE_CXX_FLAGS_RELEASE " -std=c++0x -Wall -O9 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG " -g3 -DDEBUG")
#set(CMAKE_CXX_FLAGS_DEBUG " -g3 -DNDEBUG")


# Suffix Arrays library
# add_subdirectory(external/libdivsufsort-2.0.1/)
# include_directories(external/libdivsufsort-2.0.1/include)

# Sources
include_directories(includes/)
#include_directories(../external/intSA/includes/)

set (SOURCES
"src/utils/Array.cpp"
"src/utils/BitString.cpp"
"src/utils/timing.cpp"
"src/utils/Tuples.cpp"
"src/utils/StringUtils.cpp"
"src/utils/cppUtils.cpp"
"src/static/mapper/Mapper.cpp"
"src/static/mapper/MapperNone.cpp"
"src/static/mapper/MapperCont.cpp"
"src/static/mapper/MapperRev.cpp"
"src/static/bitsequence/BitSequence.cpp"
 "src/static/bitsequence/BitSequenceRG.cpp"
 "src/static/bitsequence/BitSequenceSDArray.cpp"
 "src/static/bitsequence/sdarraySadakane.cpp"
 "src/static/bitsequence/BitSequenceDArray.cpp"
 "src/static/bitsequence/BitSequenceRRR.cpp"
 "src/static/bitsequence/BitSequenceBuilderRG.cpp"
 "src/static/bitsequence/BitSequenceBuilderRRR.cpp"
 "src/static/bitsequence/BitSequenceBuilderSDArray.cpp"
 "src/static/bitsequence/BitSequenceBuilderDArray.cpp"
 "src/static/bitsequence/TableOffsetRRR.cpp"
 "src/static/bitsequence/BitSequencePlain.cpp"
 "src/static/bitsequence/BitSequenceBuilderPlain.cpp"
 "src/static/bitsequence/BitSequenceBRW32.cpp"
"src/static/bitsequence/BitSequenceDelta.cpp"
"src/static/sequence/MultiArityWT.cpp"
"src/static/sequence/MultiArityNode.cpp"
"src/static/sequence/MultiArityInternalNode.cpp"
"src/static/sequence/MultiArityLeafNode.cpp"
"src/static/sequence/MultiArityCoderFixed.cpp"
"src/static/sequence/MultiArityWTHelper.cpp"
"src/static/coders/huff.cpp"
"src/static/coders/HuffmanCoder.cpp"
"src/static/sequence/Sequence.cpp"
"src/static/sequence/BitmapsSequence.cpp"
"src/static/sequence/wt_coder.cpp"
"src/static/sequence/wt_coder_binary.cpp"
"src/static/sequence/wt_coder_huff.cpp"
"src/static/sequence/wt_coder_huff_can.cpp"
"src/static/sequence/wt_node.cpp"
"src/static/sequence/wt_node_internal.cpp"
"src/static/sequence/wt_node_leaf.cpp"
"src/static/sequence/WaveletTree.cpp"
"src/static/sequence/WaveletTreeNoptrs.cpp"
"src/static/sequence/WaveletMatrix.cpp"
"src/static/sequence/SequenceGMR.cpp"
"src/static/sequence/SequenceGMRChunk.cpp"
"src/static/sequence/SequenceBuilderWaveletTreeNoptrs.cpp"
"src/static/sequence/SequenceBuilderWaveletTree.cpp"
"src/static/sequence/SequenceBuilderGMR.cpp"
"src/static/sequence/SequenceBuilderGMRChunk.cpp"
"src/static/sequence/SequenceBuilderStr.cpp"
"src/static/sequence/SequenceBuilderWaveletMatrix.cpp"
"src/static/sequence/Tuple.cpp"
"src/static/sequence/CanonicalWTNoptrs.cpp"
"src/static/sequence/SequenceBuilderCanonicalWTNoptrs.cpp"
"src/static/sequence/MappingSymbols.cpp"
"src/static/sequence/MappingTable.cpp"
"src/static/sequence/MappingPermutations.cpp"
"src/static/sequence/MappingWMBSBuilder.cpp"
"src/static/sequence/MappingWMBSCBuilder.cpp"
"src/static/sequence/MappingWMBlocksBuilder.cpp"
"src/static/sequence/MappingWM.cpp"
"src/static/sequence/MappingWMBMP.cpp"
"src/static/sequence/MappingWMBS.cpp"
"src/static/sequence/MappingWMBSC.cpp"
"src/static/sequence/MappingWMBlocks.cpp"
"src/static/sequence/WaveletMatrixHuff.cpp"
"src/static/sequence/SequenceBuilderWaveletMatrixHuff.cpp"
"src/static/bitsequence/BitSequenceBuilderDelta.cpp"
"src/static/sequence/WaveletTreeNoptrsHybrid.cpp"
"src/static/sequence/SequenceBuilderWTNoptrsHybrid.cpp"
"src/static/sequence/wt_coder_SubOptimalAdd.cpp"
"src/static/sequence/wt_coder_SubOptimalMult.cpp"
"src/static/sequence/wt_coder_Balanced.cpp"
"src/static/sequence/wt_coder_Pmin.cpp"
"src/static/sequence/wt_coder_Suboptimal.cpp"
"src/static/sequence/SequenceMatrix.cpp"
"src/static/sequence/SequenceBuilderSequenceMatrix.cpp"
"src/static/sequence/SequenceBinary.cpp"
"src/static/sequence/SequenceBuilderBinary.cpp"
#"src/static/sequence/MultiArityWT.cpp"
#"src/static/sequence/mwt_node.cpp"
#"src/static/sequence/mwt_coder.cpp"
#"src/static/sequence/mwt_coder_fixed.cpp"
"src/static/direct_access/DirectAccess.cpp"
"src/static/direct_access/DAC.cpp"
"src/static/direct_access/DACe.cpp"
"src/static/direct_access/DACInc.cpp"
"src/static/direct_access/ArrayDA.cpp"
"src/static/trees/CartesianTree.cpp"
"src/static/trees/bp.cpp"
"src/static/trees/bp-utils.cpp"
"src/static/trees/bp-darray.cpp"
"src/static/trees/bp-core.cpp"
"src/static/trees/Parentheses.cpp"
"src/static/trees/BP.cpp"
"src/static/trees/BPNaive.cpp"
"src/static/permutation/Permutation.cpp"
"src/static/permutation/PermutationMRRR.cpp"
"src/static/permutation/PermutationBuilderMRRR.cpp"
"src/static/permutation/perm.cpp"
"src/static/permutation/PermutationWT.cpp"
"src/static/permutation/PermutationBuilderWT.cpp"
"src/static/sequence/htwt/hu-tucker/basicsHT.cpp"
"src/static/sequence/htwt/hu-tucker/bitStringHT.cpp"
"src/static/sequence/htwt/hu-tucker/binarynode.cpp"
"src/static/sequence/htwt/hu-tucker/hutucker.cpp"
"src/static/sequence/htwt/PermWT.cpp"
"src/static/sequence/htwt/huTuckerPerm.cpp"
"src/static/sequence/htwt/HuTuckerPermPtrs.cpp"
"src/static/sequence/htwt/huTuckerNode.cpp"
"src/static/text_indexes/fmi.cpp"
#"src/static/text_indexes/SuffixSort.cpp"
"src/static/text_indexes/BWTBuilder.cpp"
"src/static/rmq/RMQ_succinct.cpp"
"src/static/rmq/RMQ.cpp"
)

# print messages
message("PROJECT = ${CMAKE_PROJECT_NAME}")
message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

#set(CMAKE_BUILD_TYPE "Debug")
# set default compilation and show details for debug build
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_VERBOSE_MAKEFILE ON)
endif(NOT CMAKE_BUILD_TYPE)

# Create the library
add_library(cds STATIC ${SOURCES})
install(TARGETS cds DESTINATION lib)
install (DIRECTORY "includes" DESTINATION includes/libcds)

# Testing
include_directories(../gtest-1.7.0/)
include_directories(../gtest-1.7.0/include/)
enable_testing()

include_directories(../lzisa/)
include_directories(../lzisa/include/)

include_directories("../libdivsufsort-2.0.1/include")
link_directories("../libdivsufsort-2.0.1/lib")

#add_executable(testArray tests/testArray.cpp ${GTEST_MAIN})
#target_link_libraries(testArray LINK_PUBLIC cds gtest gtest_main)
#add_test(TestArray testArray)

#add_executable(testSequences tests/testSequences.cpp ${GTEST_MAIN})
#target_link_libraries(testSequences LINK_PUBLIC cds gtest gtest_main)
#add_test(TestSequences testSequences)

#add_executable(testBWTBuilder tests/testBWTBuilder.cpp ${GTEST_MAIN})
#target_link_libraries(testBWTBuilder LINK_PUBLIC cds gtest gtest_main)
#add_test(TestBWTBuilder testBWTBuilder)

#add_executable(testAux tests/testAux.cpp)
#target_link_libraries(testAux LINK_PUBLIC cds)

#add_executable(testRepair tests/testRepair.cpp)
#target_link_libraries(testRepair LINK_PUBLIC cds)


#add_executable(testHeap apps/repairDisk/tests/TestHeap.cpp ${GTEST_MAIN})
#target_link_libraries(testHeap LINK_PUBLIC cds gtest gtest_main)
#add_test(TestHeap testHeap)

#add_executable(testMisc tests/testMisc.cpp)
#target_link_libraries(testMisc LINK_PUBLIC cds lzisa)

#add_executable(performanceSequences performance/performanceSequences.cpp)
#target_link_libraries(performanceSequences LINK_PUBLIC cds)

#add_executable(InBuffer tests/InBuffer.cpp ${GTEST_MAIN} includes/sequence/MappingTableAcelerators.h includes/utils/BitStreamIterator.h)
#target_link_libraries(InBuffer LINK_PUBLIC cds gtest gtest_main)
#add_test(InBuffer InBuffer)

#add_executable(testGenericMapper tests/testGenericMapper.cpp ${GTEST_MAIN})
#target_link_libraries(testGenericMapper LINK_PUBLIC cds gtest gtest_main)
#add_test(testGenericMapper testGenericMapper)

#add_executable(sssTest tests/utils/TestSparseSuffixSort.cpp ${GTEST_MAIN})
#target_link_libraries(sssTest LINK_PUBLIC cds gtest gtest_main)
#add_test(Testsss sssTest)

#add_executable(BitStreamTests tests/utils/BitStreamTests.cpp ${GTEST_MAIN})
#target_link_libraries(BitStreamTests LINK_PUBLIC cds gtest gtest_main)
#add_test(BitStreamTests bitStreamTests)


add_definitions(-D_LARGEFILE_SOURCE -D_LARGE_FILES -D_FILE_OFFSET_BITS=64)




#add_executable(apps/blockGraphs/bin/testSequences apps/blockGraphs/testSequences.cpp)
#target_link_libraries(apps/blockGraphs/bin/testSequences LINK_PUBLIC cds lzisa)

#add_executable(testFMIndex apps/indexingCPM/testFMIndex.cpp)
#target_link_libraries(testFMIndex LINK_PUBLIC cds)



