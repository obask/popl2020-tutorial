#include <gtest/gtest.h>

#include "include/Verifier.h"

// Demonstrate some basic assertions.
TEST(Ex3Test, BasicAssertions) {
    const auto result = doMain("./test/branch0.ll");
    // Expect two strings not to be equal.
    EXPECT_STREQ("hello world", "hello world");
    // Expect equality.
    EXPECT_EQ(result, 0);
}
