cmake_minimum_required(VERSION 3.15)
project(server)

set(CMAKE_CXX_STANDARD 17)


file(GLOB_RECURSE SERVER_SOURCES
        core/*.cpp
        logic/*.cpp
        network/*.cpp
        ../common/*.cpp
)

file(GLOB_RECURSE SERVER_HEADERS
        include/server/**/*.h
        include/common/*.h
)

add_executable(server
        ${SERVER_SOURCES}
        ${SERVER_HEADERS}
)

target_include_directories(server PRIVATE
        ${CMAKE_SOURCE_DIR}/include/server
        ${CMAKE_SOURCE_DIR}/include/common
)
