cmake_minimum_required(VERSION 3.26)
project(webserv)

set(CMAKE_CXX_STANDARD 98)

add_executable(ServerTestTdameros
        main.cpp
        Server.cpp
        Server.hpp
        AHttpParser.cpp
        AHttpParser.hpp)
add_executable(DEBUG_Parse_request
        DEBUG_Parse_request.cpp
        AHttpParser.cpp
        AHttpParser.hpp)
