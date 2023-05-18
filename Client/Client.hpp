#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "InputHandling/InputSanitizer.hpp"
#include "Movies/TheaterAdmin.hpp"


typedef std::unordered_map<std::string, std::shared_ptr<Theater>> TheaterMap;
struct TheaterCapacity {
    TheaterCapacity(const std::string& n, int c) : name(n), capacity(c) {}

    std::string name;
    int capacity;
};
typedef std::vector<TheaterCapacity> AvailableTheaters;


/**
 * The booking client for the theaters.
 *
 * Connects to the TheaterAdmin to get information on currently playing movies, theaters playing them,
 * free seats therein, and to book tickets.
 */
class Client {
    enum CheckShowMessage {
        OK = 1,
        NOT_FOUND,
        CANCELLED
    };

    struct CheckShowResult {
        CheckShowMessage msg;
        TheaterMap::iterator it;
    };
public:

    Client(TheaterAdmin& ta) : m_admin(ta) {}

    MovieDb::Titles listAvailableMovies() const {
        return m_admin.listCurrentMovies();
    }

    /**
     * List the theaters playing the selected movie.
     *
     * @param movie  The selected movie.
     *
     * @returns A list of theaters and number of available seats therein.
     */
    AvailableTheaters listTheatersPlayingMovie(const std::string& movie) {
        m_theatersForMovie.clear();

        AvailableTheaters result;
        for(auto& res : m_admin.listTheatersPlayingMovie(movie)) {
            if(res->freeCapacity()) {
                result.emplace_back(res->name(), res->freeCapacity());
                m_theatersForMovie.emplace(res->name(), res);
            }
        }
        return result;
    }

    /**
     * Get the list of free seats in a given theater playing a movie.
     *
     * This is not the list of all the seats for all the movies in a theater, since a Theater
     * instance is a given movie in a given theater.
     *
     * @param theater  A given theater's showing of a movie.
     *
     * @returns An array of seats that are currently free.
     */
    Seats listFreeSeatsForTheater(const std::string& theater) {
        auto res = checkShowInCurrent(theater);
        if(res.msg != CheckShowMessage::OK) {
            return {};
        }
        auto result = res.it->second->freeSeats();
        std::sort(result.begin(), result.end());
        return result;
    }

    /**
     * Book a set of seats for a showing of a movie.
     *
     * @param theater            The selected theater's showing of the movie.
     * @param rawBookingString   A string of seats that the user wishes to book.
     *
     * @returns  //
     */
    Booking book(const std::string& theater, const std::string& rawBookingString) {
        auto input = convertAndSanitizeInput(rawBookingString);
        auto res = checkShowInCurrent(theater);
        if(res.msg != CheckShowMessage::OK) {
            return {{},{},input};
        }
        return res.it->second->book(input);
    }

protected:
    CheckShowResult checkShowInCurrent(const std::string& theater) {
        auto it = m_theatersForMovie.find(theater);
        if(it == m_theatersForMovie.end()) {
            return {CheckShowMessage::NOT_FOUND, it};
        }
        if(it->second.use_count() == 1) {
            m_theatersForMovie.erase(it);
            return {CheckShowMessage::CANCELLED, it};
        }
        return {CheckShowMessage::OK, it};
    }

    TheaterMap m_theatersForMovie;

private:
    TheaterAdmin& m_admin;
};
