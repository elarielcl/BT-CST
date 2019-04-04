include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_subdirectory(compressed)
add_subdirectory(padding_blocktree)
add_subdirectory(paper_blocktree)

add_subdirectory(googletest)