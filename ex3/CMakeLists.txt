include(GoogleTest)
include_directories(PRIVATE include)

add_executable(verifier
        src/Verifier.cpp
        src/Extractor.cpp
        src/Utils.cpp
        main_ex3.cpp)

target_link_libraries(verifier ${llvm_libs} ${Z3_LIBRARIES})


add_executable(ex3_tests
        ex3_tests.cpp
        src/Verifier.cpp
        src/Extractor.cpp
        src/Utils.cpp
        )

target_link_libraries(ex3_tests ${llvm_libs} ${Z3_LIBRARIES} GTest::gtest_main)


gtest_discover_tests(ex3_tests)
