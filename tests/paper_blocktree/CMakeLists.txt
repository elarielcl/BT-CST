set(paper_TESTS
        BlockTreeBasicPropertiesFixture
        BlockTreeFixture
        BlockTreeWithoutCleanningFixture
        HeuristicConcatenateBlockTreeFixture
        HeuristicBlockTreeFixture
        LiberalHeuristicBlockTreeFixture
        ConservativeHeuristicBlockTreeFixture
        OtherConservativeHeuristicBlockTreeFixture
        OtherBackFrontBlockTreeFixture
        BackFrontBlockTreeFixture
        ConservativeOptimizedHeuristicBlockTreeFixture
        ColorBlockTreeFixture
        ColorPruningBlockTreeFixture
        ReverseColorBlockTreeFixture)

foreach( exe ${paper_TESTS} )
    add_executable( check_${exe} ${exe}.cpp)
endforeach()



foreach( exe ${paper_TESTS} )
    target_link_libraries( check_${exe} gtest )
    target_link_libraries( check_${exe} gtest_main )
    target_link_libraries( check_${exe} blocktree )
endforeach()