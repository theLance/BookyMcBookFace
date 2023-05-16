#pragma once

#include <algorithm>
#include <cctype>
#include <mutex>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "../InputHandling/InputSanitizer.hpp"


struct Booking {
    std::vector<std::string> success;
    std::vector<std::string> taken;
    std::vector<std::string> invalid;
};

/**
 * The class that handles the seat assignments in a theater. This is technically not
 * a theater, only one premiere in it. This way, multiple movies can be booked at the
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
    int freeSeats() const {
        return m_capacity;
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
    std::vector<std::string> validateSeats(const std::vector<std::string>& seats) const {
        std::vector<std::string> invalidSeats;
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