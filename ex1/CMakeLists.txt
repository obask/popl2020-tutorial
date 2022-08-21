include(GoogleTest)
include_directories(PRIVATE include)

add_executable(constraint
        src/Constraint.cpp
        src/Extractor.cpp
        src/Utils.cpp
        main_ex1.cpp)

target_link_libraries(constraint ${llvm_libs} ${Z3_LIBRARIES})

add_executable(ex1_tests
        ex1_tests.cpp
        src/Constraint.cpp
        src/Extractor.cpp
        src/Utils.cpp
        )

target_link_libraries(ex1_tests ${llvm_libs} ${Z3_LIBRARIES} GTest::gtest_main)


gtest_discover_tests(ex1_tests)
