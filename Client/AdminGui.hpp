#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/textbox.hpp>

#include "Movies/TheaterAdmin.hpp"


/**
 * The administrative GUI for the theaters and movies.
 * This is used to add and remove movie showings for theaters.
 */
class AdminGui {
public:
    AdminGui(TheaterAdmin& tha) : m_admin(tha) {}

    void mainWindow() {
        nana::form win;
        nana::label title(win, "Theater Admin GUI\n");

        nana::label thGuidance(win, "Theater name");
        nana::textbox theater(win);
        nana::label mvGuidance(win, "Movie name");
        nana::textbox movie(win);

        nana::button add(win, "Add movie");
        add.events().click([&]{
            if(not checkEmptyFields(theater.text(), movie.text())) {
                return;
            }
            m_admin.registerTheaterShowingMovie(theater.text(), movie.text());
            nana::msgbox msg("SUCCESS");
            msg << "Movie added successfully.";
            msg();
        });

        nana::button rem(win, "Remove movie");
        rem.events().click([&]{
            if(not checkEmptyFields(theater.text(), movie.text())) {
                return;
            }
            nana::msgbox resultMsg("Deletion result");
            switch(m_admin.removeMovieFromTheater(theater.text(), movie.text())) {
                case TheaterAdmin::OperationResult::Removal::MOVIE_NOT_FOUND:
                    resultMsg << "The movie doesn't exist in the DB.";
                    break;
                case TheaterAdmin::OperationResult::Removal::NO_THEATER_FOR_MOVIE:
                    resultMsg << "There are no theaters showing this movie.";
                    break;
                case TheaterAdmin::OperationResult::Removal::THEATER_NOT_SHOWING_MOVIE:
                    resultMsg << "The given theater is not showing the movie.";
                    break;
                case TheaterAdmin::OperationResult::Removal::SUCCESSFUL:
                    resultMsg << "Removed successfully.";
                    break;
                default:
                    break;
            };
            resultMsg();
        });

        nana::button quit(win, "Quit");
        quit.events().click([&win]{ win.close(); });

        win.div("vertical all  min=260 gap=3 margin=5");
        win["all"] << title << thGuidance << theater << mvGuidance << movie << add << rem << quit;

        win.collocate();
        win.show();
        nana::exec();
    }

private:
    bool checkEmptyFields(const std::string& th, const std::string& mv) {
        if(th.empty() || mv.empty()) {
            nana::msgbox msg("ERROR");
            msg << "Theater name and movie name are mandatory!";
            msg();
            return false;
        }
        return true;
    }

    TheaterAdmin& m_admin;
};
