add_executable(cpack_test cpack_test.cc)
target_link_libraries(cpack_test PRIVATE compression)

add_executable(bdi_test bdi_test.cc)
target_link_libraries(bdi_test PRIVATE compression)

add_executable(fpc_test fpc_test.cc)
target_link_libraries(fpc_test PRIVATE compression)

add_executable(lz4_test lz4_test.cc)
target_link_libraries(lz4_test PRIVATE compression)

add_executable(huffman_test huffman_test.cc)
target_link_libraries(huffman_test PRIVATE compression)