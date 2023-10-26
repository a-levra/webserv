cmake_minimum_required(VERSION 3.26)
project(42_webserv)

set(CMAKE_CXX_STANDARD 98)

include_directories(include)
include_directories(include/config)
include_directories(include/server)
include_directories(include/HttpMessages)
include_directories(include/utils)
include_directories(include/virtualServer)

add_executable(42_webserv
        include/config/ConfigContext.hpp
        include/config/ConfigLexer.hpp
        include/config/ConfigParser.hpp
        include/server/Server.hpp
        include/server/Socket.hpp
        include/virtualServer/VirtualServer.hpp
        include/virtualServer/Location.hpp
        include/utils/utils.hpp
        src/config/ConfigContext.cpp
        src/config/ConfigLexer.cpp
        src/config/ConfigParser.cpp
        src/server/Server.cpp
        src/server/Socket.cpp
        src/main.cpp
        src/virtualServer/VirtualServer.cpp
        src/virtualServer/Location.cpp
        src/utils/readFileToString.cpp
        src/utils/coloredLog.cpp
        src/utils/getDate.cpp
        src/utils/printBold.cpp
        src/HttpMessages/AHttpMessage.cpp
        src/HttpMessages/HttpResponse.cpp
        src/HttpMessages/HttpRequest.cpp
        src/virtualServer/VirtualServer.cpp
        src/virtualServer/Location.cpp
        src/utils/isDigitString.cpp
        src/utils/split.cpp
        src/utils/trim.cpp
        src/virtualServer/VirtualServer.cpp
        src/virtualServer/Location.cpp
        src/utils/readFileToString.cpp
        src/config/ConfigFactory.cpp
        include/config/ConfigFactory.hpp
        src/config/ConfigFactory.cpp
        src/utils/sleep.cpp
        src/utils/fileExist.cpp
        src/utils/createFile.cpp
        src/server/Client.cpp
        include/server/Client.hpp
        src/logger/Logger.cpp
        include/logger/logging.hpp
)
