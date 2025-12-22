add_executable(server main.cpp
        ../common/MessageType.h
        ../common/Message.h
        ../common/Serializer.cpp
        ../common/Serializer.h
        ../common/Parser.cpp
        ../common/Parser.h
        ../common/Constants.h
        Server.cpp
        Server.h
        Client.cpp
        Client.h)

target_compile_options(server PRIVATE -Wall -Wextra -g)
