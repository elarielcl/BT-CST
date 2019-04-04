set(project_EXECUTABLES
        # RSA
        experiments_block_tree
        # Diff. Array
        test_rlcsa_CSA
        test_DABT_CSA
        test_lcsa_c_sampling_CSA
        test_lcsa_CSA
        test_lcsa_with_compressed_lengths_CSA
        test_lcsa_with_lengths_CSA
        # CST
        test_sct3_CST
        test_fully_cst
        test_pbtrlcsacst_cst
        test_pbtlcsacst_cst
        test_pbtlcsalcsacst_cst
        test_pbtnonelcsacst_cst
        test_pbtlcsadabtcst_cst
        test_pbtnonedabtcst_cst
        test_pbtnobackminmaxrlcsacst_cst
        test_pbtnobackrlcsacst_cst
        test_pbtcbackrlcsacst_cst
        test_sada_cst
        test_sada_rlcsa
        test_sct3_rlcsa_CST
        # Maximal Substrings
        maximal_substrings_sada
        maximal_substrings_sct3
        maximal_substrings_fully
        maximal_substrings_sct3_rlcsa
        maximal_substrings_sada_rlcsa
        maximal_substrings_pbt
        maximal_substrings_pbtlcsa
        maximal_substrings_pbtlcsalcsa
        maximal_substrings_pbtlcsadabt
        maximal_substrings_pbtnonedabt
        maximal_substrings_pbtnonelcsa)


foreach( exe ${project_EXECUTABLES} )
    add_executable( ${exe} ${exe}.cpp)
endforeach()


foreach( exe ${project_EXECUTABLES} )
    target_link_libraries( ${exe} blocktree )
    target_link_libraries( ${exe} ${CMAKE_SOURCE_DIR}/external/sdsl-lite/lib/libsdsl.a )
    target_link_libraries( ${exe} ${CMAKE_SOURCE_DIR}/external/sdsl-lite/lib/libdivsufsort.a )
    target_link_libraries( ${exe} ${CMAKE_SOURCE_DIR}/external/sdsl-lite/lib/libdivsufsort64.a )
    target_link_libraries( ${exe} gctbio )
    target_link_libraries( ${exe} textIndexes )
    target_link_libraries( ${exe} cds )
    target_link_libraries( ${exe} libRepairGN )
endforeach()