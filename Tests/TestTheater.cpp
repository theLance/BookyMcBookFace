#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#include "Movies/Theater.hpp"
#undef private

struct TheaterFixture : ::testing::Test {
    // These test values are specific to the current setup and need to be updated on changes
    const std::string OUT_OF_BOUND_ROW = "z1";
    const std::string OUT_OF_BOUND_COL = "a9";
    const std::string TOO_MANY_ROW_IDS = "aa1";

    const std::string VALID_SEAT_1_FIRST = "a1";
    const std::string VALID_SEAT_2 = "b2";
    const std::string VALID_SEAT_3 = "c3";
    const std::string VALID_SEAT_4_LAST = "d5";

    const std::string LOWERCASE_VALID_SEAT = "b2";
    const std::string LOWERCASE_INVALID_SEAT = "z2";

    TheaterFixture() : sut("Testiplex") {}

    Theater sut;
};

/* invalid seats */
TEST_F(TheaterFixture, invalid_row) {
    ASSERT_NE(0, sut.validateSeats({ OUT_OF_BOUND_ROW }).size());
    ASSERT_NE(0, sut.validateSeats({ TOO_MANY_ROW_IDS }).size());
}

TEST_F(TheaterFixture, invalid_col) {
    ASSERT_NE(0, sut.validateSeats({ OUT_OF_BOUND_COL }).size());
}

TEST_F(TheaterFixture, valid_seats_succeed_corner_cases) {
    ASSERT_EQ(0, sut.validateSeats({ VALID_SEAT_1_FIRST, VALID_SEAT_4_LAST }).size());
}

TEST_F(TheaterFixture, valid_seats_succeed_all) {
    ASSERT_EQ(0, sut.validateSeats({ VALID_SEAT_1_FIRST, VALID_SEAT_2, VALID_SEAT_3, VALID_SEAT_4_LAST }).size());
}

TEST_F(TheaterFixture, pick_valid_from_invalid) {
    auto res = sut.validateSeats({ OUT_OF_BOUND_COL, VALID_SEAT_1_FIRST, OUT_OF_BOUND_ROW, VALID_SEAT_2 });
    ASSERT_EQ(2, res.size());
    ASSERT_THAT(res, ::testing::ElementsAre(OUT_OF_BOUND_COL, OUT_OF_BOUND_ROW));
}

TEST_F(TheaterFixture, lowercase_invalid_row) {
    ASSERT_NE(0, sut.validateSeats({ LOWERCASE_INVALID_SEAT }).size());
}

TEST_F(TheaterFixture, lowercase_valid_row) {
    ASSERT_EQ(0, sut.validateSeats({ LOWERCASE_VALID_SEAT }).size());
}

/* booking */
TEST_F(TheaterFixture, cant_book_more_than_capacity) {
    std::vector<std::string> moreThanFull = {"a1", "a2", "a3", "a4", "a5",
                                             "b1", "b2", "b3", "b4", "b5",
                                             "c1", "c2", "c3", "c4", "c5", "c6",
                                             "d1", "d2", "d3", "d4", "d5"};
    auto result = sut.book(moreThanFull);
    ASSERT_EQ(moreThanFull.size(), result.invalid.size());
    ASSERT_EQ(0, result.success.size());
    ASSERT_EQ(0, result.taken.size());
}

TEST_F(TheaterFixture, full_capacity) {
    std::vector<std::string> fullCapacity = {"a1", "a2", "a3", "a4", "a5",
                                             "b1", "b2", "b3", "b4", "b5",
                                             "c1", "c2", "c3", "c4", "c5",
                                             "d1", "d2", "d3", "d4", "d5"};
    auto result = sut.book(fullCapacity);
    ASSERT_EQ(fullCapacity.size(), result.success.size());
    ASSERT_EQ(0, result.taken.size());
    ASSERT_EQ(0, result.invalid.size());
    ASSERT_EQ(0, sut.freeCapacity());
}

TEST_F(TheaterFixture, invalid_immediately_returns) {
    std::vector<std::string> moreThanFull = {"a1", "a9", "a3", "a4", "a5",
                                             "b1", "b9", "b3", "b4", "b5",
                                             "c1", "c9", "c3", "c4", "c5",
                                             "d1", "d2", "d3", "d4", "d5"};
    auto result = sut.book(moreThanFull);
    ASSERT_EQ(3, result.invalid.size());
    ASSERT_EQ(0, result.success.size());
    ASSERT_EQ(0, result.taken.size());
}

Seats whenSorted(Seats seats) {
    std::sort(seats.begin(), seats.end());
    return seats;
}

TEST_F(TheaterFixture, before_anything_booked_all_seats_free) {
    std::vector<std::string> fullCapacity = {"a1", "a2", "a3", "a4", "a5",
                                             "b1", "b2", "b3", "b4", "b5",
                                             "c1", "c2", "c3", "c4", "c5",
                                             "d1", "d2", "d3", "d4", "d5"};

    ASSERT_EQ(fullCapacity, whenSorted(sut.freeSeats()));
}

TEST_F(TheaterFixture, free_seats_reflect_bookings) {
    sut.book({"a1", "a2", "a3"});
    std::vector<std::string> expected =     {                  "a4", "a5",
                                             "b1", "b2", "b3", "b4", "b5",
                                             "c1", "c2", "c3", "c4", "c5",
                                             "d1", "d2", "d3", "d4", "d5"};
    ASSERT_EQ(expected, whenSorted(sut.freeSeats()));

    sut.book({"b2", "b3", "c4"});
    std::vector<std::string> expected2 =    {                  "a4", "a5",
                                             "b1",             "b4", "b5",
                                             "c1", "c2", "c3",       "c5",
                                             "d1", "d2", "d3", "d4", "d5"};
    ASSERT_EQ(expected2, whenSorted(sut.freeSeats()));
}

struct FirstRowBooked : TheaterFixture {
    FirstRowBooked() {
        auto result = sut.book({"a1", "a2", "a3", "a4", "a5"});
        EXPECT_EQ(5, result.success.size());
        EXPECT_EQ(0, result.taken.size());
        EXPECT_EQ(0, result.invalid.size());
    }
};

TEST_F(FirstRowBooked, seats_taken) {
    auto result = sut.book({"a2", "a3"});
    ASSERT_EQ(0, result.success.size());
    ASSERT_EQ(2, result.taken.size());
    ASSERT_EQ(0, result.invalid.size());
}

TEST_F(FirstRowBooked, not_all_seats_taken) {
    auto result = sut.book({"a2", "a3",
                            "b1", "b2"});
    ASSERT_EQ(2, result.success.size());
    ASSERT_EQ(2, result.taken.size());
    ASSERT_EQ(0, result.invalid.size());
}

TEST_F(FirstRowBooked, if_not_all_seats_taken_no_booking_occurs) {
    auto initialFreeSeats = sut.freeCapacity();

    auto result = sut.book({"a2", "a3",
                            "b1", "b2"});
    EXPECT_EQ(2, result.success.size());
    EXPECT_EQ(2, result.taken.size());
    EXPECT_EQ(0, result.invalid.size());

    ASSERT_EQ(initialFreeSeats, sut.freeCapacity());

    result = sut.book({"b1", "b2"});
    ASSERT_EQ(2, result.success.size());
    ASSERT_EQ(initialFreeSeats - 2, sut.freeCapacity());
}
