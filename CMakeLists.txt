cmake_minimum_required(VERSION 3.26)
project(ServerTestTdameros)

set(CMAKE_CXX_STANDARD 98)

add_executable(ServerTestTdameros
        main.cpp
        Server.cpp
        Server.hpp)
