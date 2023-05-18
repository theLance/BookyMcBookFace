#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/textbox.hpp>

#include "Client.hpp"


class NanaClient : public Client {
public:
    NanaClient(TheaterAdmin& ta) : Client(ta) {}

    void mainWindow() {
        nana::form win;
        nana::label title(win, "Please select an option\n\nSelect a movie:");

        if(m_movieCache.empty()) {
            m_movieCache = listAvailableMovies();
        }
        nana::combox titles(win);
        for(auto& title : m_movieCache) {
            titles.push_back(title);
        }

        titles.events().selected([this](const nana::arg_combox& sel){
            m_selectedMovie = sel.widget.caption();
        });

        nana::button refreshMovies(win, "Refresh Movies");
        refreshMovies.events().click([&]{
            nana::msgbox msg("List of currently playing movies");
            m_movieCache = listAvailableMovies();

            // this part should be a separate function, but then nana segfaults
            titles.clear();
            for(auto& title : m_movieCache) {
                titles.push_back(title);
            }

            msg << formatNames(m_movieCache);
            msg();
        });

        nana::button theaters(win, "Show theaters playing movie");
        theaters.events().click([this, &win]{
            auto available = listTheatersPlayingMovie(m_selectedMovie);
            nana::msgbox msg("List of theaters playing '" + m_selectedMovie + "'");
            msg << formatNames(available);
            msg();
        });

        nana::button seats(win, "Show free seats");
        seats.events().click([this, &win]{ showFreeSeats(); });

        nana::button book(win, "Book seats");
        book.events().click([this, &win]{ bookSeats(); });

        nana::button quit(win, "Quit");
        quit.events().click([&win]{ win.close(); });

        win.div("vertical all  min=260 gap=3 margin=5");
        win["all"] << title << titles << refreshMovies << theaters << seats << book << quit;
        win.collocate();
        win.show();
        nana::exec();
    }

private:
    void listTheaters() {
        if(m_movieCache.empty()) {
            m_movieCache = listAvailableMovies();
        }

        nana::form theaterWin;
        theaterWin.caption("List theaters");
        nana::combox titles(theaterWin);
        for(auto& title : m_movieCache) {
            titles.push_back(title);
        }

        titles.events().selected([this](const nana::arg_combox& sel){
            m_selectedMovie = sel.widget.caption();
            auto available = listTheatersPlayingMovie(m_selectedMovie);
            nana::msgbox msg("List of theaters playing '" + m_selectedMovie + "'");
            msg << formatNames(available);
            msg();
        });

        nana::button ok(theaterWin, "OK");
        ok.events().click([&theaterWin]{ theaterWin.close(); });

        theaterWin.div("vertical all  min=260 gap=3 margin=5");
        theaterWin["all"] << titles << ok;
        theaterWin.collocate();

        theaterWin.show();
        nana::exec();
    }

    bool checkIfMovieSelected() {
        if(m_selectedMovie.empty()) {
            nana::msgbox msg("ERROR");
            msg << "Please select a movie first!";
            msg();
            return false;
        }
        return true;
    }

    void comboxForTheaters(nana::combox& theaters) {
        auto available = listTheatersPlayingMovie(m_selectedMovie);
        for(auto& th : available) {
            theaters.push_back(th.name);
        }
    }

    void showFreeSeats() {
        nana::form seatsWin;
        seatsWin.caption("Show free seats in");
        nana::combox theaters(seatsWin);
        comboxForTheaters(theaters);

        theaters.events().selected([this](const nana::arg_combox& sel){
            auto name = sel.widget.caption();
            auto it = m_theatersForMovie.find(name);
            if(it == m_theatersForMovie.end()) {
                nana::msgbox msg("ERROR");
                msg << "Unfortunately " << name << " is no longer playing " << m_selectedMovie;
                msg();
                return;
            }

            auto available = listFreeSeatsForTheater(name);
            nana::msgbox msg("List of free seats for '" + m_selectedMovie + "'");
            msg << formatSeatNames(available);
            msg();
        });

        nana::button ok(seatsWin, "OK");
        ok.events().click([&seatsWin]{ seatsWin.close(); });

        seatsWin.div("vertical all  min=260 gap=3 margin=5");
        seatsWin["all"] << theaters << ok;
        seatsWin.collocate();

        if(not checkIfMovieSelected()) {
            seatsWin.close();
        }

        seatsWin.show();
        nana::exec();
    }

    void bookSeats() {
        nana::form seatsWin;
        seatsWin.caption("Book free seats in");
        nana::combox theaters(seatsWin);
        comboxForTheaters(theaters);

        decltype(m_theatersForMovie)::iterator it = m_theatersForMovie.end();
        theaters.events().selected([this, &it](const nana::arg_combox& sel){
            auto name = sel.widget.caption();
            it = m_theatersForMovie.find(name);
            if(it == m_theatersForMovie.end()) {
                nana::msgbox msg("ERROR");
                msg << "Unfortunately " << name << " is no longer playing " << m_selectedMovie;
                msg();
                return;
            }
        });

        nana::label guidance(seatsWin, "List which seats you would like to book, separated by a comma (,):");
        nana::textbox bookings(seatsWin);

        nana::button bookBtn(seatsWin, "BOOK");
        bookBtn.events().click([&seatsWin, &bookings, &it, this]{
                if(it == m_theatersForMovie.end()) {
                    nana::msgbox msg("ERROR");
                    msg << "Please select a theater first!";
                    msg();
                    return;
                }
                auto res = book(it->second->name(), bookings.text());
                if(res.taken.empty() && res.invalid.empty()) {
                    nana::msgbox msg("Booking SUCCESSFUL!");
                    msg << "Booked seats:\n" << formatSeatNames(res.success);
                    msg();
                } else {
                    nana::msgbox msg("Booking FAILURE");
                    if(res.invalid.size()) {
                        msg << "The following seats are invalid:\n" << formatSeatNames(res.invalid);
                    } else {
                        msg << "The following seats are taken:\n" << formatSeatNames(res.taken);
                    }
                    msg();
                }
        });

        nana::button back(seatsWin, "Back");
        back.events().click([&seatsWin]{ seatsWin.close(); });

        seatsWin.div("vertical all  min=260 gap=3 margin=5");
        seatsWin["all"] << theaters << guidance << bookings << bookBtn << back;
        seatsWin.collocate();

        if(not checkIfMovieSelected()) {
            seatsWin.close();
        }

        seatsWin.show();
        nana::exec();
    }

    std::string formatNames(std::vector<std::string> names) {
        std::sort(names.begin(), names.end());
        std::stringstream ss;
        for(auto& name : names) {
            ss << name << '\n';
        }
        return ss.str();
    }

    std::string formatNames(const AvailableTheaters& theaters) {
        std::vector<std::string> theatersWithCapacity;
        for(auto& theater : theaters) {
            theatersWithCapacity.push_back(theater.name + "  (free: " + std::to_string(theater.capacity) + ")");
        }
        return formatNames(theatersWithCapacity);
    }

    std::string formatSeatNames(const Seats& seats) {
        std::stringstream prettySeats;
        char prevRow = 'a';
        for(auto& seat : seats) {
            if(prevRow != seat[0]) {
                prettySeats << "\n";
                prevRow = seat[0];
            }
            prettySeats << seat << "  ";
        }
        return prettySeats.str();
    }

    std::string m_selectedMovie;

    MovieDb::Titles m_movieCache;
};
