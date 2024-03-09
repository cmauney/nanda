
add_executable(span_test
  span_test.cc
)

target_link_libraries(span_test
    PRIVATE
        nanda
        GTest::gtest_main
)

add_executable(rank_test
  rank_test.cc
)

target_link_libraries(rank_test
    PRIVATE
        nanda
        GTest::gtest_main
)

add_executable(index_algos_test
  index_algos_test.cc
)

target_link_libraries(index_algos_test
    PRIVATE
        nanda
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(rank_test)
gtest_discover_tests(index_algos_test)
gtest_discover_tests(span_test)