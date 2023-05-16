#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "InputHandling/InputSanitizer.hpp"

using ::testing::ElementsAre;

TEST(SplitInput, valid_string_with_comma_separator) {
    ASSERT_THAT( convertAndSanitizeInput("a1,a2,a3"), ElementsAre("a1", "a2", "a3") );
}

TEST(SplitInput, valid_string_with_space_separator) {
    ASSERT_THAT( convertAndSanitizeInput("a1 a2 a3"), ElementsAre("a1", "a2", "a3") );
}

TEST(SplitInput, valid_string_with_mixed_separators) {
    ASSERT_THAT( convertAndSanitizeInput("a1 a2,a3"), ElementsAre("a1", "a2", "a3") );
}

TEST(SplitInput, consecutive_separators_dont_add_empty_item) {
    ASSERT_THAT( convertAndSanitizeInput("a1  a2    a3"), ElementsAre("a1", "a2", "a3") );
}

TEST(SplitInput, consecutive_mixed_separators_dont_add_empty_item) {
    ASSERT_THAT( convertAndSanitizeInput("a1,,,a2    a3 , , a4"), ElementsAre("a1", "a2", "a3", "a4") );
}

TEST(FilterInput, valid_string_with_wrong_separator_filtered) {
    ASSERT_THAT( convertAndSanitizeInput("a-1, b-2, c-3"), ElementsAre("a1", "b2", "c3") );
}

TEST(FilterInput, invalid_elements_removed) {
    ASSERT_THAT( convertAndSanitizeInput("a1,a2,a3,%^"), ElementsAre("a1", "a2", "a3") );
}

TEST(FilterInput, invalid_elements_removed_within_item) {
    ASSERT_THAT( convertAndSanitizeInput("a1,a*2,a%3"), ElementsAre("a1", "a2", "a3") );
}

TEST(InputCasing, uppercase_input_normalized) {
    ASSERT_THAT( convertAndSanitizeInput("A1,A2,A3"), ElementsAre("a1", "a2", "a3") );
}

TEST(InputCasing, mixedcase_input_normalized) {
    ASSERT_THAT( convertAndSanitizeInput("A1,a2,aA3"), ElementsAre("a1", "a2", "aa3") );
}
