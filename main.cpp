#include <string_view>

#include "parser.hpp"

#include "gtest/gtest.h"

struct TestError : testing::TestWithParam<std::string_view> {
};

TEST_P(TestError, ThrowsOnError)
{
    EXPECT_ANY_THROW({ gtask::parse(GetParam()); });
}

INSTANTIATE_TEST_SUITE_P(TestSuite,
                         TestError,
                         testing::Values(
    "[",
    "]",
    "0",
    "4",
    "a1[]",
    "a1[2]",
    "a1[2b]",
    "a1[2[b]",
    "a1[2]b",
    "a2[2[b3[c]4]def3[uX]]"
));

struct TestSuccessInputs {
    std::string_view input;
    std::string_view output_expected;
};

struct TestSuccess : testing::TestWithParam<TestSuccessInputs> {
};

TEST_P(TestSuccess, Parses)
{
    EXPECT_NO_THROW({
        EXPECT_STREQ(gtask::parse(GetParam().input).c_str(), GetParam().output_expected.data());
    });
}

INSTANTIATE_TEST_SUITE_P(TestSuite,
                         TestSuccess,
                         testing::Values(
    TestSuccessInputs{ .input = "",                     .output_expected = ""},
    TestSuccessInputs{ .input = "a1[2[b]]",             .output_expected = "abb"},
    TestSuccessInputs{ .input = "a2[2[bc]]",            .output_expected = "abcbcbcbc"},
    TestSuccessInputs{ .input = "a2[2[b3[c]]]",         .output_expected = "abcccbcccbcccbccc" },
    TestSuccessInputs{ .input = "a2[2[b3[c]]def3[uX]]", .output_expected = "abcccbcccdefuXuXuXbcccbcccdefuXuXuX"}
));
