cmake_minimum_required(VERSION 2.8)
project(Repair)
set(PROJECT_VENDOR "Adrian Gomez Brandon")
set(CMAKE_PROJECT_NAME "Repair")
set(PROJECT_CONTACT "adrian.gbrandon@udc.es")
set(CMAKE_INSTALL_PREFIX /home/adrian/software)
set(CMAKE_COMPILER_IS_GNUCXX)

include_directories(./include)

set (SOURCES
        include/array.h
        include/basics.h
        include/hash.h
        include/heap.h
        include/records.h
        include/irepair.h
        src/array.cpp
        src/basics.cpp
        src/hash.cpp
        src/heap.cpp
        src/irepair.cpp
        src/records.cpp
        #main.cpp
        )


# Flags
set(CMAKE_CXX_FLAGS " -Wall -m64")
set(CMAKE_CXX_FLAGS_RELEASE " -std=c++0x -Wall -O9 -DNDEBUG")
#set(CMAKE_CXX_FLAGS_DEBUG " -std=c++0x -Wall -O9 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG " -std=c++0x -Wall -g3 -DDEBUG")
#set(CMAKE_CXX_FLAGS_DEBUG " -std=c++0x -Wall -O3 -pg -DDEBUG")

# set default compilation and show details for debug build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif(NOT CMAKE_BUILD_TYPE)

# Create the library
add_library(libRepairGN STATIC ${SOURCES})
install(TARGETS libRepairGN DESTINATION lib)
install (DIRECTORY "includes/repair_gn" DESTINATION includes/libRepairGN)


#add_executable(repair ./main.cpp)
#target_link_libraries(repair LINK_PUBLIC libRepairGN)