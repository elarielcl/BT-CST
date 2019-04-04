
cmake_minimum_required(VERSION 3.0)
project(BlockTree)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O9")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")


include_directories(includes)
include_directories(external)
include_directories(external/sdsl-lite/include)


include_directories(external/cst.jouni/libraries/TextIndexes/includes)
include_directories(external/cst.jouni/libraries/libcds2/includes)
include_directories(external/cst.jouni/libraries/tools/includes)



add_subdirectory(src)
#add_subdirectory(tests)
add_subdirectory(experiments)

add_subdirectory(external/cst.jouni)
add_subdirectory(external/repair_gn)


