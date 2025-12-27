cmake_minimum_required(VERSION 3.15)
project(server)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SERVER_SOURCES
        *.cpp
        ../common/*.cpp
)

file(GLOB_RECURSE SERVER_HEADERS
        *.h
        ../common/*.h
)

file(GLOB RESOURCE_FILES
        ../../resources/*
)

file(COPY ${RESOURCE_FILES}
        DESTINATION ${CMAKE_BINARY_DIR}
)

add_executable(server
        ${SERVER_SOURCES}
        ${SERVER_HEADERS}
)

target_include_directories(server PRIVATE
        ../common
)

target_compile_options(server PRIVATE -Wall -Wextra -g)
