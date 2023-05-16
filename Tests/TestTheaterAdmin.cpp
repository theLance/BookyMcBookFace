#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Movies/TheaterAdmin.hpp"

struct TheaterAdminFixture : ::testing::Test {
    const std::string MOVIE_1 = "cica";
    const std::string THEATER_NAME = "cicaplex";

    TheaterAdmin sut;
};

TEST_F(TheaterAdminFixture, can_add_theater_with_movie) {
    sut.registerTheaterShowingMovie(THEATER_NAME, MOVIE_1);

    ASSERT_EQ(1, sut.listTheatersPlayingMovie(MOVIE_1).size());
    ASSERT_EQ(THEATER_NAME, sut.listTheatersPlayingMovie(MOVIE_1)[0]->name());
}

TEST_F(TheaterAdminFixture, adding_new_movie_extend_db) {
    EXPECT_TRUE(sut.listCurrentMovies().empty());

    sut.registerTheaterShowingMovie(THEATER_NAME, MOVIE_1);

    ASSERT_EQ(1, sut.listCurrentMovies().size());
}

struct OneMovieAlreadyInOneTheater : TheaterAdminFixture {
    const std::string MOVIE_2 = "cica2 - the sequel";
    const std::string OTHER_THEATER_NAME = "cicaverse";

    OneMovieAlreadyInOneTheater() {
        sut.registerTheaterShowingMovie(THEATER_NAME, MOVIE_1);
    }
};

std::vector<std::string> namesOf(const Theaters& theaters) {
    std::vector<std::string> names;
    for(auto& t : theaters) {
        names.push_back(t->name());
    }
    return names;
}

TEST_F(OneMovieAlreadyInOneTheater, can_add_another_theater_with_same_movie) {
    sut.registerTheaterShowingMovie(OTHER_THEATER_NAME, MOVIE_1);

    auto res = sut.listTheatersPlayingMovie(MOVIE_1);
    ASSERT_EQ(2, res.size());
    ASSERT_THAT(namesOf(res), ::testing::Contains(THEATER_NAME));
    ASSERT_THAT(namesOf(res), ::testing::Contains(OTHER_THEATER_NAME));
}

TEST_F(OneMovieAlreadyInOneTheater, can_add_another_movie_to_same_theater) {
    sut.registerTheaterShowingMovie(THEATER_NAME, MOVIE_2);

    ASSERT_EQ(1, sut.listTheatersPlayingMovie(MOVIE_2).size());
    ASSERT_EQ(THEATER_NAME, sut.listTheatersPlayingMovie(MOVIE_2)[0]->name());
}

TEST_F(OneMovieAlreadyInOneTheater, can_add_another_movie_to_different_theater) {
    sut.registerTheaterShowingMovie(OTHER_THEATER_NAME, MOVIE_2);

    EXPECT_EQ(1, sut.listTheatersPlayingMovie(MOVIE_1).size());

    ASSERT_EQ(1, sut.listTheatersPlayingMovie(MOVIE_2).size());
    ASSERT_EQ(OTHER_THEATER_NAME, sut.listTheatersPlayingMovie(MOVIE_2)[0]->name());
}

TEST_F(OneMovieAlreadyInOneTheater, both_movies_can_play_in_both_theaters) {
    sut.registerTheaterShowingMovie(OTHER_THEATER_NAME, MOVIE_1);
    sut.registerTheaterShowingMovie(THEATER_NAME, MOVIE_2);
    sut.registerTheaterShowingMovie(OTHER_THEATER_NAME, MOVIE_2);

    auto res = sut.listTheatersPlayingMovie(MOVIE_1);
    ASSERT_EQ(2, res.size());
    ASSERT_THAT(namesOf(res), ::testing::Contains(THEATER_NAME));
    ASSERT_THAT(namesOf(res), ::testing::Contains(OTHER_THEATER_NAME));

    res = sut.listTheatersPlayingMovie(MOVIE_2);
    ASSERT_EQ(2, res.size());
    ASSERT_THAT(namesOf(res), ::testing::Contains(THEATER_NAME));
    ASSERT_THAT(namesOf(res), ::testing::Contains(OTHER_THEATER_NAME));
}

TEST_F(OneMovieAlreadyInOneTheater, removing_movie_possible) {
    sut.removeMovieFromTheater(THEATER_NAME, MOVIE_1);

    ASSERT_TRUE(sut.listTheatersPlayingMovie(MOVIE_1).empty());
}

TEST_F(OneMovieAlreadyInOneTheater, removing_only_movie_clears_db) {
    sut.removeMovieFromTheater(THEATER_NAME, MOVIE_1);

    ASSERT_TRUE(sut.listCurrentMovies().empty());
}

TEST_F(OneMovieAlreadyInOneTheater, db_not_cleared_if_other_theater_still_playing_movie) {
    sut.registerTheaterShowingMovie(OTHER_THEATER_NAME, MOVIE_1);
    sut.removeMovieFromTheater(THEATER_NAME, MOVIE_1);

    ASSERT_FALSE(sut.listCurrentMovies().empty());
}

struct OneUniqueMovieInEachTheater : OneMovieAlreadyInOneTheater {
    const std::string NON_EXISTENT_MOVIE = "nope";

    OneUniqueMovieInEachTheater() {
        sut.registerTheaterShowingMovie(OTHER_THEATER_NAME, MOVIE_2);
    }
};

TEST_F(OneUniqueMovieInEachTheater, successful_removal) {
    ASSERT_EQ(TheaterAdmin::OperationResult::SUCCESSFUL, sut.removeMovieFromTheater(THEATER_NAME, MOVIE_1));
}

TEST_F(OneUniqueMovieInEachTheater, movie_to_be_removed_doesnt_exist) {
    ASSERT_EQ(TheaterAdmin::OperationResult::MOVIE_NOT_FOUND,
              sut.removeMovieFromTheater(THEATER_NAME, NON_EXISTENT_MOVIE));
}

TEST_F(OneUniqueMovieInEachTheater, movie_to_be_removed_doesnt_play_in_theater) {
    ASSERT_EQ(TheaterAdmin::OperationResult::THEATER_NOT_SHOWING_MOVIE,
              sut.removeMovieFromTheater(THEATER_NAME, MOVIE_2));
}
