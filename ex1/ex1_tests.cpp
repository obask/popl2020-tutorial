#include <gtest/gtest.h>
#include <filesystem>

#include "include/Constraint.h"

// Demonstrate some basic assertions.
TEST(Ex1DivisionBy0Test, Loop0_ll) {
    const std::filesystem::path &path = std::filesystem::current_path();
    std::cout << path << std::endl;
    auto result = runConstraints(path / "../../ex1/test/loop0.ll", false);
    EXPECT_EQ(result.size(), 1);
    EXPECT_STREQ("%div = sdiv i32 4, %3", result[0].c_str());
}
