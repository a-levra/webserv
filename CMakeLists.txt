cmake_minimum_required(VERSION 3.26)
project(42_webserv)

set(CMAKE_CXX_STANDARD 98)

include_directories(include)
include_directories(include/config)
include_directories(include/server)

add_executable(42_webserv
        include/config/ConfigContext.hpp
        include/config/ConfigLexer.hpp
        include/config/ConfigParser.hpp
        include/server/Server.hpp
        include/server/Socket.hpp
        include/utils.hpp
        src/config/ConfigContext.cpp
        src/config/ConfigLexer.cpp
        src/config/ConfigParser.cpp
        src/server/Server.cpp
        src/server/Socket.cpp
        src/utils/isDigitString.cpp
        src/utils/split.cpp
        src/utils/trim.cpp
        src/main.cpp)
