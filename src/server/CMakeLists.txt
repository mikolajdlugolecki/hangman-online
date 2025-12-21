add_executable(server server.cpp
        ../common.cpp
        ../common.h)

target_compile_options(server PRIVATE -Wall -Wextra -g)
