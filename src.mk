LIST_SRC	=\
		cgi/CGI.cpp	\
		\
		config/ConfigContext.cpp	\
		config/ConfigFactory.cpp	\
		config/ConfigLexer.cpp		\
		config/ConfigParser.cpp		\
		\
        HttpMessages/AHttpMessage.cpp		\
        HttpMessages/HttpRequest.cpp		\
        HttpMessages/HttpResponse.cpp		\
        HttpMessages/HttpResponseCGI.cpp	\
        \
        logger/Logger.cpp	\
        logger/logging.cpp	\
        \
        server/Client.cpp	\
        server/Server.cpp	\
        server/Socket.cpp	\
        \
        utils/getDate.cpp 				\
        utils/isDigitString.cpp			\
        utils/printBold.cpp 			\
        utils/readFileToString.cpp		\
        utils/sleep.cpp					\
        utils/split.cpp					\
        utils/trim.cpp					\
        utils/fileExist.cpp				\
        utils/createFile.cpp			\
        utils/getPositionsOfWord.cpp	\
        utils/isDirectory.cpp			\
        \
        virtualServer/Location.cpp 		\
        virtualServer/VirtualServer.cpp	\
        \
        argument.cpp	\
        main.cpp		\
        options.cpp