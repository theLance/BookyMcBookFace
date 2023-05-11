#include "Movies/Cinema.hpp"
#include "Movies/Movies.hpp"
#include <string>


class Runner {
public:
    void start();
    void stop();
private:
};

int main() {
    Cinema sut;
    std::vector<std::string> fullCapacity = {"a1", "a2", "a3", "a4", "a5",
                                             "b1", "b2", "b3", "b4", "b5",
                                             "c1", "c2", "c3", "c4", "c5",
                                             "d1", "d2", "d3", "d4", "d5"};
    auto result = sut.book(fullCapacity);


    Runner r;
    MovieDb mdb;
    MovieOrchestrator mo;
    return 0;
}