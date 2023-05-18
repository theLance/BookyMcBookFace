# Booky McBookFace

This is a program used to book shows in theaters.
There is an administrative GUI where one can add and remove shows and a client interface to
 list ongoing shows, check which theaters are playing them, check the free seats for said shows,
 and to book seats for these shows.

## Installation

### Prerequisites

This program requires the boost, gtest, gmock, and nana libraries to compile.

Technically, boost includes are enough, but they must be in the search path. Otherwise, please
 extend the list of includes (CMPFLG) in the Makefile.

GTest and GMock must be installed and in the searchpath, otherwise the include flags (CMPFLG)
 and the link flags for tests (TSTLNKFLG) must be extended. If testing is not necessary, this
 can be skipped. (Ie. the program compiles just fine without GTest/GMock).

The Nana library is thought to be, by default, to be one level above the app directory, but this
 can be overridden using the Nana library path make variable (NANALIBPATH). Note that the library
 must be in this directory. The includes for Nana are assumed to be in the include/ directory
 that is directly in this directory. If not, please also override the NANAINCLUDE variable.

### Command to install

Once everything is set up, simply use the following command:
```bash
make run
```
To make and run the unit tests, use the following command:
```bash
make tst
```
To clean up the artifacts created by builds (including the executables), use:
```bash
make clean
```
NOTE: The current deletion command is `rm`. If this is not supported on your OS, override RMCMD.

## Usage

The app will launch two GUI windows: one for the admin and one for the client. Due to the nature of the
 GUI library used, these will be stacked.

Use the admin to add/remove Theater/Movie combinations. Note that these go hand-in-hand, there is no
 Theater without a Movie, or Movie that is not in a Theater. One Movie can be in multiple Theaters,
 and mutliple Theaters can play a Movie. But once no Theater is playing a Movie, it will be removed from
 the database.


## Developer notes

There could've been a better separation of the admin class (ie. Client should not be able to call all
 public functions), but since the GUI hides this and time was short, I decided to leave it as is.

In a similar manner, due to time constraints, I did not implement the feature, which would ask the user
 if they want to book only the free seats, in case there were no invalid ones, but some were taken.
 Since this was not a part of the specs, I thought it best to not spend more time on it.

It was not clear to me how the backend should connect to the frontend, so I made a simple GUI for the
 client of which currently one is created in the main() function. This number can be increased, but it
 is assumed that there is only one admin.
