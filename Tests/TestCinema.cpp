#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#include "../Movies/Cinema.hpp"
#undef private

struct CinemaFixture : ::testing::Test {
    // These test values are specific to the current setup and need to be updated on changes
    const std::string OUT_OF_BOUND_ROW = "Z1";
    const std::string OUT_OF_BOUND_COL = "A9";
    const std::string TOO_MANY_ROW_IDS = "AA1";

    const std::string VALID_SEAT_1_FIRST = "A1";
    const std::string VALID_SEAT_2 = "B2";
    const std::string VALID_SEAT_3 = "C3";
    const std::string VALID_SEAT_4_LAST = "D5";

    const std::string LOWERCASE_VALID_SEAT = "b2";    
    const std::string LOWERCASE_INVALID_SEAT = "z2";

    Cinema sut;
};

TEST_F(CinemaFixture, invalid_row) {
    ASSERT_NE(0, sut.validateSeats({ OUT_OF_BOUND_ROW }).size());
    ASSERT_NE(0, sut.validateSeats({ TOO_MANY_ROW_IDS }).size());
}

TEST_F(CinemaFixture, invalid_col) {
    ASSERT_NE(0, sut.validateSeats({ OUT_OF_BOUND_COL }).size());
}

TEST_F(CinemaFixture, valid_seats_succeed_corner_cases) {
    ASSERT_EQ(0, sut.validateSeats({ VALID_SEAT_1_FIRST, VALID_SEAT_4_LAST }).size());
}

TEST_F(CinemaFixture, valid_seats_succeed_all) {
    ASSERT_EQ(0, sut.validateSeats({ VALID_SEAT_1_FIRST, VALID_SEAT_2, VALID_SEAT_3, VALID_SEAT_4_LAST }).size());
}

TEST_F(CinemaFixture, pick_valid_from_invalid) {
    auto res = sut.validateSeats({ OUT_OF_BOUND_COL, VALID_SEAT_1_FIRST, OUT_OF_BOUND_ROW, VALID_SEAT_2 });
    ASSERT_EQ(2, res.size());
    ASSERT_THAT(res, ::testing::ElementsAre(OUT_OF_BOUND_COL, OUT_OF_BOUND_ROW));
}

TEST_F(CinemaFixture, lowercase_invalid_row) {
    ASSERT_NE(0, sut.validateSeats({ LOWERCASE_INVALID_SEAT }).size());
}

TEST_F(CinemaFixture, lowercase_valid_row) {
    ASSERT_EQ(0, sut.validateSeats({ LOWERCASE_VALID_SEAT }).size());
}
