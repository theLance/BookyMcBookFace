#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "Theater.hpp"
#include "Movies.hpp"


typedef std::unordered_map<int, std::unordered_map<std::string, std::shared_ptr<Theater>>> MovieToTheaterMapping;
typedef std::vector<std::shared_ptr<Theater>> Theaters;

/**
 * Class overseeing the configurations for theaters and the movie DB, including which theater is showing which movie.
 */
class TheaterAdmin {
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
     * Register a new show. Creates an entry for a theater showing a given movie.
     * Also registers the movie, if it is new.
     *
     * @param theater The name of the theater where the movie is being introduced.
     * @param movie   The title of the movie.
     */
    void registerTheaterShowingMovie(const std::string& theater, const std::string& movie) {
        int movieId = m_movieDb.getMovieIdForTitle(movie);
        if(movieId == MovieDb::ID_NOT_FOUND) {
            movieId = m_movieDb.addMovie(movie);
        }
        m_theaters[movieId].emplace(theater, std::make_shared<Theater>(theater));
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
            m_movieDb.removeMovie(movie);
            return OperationResult::Removal::NO_THEATER_FOR_MOVIE;
        }

        auto entryForMovieInTheater = theatersShowingMovie->second.find(theater);
        if(entryForMovieInTheater == theatersShowingMovie->second.end()) {
            return OperationResult::Removal::THEATER_NOT_SHOWING_MOVIE;
        }

        bool movieNoLongerAvailable = false;
        {
            theatersShowingMovie->second.erase(theater);
            if(theatersShowingMovie->second.empty()) {
                movieNoLongerAvailable = true;
                m_theaters.erase(theatersShowingMovie);
            }
        }

        if(movieNoLongerAvailable) {
            m_movieDb.removeMovie(movie);
        }
        return OperationResult::Removal::SUCCESSFUL;
    }

    /// Fetch currently playing movies' list.
    MovieDb::Titles listCurrentMovies() const {
        return m_movieDb.listMovies();
    }

    /**
     * Fetch an array of theaters currently playing the selected movie.
     */
    Theaters listTheatersPlayingMovie(const std::string& movie) const {
        auto resIt = m_theaters.find(m_movieDb.getMovieIdForTitle(movie));
        if(resIt == m_theaters.end()) {
            return {};
        }

        Theaters result;
        for(auto& entry : resIt->second) {
            result.push_back(entry.second);
        }
        return result;
    }

private:
    MovieToTheaterMapping m_theaters;
    MovieDb m_movieDb;
};
