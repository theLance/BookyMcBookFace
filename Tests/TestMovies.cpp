#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../Movies/Movies.hpp"

struct MovieFixture : ::testing::Test {
    MovieDb sut;
};

struct TestAddOneMovie : MovieFixture {
    const std::string TITLE = "Cica";
    TestAddOneMovie() {
        sut.addMovie(TITLE);
    }
};

TEST_F(TestAddOneMovie, size_is_correct) {
    ASSERT_EQ(1, sut.listMovies().size());
}

TEST_F(TestAddOneMovie, title_is_correct) {
    ASSERT_EQ(TITLE, sut.listMovies()[0]);
}

TEST_F(TestAddOneMovie, id_and_title_can_be_found) {
    auto id = sut.getMovieIdForTitle(TITLE);
    ASSERT_EQ(TITLE, sut.getMovieTitleForId(id));
}

TEST_F(TestAddOneMovie, remove_works) {
    sut.removeMovie(TITLE);
    ASSERT_EQ(0, sut.listMovies().size());
}

struct TestAddMoreMovies : MovieFixture {
    const std::string TITLE_1 = "Cica1";
    const std::string TITLE_2 = "Cica2";
    const std::string TITLE_3 = "Cica3";
    TestAddMoreMovies() {
        sut.addMovie(TITLE_1);
        sut.addMovie(TITLE_2);
        sut.addMovie(TITLE_3);
    }
};

TEST_F(TestAddMoreMovies, size_is_correct) {
    ASSERT_EQ(3, sut.listMovies().size());
}

TEST_F(TestAddMoreMovies, titles_are_correct) {
    ASSERT_THAT(sut.listMovies(), ::testing::ElementsAre(TITLE_1, TITLE_2, TITLE_3));
}

TEST_F(TestAddMoreMovies, ids_are_unique) {
    auto id1 = sut.getMovieIdForTitle(TITLE_1);
    auto id2 = sut.getMovieIdForTitle(TITLE_2);
    auto id3 = sut.getMovieIdForTitle(TITLE_3);
    ASSERT_NE(id1, id2);
    ASSERT_NE(id3, id2);
}

TEST_F(TestAddMoreMovies, remove_works) {
    sut.removeMovie(TITLE_1);
    ASSERT_EQ(2, sut.listMovies().size());
    sut.removeMovie(TITLE_2);
    ASSERT_EQ(1, sut.listMovies().size());
}
