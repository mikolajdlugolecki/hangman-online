cmake_minimum_required(VERSION 3.19)
project(Client LANGUAGES CXX)

find_package(Qt6 6 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

qt_add_executable(Client
    main.cpp
    ConnectWidget.cpp
    ConnectWidget.h
    ConnectWidget.ui
        ../../include/common/MessageType.h
        ../../include/common/Message.h
        ../common/Serializer.cpp
        ../../include/common/Serializer.h
        ../common/Parser.cpp
        ../../include/common/Parser.h
        ../../include/common/Constants.h
        MainWindow.h MainWindow.cpp
        LoginWidget.ui
        LoginWidget.h LoginWidget.cpp
        LobbyWidget.ui
        LobbyWidget.h LobbyWidget.cpp
        TcpClient.h TcpClient.cpp
)

target_link_libraries(Client
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)

target_compile_options(Client PRIVATE -Wall -Wextra -g)
