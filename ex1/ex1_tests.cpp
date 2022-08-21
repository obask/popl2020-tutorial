#include <gtest/gtest.h>

#include "include/Constraint.h"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    const auto result = runConstraints("./test/branch0.ll");
    // Expect two strings not to be equal.
    EXPECT_STREQ("hello world", "hello world");
    // Expect equality.
    EXPECT_EQ(result, 0);
}
