#include "Movies/TheaterAdmin.hpp"
#include "Client/NanaClient.hpp"
#include "Client/AdminGui.hpp"

#include <thread>


int main() {
    TheaterAdmin mo;

    std::thread client([&mo]{
        NanaClient nc(mo);
        nc.mainWindow();
    });

    AdminGui adg(mo);
    adg.mainWindow();

    client.join();

    return 0;
}
