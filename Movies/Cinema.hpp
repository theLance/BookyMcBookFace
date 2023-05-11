#pragma once

#include <algorithm>
#include <cctype>
#include <mutex>
#include <vector>

#include "../utils/utils.hpp"


struct Booking {
    std::vector<std::string> success;
    std::vector<std::string> taken;
    std::vector<std::string> invalid;
};

struct RowCol {
    RowCol(int row, int col) : r(row), c(col) {}
    int r;
    int c;
};

/**
 * The class that handles the seat assignments in a cinema. This is technically not
 * a cinema, only one premiere in it. This way, multiple movies can be booked at the
 * same time, with the least amount of blocking.
 *
 * Currently all cinemas have 20 seats in a 4x5 arrangement. This can be changed by
 * making the ROWxCOL dimensions configurable.
 */
class Cinema {

    /* Every cinema has 4 rows of 5 seats */
    static const int ROWS = 4;
    static const int COLS = 5;

public:
    Cinema(const std::string& name)
            : m_name(name) {
        clearSeats();
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
    Booking book(const std::vector<std::string>& booking) {
        if(booking.size() > m_capacity) {
            return {{}, {}, booking};
        }

        Booking result;
        result.invalid = validateSeats(booking);
        if(result.invalid.size()) {
            return result;
        }

        auto seatsRequested = convertToRowCol(booking);
        std::scoped_lock sl(m_bookingLock);
        for(auto idx = 0; idx < seatsRequested.size(); ++idx) {
            if(m_seatFree[seatsRequested[idx].r][seatsRequested[idx].c]) {
                result.success.push_back(booking[idx]);
            } else {
                result.taken.push_back(booking[idx]);
            }
        }
        if(result.taken.empty()) {
            for(const auto& seat : seatsRequested) {
                m_seatFree[seat.r][seat.c] = false;
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
        for(auto& row : m_seatFree) {
            for(auto& seat : row) {
                seat = true;
            }
        }
        m_capacity = ROWS*COLS;
    }

private:
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

    /**
     * Make sure that seat exists in cinema.
     * It is assumed that no theatre will have more than 26 rows, so only one letter will be
     * at the beginning. Numbering is 1-indexed.
     */
    bool isSeatValid(const std::string& seat) const {
        // min requirements are met
        if(seat.empty() || seat.size() < 2 || not std::isalpha(seat[0])) {
            return false;
        }
        // row exists
        if(int(std::tolower(seat[0]) - 'a') >= ROWS) {
            return false;
        }
        // numeric part fully numeric
        std::string numPart(seat.begin() + 1, seat.end());
        if(std::any_of(numPart.begin(), numPart.end(), [](int c){ return not std::isdigit(c); })) {
            return false;
        }
        int col(strToInt(numPart));
        return col > 0 && col <= COLS;
    }

    /// Converts the seat strings to row/col. Assumes seat number is valid. Cols are 1-indexed!
    std::vector<RowCol> convertToRowCol(const std::vector<std::string>& seats) const {
        std::vector<RowCol> rcv;
        for(const auto& seat : seats) {
            rcv.emplace_back( int(std::tolower(seat[0]) - 'a'), strToInt({seat.begin()+1, seat.end()}) - 1 );
        }
        return rcv;
    }

    const std::string m_name;

    bool m_seatFree[ROWS][COLS];
    int m_capacity = ROWS*COLS;

    std::mutex m_bookingLock;
};