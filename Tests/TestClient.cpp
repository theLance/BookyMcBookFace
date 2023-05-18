#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Movies/TheaterAdmin.hpp"
#include "Client/Client.hpp"

struct ClientFixture : ::testing::Test {
    const std::string MOVIE_1 = "cica";
    const std::string THEATER_NAME = "cicaplex";
    const std::string MOVIE_2 = "cica2 - the sequel";
    const std::string OTHER_THEATER_NAME = "cicaverse";

    ClientFixture() : sut(admin) {
        admin.registerTheaterShowingMovie(THEATER_NAME, MOVIE_1);
    }

    TheaterAdmin admin;
    Client sut;
};

TEST_F(ClientFixture, list_movies) {
    ASSERT_EQ(1, sut.listAvailableMovies().size());
    ASSERT_EQ(MOVIE_1, sut.listAvailableMovies()[0]);
}

TEST_F(ClientFixture, list_theaters_for_movies) {
    ASSERT_EQ(1, sut.listTheatersPlayingMovie(MOVIE_1).size());
    ASSERT_EQ(THEATER_NAME, sut.listTheatersPlayingMovie(MOVIE_1)[0].name);
}

TEST_F(ClientFixture, list_theaters_for_movies__not_avialable) {
    ASSERT_EQ(0, sut.listTheatersPlayingMovie(MOVIE_2).size());
}

std::vector<std::string> fullCapacity = {"a1", "a2", "a3", "a4", "a5",
                                         "b1", "b2", "b3", "b4", "b5",
                                         "c1", "c2", "c3", "c4", "c5",
                                         "d1", "d2", "d3", "d4", "d5"};

TEST_F(ClientFixture, get_free_seats__initially_all_free) {
    sut.listTheatersPlayingMovie(MOVIE_1); // necessary to load theater
    ASSERT_EQ(fullCapacity, sut.listFreeSeatsForTheater(THEATER_NAME));
}

TEST_F(ClientFixture, cant_find_seats_if_theater_not_playing_movie) {
    sut.listTheatersPlayingMovie(MOVIE_1); // necessary to load theater
    ASSERT_EQ(Seats{}, sut.listFreeSeatsForTheater(OTHER_THEATER_NAME));
}

TEST_F(ClientFixture, get_free_seats__booking_makes_seats_not_free) {
    sut.listTheatersPlayingMovie(MOVIE_1); // necessary to load theater
    EXPECT_EQ(fullCapacity, sut.listFreeSeatsForTheater(THEATER_NAME));

    auto result = sut.book(THEATER_NAME, "a1,a2,a3");
    ASSERT_EQ(3, result.booking.success.size());
    ASSERT_EQ(0, result.booking.taken.size());
    ASSERT_EQ(0, result.booking.invalid.size());
    std::vector<std::string> expected = {                  "a4", "a5",
                                         "b1", "b2", "b3", "b4", "b5",
                                         "c1", "c2", "c3", "c4", "c5",
                                         "d1", "d2", "d3", "d4", "d5"};
    ASSERT_EQ(expected, sut.listFreeSeatsForTheater(THEATER_NAME));
}

