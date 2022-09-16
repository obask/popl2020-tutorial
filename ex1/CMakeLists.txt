include(GoogleTest)

add_executable(EX1_CODELAB ex1_main.cpp)

target_link_libraries(EX1_CODELAB ${llvm_libs} ${Z3_LIBRARIES})

add_executable(ex1_tests ex1_tests.cpp)

target_link_libraries(ex1_tests ${llvm_libs} ${Z3_LIBRARIES} GTest::gtest_main)


gtest_discover_tests(ex1_tests)
