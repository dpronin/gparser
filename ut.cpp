#include <stdexcept>
#include <string_view>

#include "gparser.hpp"

#include "gtest/gtest.h"

struct TestFailureInputInvalid : testing::TestWithParam<std::string_view> {
};

TEST_P(TestFailureInputInvalid, ThrowsOnError)
{
    EXPECT_ANY_THROW({ gtask::parse(GetParam()); });
}

INSTANTIATE_TEST_SUITE_P(TestSuite,
                         TestFailureInputInvalid,
                         testing::Values(
    "[",
    "]",
    "0",
    "0[",
    "0[ab]",
    "4",
    "4[",
    "4[]",
    "a1[]",
    "a1[2]",
    "a1[2b]",
    "a1[2[b]",
    "a1[2]b",
    "a2[2[b3[c]4]def3[uX]]",
    "a1x[ab]",
    "a1[10a[ab]]",
    "a4294967296[bc]",
    "a1[b4294967296[c]]"
));

struct TestSuccessParam {
    std::string_view input;
    std::string_view output_expected;
};

struct TestSuccess : testing::TestWithParam<TestSuccessParam> {
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
    TestSuccessParam{ .input = "",                     .output_expected = ""},
    TestSuccessParam{ .input = "a1[2[b]]",             .output_expected = "abb"},
    TestSuccessParam{ .input = "a2[2[bc]]",            .output_expected = "abcbcbcbc"},
    TestSuccessParam{ .input = "a2[2[b3[c]]]",         .output_expected = "abcccbcccbcccbccc" },
    TestSuccessParam{ .input = "a2[2[b3[c]]def3[uX]]", .output_expected = "abcccbcccdefuXuXuXbcccbcccdefuXuXuX"},
    TestSuccessParam{ .input = "12[ab]",               .output_expected = "abababababababababababab"},
    TestSuccessParam{ .input = "1[a21[b]]",            .output_expected = "abbbbbbbbbbbbbbbbbbbbb"},
    TestSuccessParam{ .input = "100[y]",               .output_expected = "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"}
));

struct TestFailureOutputExceedsMaxLenParam {
    std::string_view input;
    gtask::params params;
};

struct TestFailureOutputExceedsMaxLen : testing::TestWithParam<TestFailureOutputExceedsMaxLenParam> {
};

TEST_P(TestFailureOutputExceedsMaxLen, ThrowsOnError)
{
    EXPECT_THROW({ gtask::parse(GetParam().input, GetParam().params); }, std::range_error);
}

INSTANTIATE_TEST_SUITE_P(TestSuite,
                         TestFailureOutputExceedsMaxLen,
                         testing::Values(
    TestFailureOutputExceedsMaxLenParam{ .input = "a1[2[b]]",             .params = { 2 }  },
    TestFailureOutputExceedsMaxLenParam{ .input = "a2[2[bc]]",            .params = { 8 }  },
    TestFailureOutputExceedsMaxLenParam{ .input = "a2[2[b3[c]]]",         .params = { 16 } },
    TestFailureOutputExceedsMaxLenParam{ .input = "a2[2[b3[c]]def3[uX]]", .params = { 34 } },
    TestFailureOutputExceedsMaxLenParam{ .input = "12[ab]",               .params = { 23 } },
    TestFailureOutputExceedsMaxLenParam{ .input = "1[a21[b]]",            .params = { 21 } },
    TestFailureOutputExceedsMaxLenParam{ .input = "100[y]",               .params = { 99 } }
));
