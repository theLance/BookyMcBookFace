#pragma once
#include <iostream>

#include <string>
#include <vector>

#include <boost/bimap/bimap.hpp>
#include <boost/bimap/set_of.hpp>

class MovieOrchestrator {
    // cinemas showing movies
    // movie can only be removed if no cinemas are showing it
    // should include cleanse() where titles no cinemas are showing are removed
};

namespace bm = ::boost::bimaps;

/** If two movies have the same name, the production year must be added, otherwise insertion will fail. */
class MovieDb {
    struct title_t {};
    struct id_t {};

    typedef bm::bimap<bm::set_of<bm::tagged< std::string, title_t >>,
                      bm::set_of<bm::tagged< int, id_t >>
                      > MovieMap;
public:
    static const int ID_NOT_FOUND = -1;

    MovieDb() {
        loadMovieDb();
    }

    ~MovieDb() {
        if(m_moviesChangedSinceLoad) {
            commitMovieDb();
        }
    }

    std::vector<std::string> listMovies() const {
        if(m_moviesChanged) {
            reloadMovieTitleList();
        }
        return m_movieTitles;
    }

    int getMovieIdForTitle(const std::string& title) {
        auto it = m_movieIds.by<title_t>().find(title);
        if(it == m_movieIds.by<title_t>().end()) {
            return ID_NOT_FOUND;
        }
        return it->get_right();
    }

    std::string getMovieTitleForId(int id) {
        auto it = m_movieIds.by<id_t>().find(id);
        if(it == m_movieIds.by<id_t>().end()) {
            return "";
        }
        return it->get_left();
    }

    void addMovie(const std::string& title) {
        m_moviesChangedSinceLoad = true;
        m_moviesChanged = true;
        m_movieIds.insert(MovieMap::value_type(title, getFreeId()));
    }

    void removeMovie(const std::string& title) {
        m_moviesChangedSinceLoad = true;
        m_moviesChanged = true;
        m_movieIds.left.erase(title);
    }

private:
    void loadMovieDb() {}
    void commitMovieDb() {} // if we can manually commit, reset bool

    void reloadMovieTitleList() const {
        m_movieTitles.clear();
        for(auto& movie : m_movieIds)
        {
            m_movieTitles.push_back(movie.get_left());
        }
        m_moviesChanged = false;
    }

    int getFreeId() const {
        int available = -1;
        while(m_movieIds.by<id_t>().find(++available) != m_movieIds.by<id_t>().end());
        return available;
    }

    MovieMap m_movieIds;

    bool m_moviesChangedSinceLoad = false;

    mutable bool m_moviesChanged = false;
    mutable std::vector<std::string> m_movieTitles;
};