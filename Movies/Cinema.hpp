#pragma once

#include <algorithm>
#include <cctype>
#include <mutex>
#include <sstream>
#include <vector>

#include "../utils/utils.hpp"


struct Booking {
    std::vector<std::string> success;
    std::vector<std::string> taken;
    std::vector<std::string> doesntExist;
};

/**
 * The class that handles the seat assignments in a cinema and keeps track of which
 * movies are showing. This latter data is shared with the movie orchestrator.
 * 
 * Currently all cinemas have 20 seats in a 4x5 arrangement. This can be changed by
 * making the ROWxCOL dimensions configurable.
*/
class Cinema {
    struct RowCol {
        RowCol(int row, int col) : r(row), c(col) {}
        int r;
        int c;
    };

    /* Every cinema has 4 rows of 5 seats */
    static const int ROWS = 4;
    static const int COLS = 5;

public:
    Booking book(const std::vector<std::string>& booking);
    void registerMovie(int movieId);

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

    bool m_seats[ROWS][COLS] = { false };

    std::mutex m_bookingLock;

    // shared_ptr of movies shown, owned by this class
};