cmake_minimum_required(VERSION 3.19)
project(Client LANGUAGES CXX)

find_package(Qt6 6 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

qt_add_executable(Client
    main.cpp
    Client.cpp
    Client.h
    Client.ui
        ../common/MessageType.h
        ../common/Message.h
        ../common/Serializer.cpp
        ../common/Serializer.h
        ../common/Parser.cpp
        ../common/Parser.h
        ../common/Constants.h
)

target_link_libraries(Client
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)

target_compile_options(Client PRIVATE -Wall -Wextra -g)
