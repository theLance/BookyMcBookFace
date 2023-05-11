#include "Movies/Movies.hpp"
#include <string>


class Runner {
public:
    void start();
    void stop();
private:
};

int main() {
    Runner r;
    MovieDb mdb;
    MovieOrchestrator mo;
    return 0;
}