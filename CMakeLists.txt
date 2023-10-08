cmake_minimum_required(VERSION 3.26)
project(42_webserv)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(include/server)
include_directories(include/server/parser)

add_executable(42_webserv
        include/server/parser/ConfigParser.hpp
        include/server/Server.hpp
        include/server/Socket.hpp
        src/server/parser/ConfigParser.cpp
        src/server/Server.cpp
        src/server/Socket.cpp
        src/main.cpp)
