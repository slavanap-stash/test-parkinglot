cmake_minimum_required(VERSION 2.8.12)

project(ParkingLot)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Charts)

add_definitions(-D USE_QT)

add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Charts)
