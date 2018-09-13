cmake_minimum_required(VERSION 2.8.12)

project(ParkingLot)

option(USE_QT "Allow Qt to build graph and json support" TRUE)
if (USE_QT)
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	set(CMAKE_AUTOMOC ON)

	set (CMAKE_CXX_STANDARD 14)
	add_definitions(-D USE_QT)
	if (MSVC)
		set(CMAKE_PREFIX_PATH "c:\\Qt\\5.11.1\\msvc2017_64")
	endif()

	find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Charts)
endif()

add_executable(${PROJECT_NAME} "main.cpp")

if (USE_QT)
	target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Charts)
endif()
