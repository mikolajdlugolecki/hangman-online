cmake_minimum_required(VERSION 3.12)

project(hangman-online LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src/server)

add_subdirectory(src/client)
