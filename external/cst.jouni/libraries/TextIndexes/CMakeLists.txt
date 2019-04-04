  cmake_minimum_required(VERSION 2.8)
project(TextIndexes)
set(PROJECT_VENDOR "Alberto Ordóñez")
set(CMAKE_PROJECT_NAME "Text Indexes")
set(PROJECT_CONTACT "alberto.ordonez@udc.es")
set(PROJECT_DESCRIPTION "TextIndexes")

set(CMAKE_INSTALL_PREFIX /home/alberto/software)
set(CMAKE_COMPILER_IS_GNUCXX)

# Flags
set(CMAKE_CXX_FLAGS " -Wall -m64 -std=c++0x ")
set(CMAKE_CXX_FLAGS_RELEASE " -Wall -O9 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG " -g3 -DDEBUG")
#set(CMAKE_CXX_FLAGS_DEBUG " -g3 -DNDEBUG")

set(LIBCDS_DIR "../libcds2/")

set(SOURCES
"src/rlcsa/rlcsa.cpp"
"src/rlcsa/rlcsa_builder.cpp"
"src/rlcsa/sasamples.cpp"
"src/rlcsa/sampler.cpp"
"src/rlcsa/lcpsamples.cpp"
"src/rlcsa/arrayCSA.cpp"
"src/rlcsa/bitvector.cpp"
"src/rlcsa/deltavector.cpp"
"src/rlcsa/rlevector.cpp"
"src/rlcsa/nibblevector.cpp"
"src/rlcsa/parameters.cpp"
"src/rlcsa/utils.cpp"
"src/rlcsa/qsufsort.cpp"
"src/TextIndex.cpp"
"src/TextIndexRLCSA.cpp"
"src/BWTBuilder.cpp"
)

#set(CMAKE_BUILD_TYPE "Debug")
# set default compilation and show details for debug build
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_VERBOSE_MAKEFILE ON)
endif(NOT CMAKE_BUILD_TYPE)

include_directories(includes/)
include_directories(${LIBCDS_DIR})
include_directories(${LIBCDS_DIR}includes/)
include_directories(../intSA/includes/)

add_library(textIndexes STATIC ${SOURCES})
install(TARGETS textIndexes DESTINATION lib)
install (DIRECTORY "includes" DESTINATION includes/libtextIndexes)

include_directories(../gtest-1.7.0)
include_directories(../gtest-1.7.0/include/)
enable_testing()

#add_executable(ttIndex test/ttIndex.cpp ${GTEST_MAIN})
#target_link_libraries(ttIndex LINK_PUBLIC textIndexes cds gtest gtest_main)
#add_test(TtIndex ttIndex)