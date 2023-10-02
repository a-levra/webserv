cmake_minimum_required(VERSION 3.26)
project(42_webserv)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(include/server)

add_executable(42_webserv
        include/server/Server.hpp
        include/server/Socket.hpp
        src/server/Server.cpp
        src/server/Socket.cpp
        src/main.cpp)
