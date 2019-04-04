cmake_minimum_required(VERSION 2.8)
project(gct+bio)
set(PROJECT_VENDOR "Alberto Ordóñez")
set(CMAKE_PROJECT_NAME "CST_REP")
set(PROJECT_CONTACT "alberto.ordonez@udc.es")
set(PROJECT_DESCRIPTION "cst_rep")
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_INSTALL_PREFIX /home/alberto/software)
set(CMAKE_COMPILER_IS_GNUCXX)

# Flags
set(CMAKE_CXX_FLAGS " -Wall -std=c++0x ")
set(CMAKE_CXX_FLAGS_RELEASE " -O9 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG " -g3 -DDEBUG")
#set(CMAKE_CXX_FLAGS_DEBUG " -g3 -DNDEBUG")

set(LIBCDS_DIR "../libraries/libcds2/")

include_directories(includes/)
set(SOURCES
"src/LCP_SAD.cpp"
"src/LCP_FMN_RLCSA.cpp"
"src/LCP_RLCSA.cpp"
)

#set(CMAKE_BUILD_TYPE "Debug")
# set default compilation and show details for debug build
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_VERBOSE_MAKEFILE ON)
endif(NOT CMAKE_BUILD_TYPE)

include_directories(${LIBCDS_DIR})
include_directories(${LIBCDS_DIR}includes/)

add_library(gctbio STATIC ${SOURCES})
install(TARGETS gctbio DESTINATION lib)
install (DIRECTORY "includes" DESTINATION includes/libgctbio)
include_directories(../libraries/TextIndexes/includes/)
include_directories(../libraries/gtest-1.7.0)
include_directories(../libraries/gtest-1.7.0/include/)
enable_testing()


#add_executable(buildCST runnable/buildCST.cpp)
#target_link_libraries(buildCST LINK_PUBLIC gctbio cds textIndexes )


