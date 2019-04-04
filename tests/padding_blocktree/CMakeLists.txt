set(padding_TESTS
        PBlockTreeBasicPropertiesFixture
        PBlockTreeFixture
        PBlockTreeWithoutCleanningFixture
        PConservativeHeuristicBlockTreeFixture
        PRealConservativeHeuristicBlockTreeFixture
        PRealOtherConservativeHeuristicBlockTreeFixture
        POtherConservativeHeuristicBlockTreeFixture
        PRealBackFrontBlockTreeFixture
        PRealOtherBackFrontBlockTreeFixture
        POtherBackFrontBlockTreeFixture
        PBackFrontBlockTreeFixture
        PColorBlockTreeFixture
        PRealColorBlockTreeFixture
        PColorPruningBlockTreeFixture
        PReverseColorBlockTreeFixture
        PConservativeOptimizedHeuristicBlockTreeFixture
        PRealConservativeOptimizedHeuristicBlockTreeFixture
        PLiberalHeuristicBlockTreeFixture
        PHeuristicConcatenateBlockTreeFixture
        PHeuristicBlockTreeFixture)

foreach( exe ${padding_TESTS} )
    add_executable( check_${exe} ${exe}.cpp)
endforeach()



foreach( exe ${padding_TESTS} )
    target_link_libraries( check_${exe} gtest )
    target_link_libraries( check_${exe} gtest_main )
    target_link_libraries( check_${exe} blocktree )
endforeach()