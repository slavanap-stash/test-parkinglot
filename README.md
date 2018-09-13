
## ParkingLot assignment

### Build and run instructions

#### Basic requirements part (text file)

On Windows:
* use `configure.bat` to generate __build/ParkingLot.sln MSVS 2017 solution file

* **OR** use any C++14 compiler to build main.cpp
 
On Linux:
* use `cmake . -DUSE_QT=FALSE` to generate Makefile, then run `make` and `./ParkingLot test.txt`

* **OR** `g++ -Wall main.cpp`

The app outputs busiest hours intervals and plotting data to console.

#### Additional requirements part (json file and visualization)

On Windows:
* Install Qt 5.11.1 with msvc2017_64 framework, open CMakeLists.txt with Qt Creator, build and run. Additional command line argument with input json file required.

On Ubuntu Bionic:
 *  `sudo apt-get install build-essential cmake qt5-default libqt5charts5-dev && cmake . && make && ./ParkingLot test.json`
 *  use .gitlab-ci.yml as a reference 

The app outputs busiest hours intervals to console and draws plot data with Qt5Graphs.
