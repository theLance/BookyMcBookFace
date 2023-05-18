#pragma once

#include <algorithm>
#include <cctype>
#include <mutex>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "InputHandling/InputSanitizer.hpp"

typedef std::vector<std::string> Seats;

struct Booking {
    Seats success;
    Seats taken;
    Seats invalid;
};

/**
 * The class that handles the seat assignments in a theater. This is technically not
 * a theater, only one show in it. This way, multiple movies can be booked at the
 * same time, with the least amount of blocking.
 *
 * Currently all theaters have 20 seats in a 4x5 arrangement. This can be changed by
 * making the ROWxCOL dimensions configurable.
 */
class Theater {
    /* Every theater has 4 rows of 5 seats */
    static const int ROWS = 4;
    static const int COLS = 5;

public:
    Theater(const std::string& name)
            : m_name(name) {
        fillSeatNames();
    }

    const std::string& name() const {
        return m_name;
    }

    // The destructor could check if the time of showing has already happened and in case it has not
    // it could initiate a refund. But this is probably outside of the scope of this task.

    /**
     * When booking, the invalid results are filtered out first. These are returned without checking
     * for free seats, since it is clearly a user error.
     * Then, the seats are checked for availability. This is returned without booking, if there are
     * seats that are not available, to offer the user the chance to book only the free ones.
     * If all seats are free, booking is done.
     *
     * @param   booking The requested seats in format "a1", "b2", etc.
     * @returns         Invalid seats if any (without any futher checks) or the free and non-free seats.
     *                  Returns all as invalid if more seats are trying to be booked than the capacity.
     */
    Booking book(const BookingInput& booking) {
        if(booking.empty()) {
            return {};
        }
        if(booking.size() > m_capacity) {
            return {{}, {}, booking};
        }

        Booking result;
        result.invalid = validateSeats(booking);
        if(result.invalid.size()) {
            return result;
        }

        std::vector<decltype(m_seatsFree)::iterator> freeSeats;
        freeSeats.reserve(booking.size());
        std::scoped_lock sl(m_bookingLock);
        for(const auto& bookedSeat : booking) {
            auto it = m_seatsFree.find(bookedSeat);
            if(it->second) {
                result.success.push_back(bookedSeat);
                freeSeats.push_back(it);
            } else {
                result.taken.push_back(bookedSeat);
            }
        }
        if(result.taken.empty()) {
            for(auto& it : freeSeats) {
                it->second = false;
                --m_capacity;
            }
        }
        return result;
    }

    /** Used to determine whether there are still tickets available. */
    int freeCapacity() const {
        return m_capacity;
    }

    /** Get a list of free seats in the theater. */
    Seats freeSeats() const {
        Seats result;
        for(const auto& seat : m_seatsFree) {
            if(seat.second) {
                result.push_back(seat.first);
            }
        }
        return result;
    }

    void clearSeats() {
        std::scoped_lock sl(m_bookingLock);
        for(auto& seat : m_seatsFree) {
            seat.second = true;
        }
        m_capacity = ROWS*COLS;
    }

private:
    void fillSeatNames() {
        for(int row = 0; row < ROWS; ++row) {
            for(int col = 1; col <= COLS; ++col) {
                std::stringstream ss;
                ss << char('a' + row) << col;
                m_seatsFree[ss.str()] = true;
            }
        }
    }

    /**
     * Validate requested seats.
     * @param   seats The requested seats in format "a1", "b2", etc.
     * @returns       Invalid seats
     */
    Seats validateSeats(const Seats& seats) const {
        Seats invalidSeats;
        for(const auto& seat : seats) {
            if(not isSeatValid(seat)) {
                invalidSeats.push_back(seat);
            }
        }
        return invalidSeats;
    }

    /** Make sure that seat exists in theater. */
    bool isSeatValid(const std::string& seat) const {
        return m_seatsFree.find(seat) != m_seatsFree.end();
    }

    const std::string m_name;
    std::unordered_map<std::string, bool> m_seatsFree;
    int m_capacity = ROWS*COLS;

    std::mutex m_bookingLock;
};