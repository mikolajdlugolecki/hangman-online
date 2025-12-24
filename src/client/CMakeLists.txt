cmake_minimum_required(VERSION 3.19)
project(Client LANGUAGES CXX)

find_package(Qt6 6 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

set(CLIENT_SOURCES
        main.cpp

        TcpClient.cpp

        MainWindow.cpp
        ConnectWidget.cpp
        LobbyWidget.cpp
        LoginWidget.cpp

        ../common/Parser.cpp
        ../common/Serializer.cpp
)

set(CLIENT_HEADERS
        TcpClient.h

        MainWindow.h
        ConnectWidget.h
        LobbyWidget.h
        LoginWidget.h

        ../common/Parser.h
        ../common/Serializer.h
        ../common/Constants.h
        ../common/Message.h
        ../common/MessageType.h
)

set(CLIENT_UI
        ConnectWidget.ui
        LobbyWidget.ui
        LoginWidget.ui
)

qt_add_executable(Client
        ${CLIENT_SOURCES}
        ${CLIENT_HEADERS}
        ${CLIENT_UI}
)

target_include_directories(Client PRIVATE
        ../common
)

target_link_libraries(Client
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)

target_compile_options(Client PRIVATE -Wall -Wextra -g)
