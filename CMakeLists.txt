cmake_minimum_required(VERSION 3.26)
project(42_webserv)

set(CMAKE_CXX_STANDARD 98)

include_directories(include)
include_directories(include/server)

add_executable(42_webserv
        include/server/Server.hpp
        include/server/Socket.hpp
        include/virtualServer/VirtualServer.hpp
        include/virtualServer/Location.hpp
        include/utils/utils.hpp
        src/server/Server.cpp
        src/server/Socket.cpp
        src/main.cpp
        src/HttpRequest.cpp)
        src/HttpRequest.cpp
        src/virtualServer/VirtualServer.cpp
        src/virtualServer/Location.cpp
        src/utils/readFileToString.cpp
)
