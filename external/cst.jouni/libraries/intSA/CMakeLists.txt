cmake_minimum_required(VERSION 2.8.4)
project(intSA)

# Project options
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_INSTALL_PREFIX /home/alberto/software)
set(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_PROJECT_NAME "Integer SA")

# Flags
set(CMAKE_CXX_FLAGS " -Wall -m64")
set(CMAKE_CXX_FLAGS_RELEASE " -std=c++0x -Wall -O9 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG " -g3 -DDEBUG")
#set(CMAKE_CXX_FLAGS_DEBUG " -g3 -DNDEBUG")
# Sources
include_directories(includes)

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
#add_library(intSA STATIC ${SOURCES})
#install(TARGETS intSA DESTINATION lib)
#install (DIRECTORY "includes" DESTINATION includes/libintSA)


# Testing
include_directories(../gtest-1.7.0/)
include_directories(../gtest-1.7.0/include/)
enable_testing()

#add_executable(testIntSA tests/testIntSA.cpp ${GTEST_MAIN})
#target_link_libraries(testIntSA LINK_PUBLIC intSA gtest gtest_main)
#add_test(TestIntSA testIntSA)

# Testing



