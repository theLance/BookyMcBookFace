#pragma once

#include <mutex>
#include <string>
#include <unordered_map>

#include "Theater.hpp"
#include "Movies.hpp"


typedef std::unordered_map<int, std::unordered_map<std::string, Theater>> MovieToTheaterMapping;

struct Theaters {
    std::vector<std::string> names;
    std::vector<Theater*> theaters;
};

/**
 * Class overseeing the configurations for theaters and the movie DB, including which theater is showing which movie.
 */
class TheaterAdmin {
    // theaters showing movies
    // movie can only be removed if no theaters are showing it
    // should include cleanse() where titles no theaters are showing are removed
public:
    struct OperationResult {
        enum Removal {
            SUCCESSFUL = 0,
            MOVIE_NOT_FOUND,
            NO_THEATER_FOR_MOVIE,     ///< There are no theaters showing this movie
            THEATER_NOT_SHOWING_MOVIE ///< The given theater is not showing the movie
        };
    };

    /**
     * Register a new premier. Creates an entry for a theater showing a given movie.
     * Also registers the movie, if it is new.
     *
     * @param theater The name of the theater where the movie is being introduced.
     * @param movie   The title of the movie.
     */
    void registerTheaterShowingMovie(const std::string& theater, const std::string& movie) {
        int movieId = m_movieDb.getMovieIdForTitle(movie);
        if(movieId == MovieDb::ID_NOT_FOUND) {
            std::scoped_lock sl(m_movieDbLock);
            m_movieDb.addMovie(movie);
        }
        std::scoped_lock sl(m_theaterListLock);
        m_theaters[movieId].emplace(theater, theater);
    }

    /**
     * Cancel a showing of a movie from the database. This means the given movie will not be
     * available in that given theater, but might still be elsewhere. If it is not, the movie
     * is also removed from the database.
     *
     * @param theater The name of the theater where the movie is being shown.
     * @param movie   The title of the movie.
     *
     * @returns       A result enum indicating whether the movie was found connected to the given
     *                theater and the removal was a success.
     */
    OperationResult::Removal removeMovieFromTheater(const std::string& theater, const std::string& movie) {
        int movieId = m_movieDb.getMovieIdForTitle(movie);
        if(movieId == MovieDb::ID_NOT_FOUND) {
            return OperationResult::Removal::MOVIE_NOT_FOUND;
        }

        auto theatersShowingMovie = m_theaters.find(movieId);
        if(theatersShowingMovie == m_theaters.end()) {
            // since the movie is no longer available, it should not be in the DB at all
            std::scoped_lock sl(m_movieDbLock);
            m_movieDb.removeMovie(movie);
            return OperationResult::Removal::NO_THEATER_FOR_MOVIE;
        }

        auto entryForMovieInTheater = theatersShowingMovie->second.find(theater);
        if(entryForMovieInTheater == theatersShowingMovie->second.end()) {
            return OperationResult::Removal::THEATER_NOT_SHOWING_MOVIE;
        }

        bool movieNoLongerAvailable = false;
        {
            std::scoped_lock sl(m_theaterListLock);
            theatersShowingMovie->second.erase(theater);
            if(theatersShowingMovie->second.empty()) {
                movieNoLongerAvailable = true;
                m_theaters.erase(theatersShowingMovie);
            }
        }

        if(movieNoLongerAvailable) {
            std::scoped_lock sl(m_movieDb);
            m_movieDb.removeMovie(movie);
        }
        return OperationResult::Removal::SUCCESSFUL;
    }

    /// Fetch currently playing movies' list.
    MovieDb::Titles listCurrentMovies() const {
        return m_movieDb.listMovies();
    }

    Theaters listTheatersPlayingMovie() const {
        //
    }

private:
    MovieToTheaterMapping m_theaters;
    MovieDb m_movieDb;

    std::mutex m_theaterListLock;
    std::mutex m_movieDbLock;
};