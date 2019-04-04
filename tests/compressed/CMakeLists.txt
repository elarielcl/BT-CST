set(compressed_TESTS
        CBlockTreeFixture
        BPSCBlockTreeFixture
        PCBlockTreeFixture
        DAPCBlockTreeFixture
        BPSPCBlockTreeFixture
        PBTRLCSACSTFixture
        PBTLCSACSTFixture
        PBTDABTCSTFixture
        PBTDABTDABTCSTFixture
        PBTDABTLCSACSTFixture
        PBTLCSALCSACSTFixture
        PBTNONELCSACSTFixture
        PBTNONEDABTCSTFixture
        PBTLCSADABTCSTFixture
        PBTnobackRLCSACSTFixture
        PBTnobackminmaxRLCSACSTFixture
        PBTcbackRLCSACSTFixture
        PBTISARLCSACSTFixture
        PBTCSTFixture
        CST_SADA_RLCSAFixture
        CST_SCT3_RLCSAFixture
        repair_differential_arrayFixture)


foreach( exe ${compressed_TESTS} )
    add_executable( check_${exe} ${exe}.cpp)
endforeach()



foreach( exe ${compressed_TESTS} )
    target_link_libraries( check_${exe} gtest )
    target_link_libraries( check_${exe} gtest_main )
    target_link_libraries( check_${exe} blocktree )
    target_link_libraries( check_${exe} ${CMAKE_SOURCE_DIR}/external/sdsl-litle/build/lib/libsdsl.a )
    target_link_libraries( check_${exe} ${CMAKE_SOURCE_DIR}/external/sdsl-litle/build/external/libdivsufsort/lib/libdivsufsort.a )
    target_link_libraries( check_${exe} ${CMAKE_SOURCE_DIR}/external/sdsl-litle/build/external/libdivsufsort/lib/libdivsufsort64.a )
    target_link_libraries( check_${exe} gctbio )
    target_link_libraries( check_${exe} textIndexes )
    target_link_libraries( check_${exe} cds )
    target_link_libraries( check_${exe} libRepairGN )
endforeach()