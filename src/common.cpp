#include "common.h"

int construct_response(const int socket, const Response::Type type, const std::string& payload) {
    const auto raw_type = static_cast<uint8_t>(type); // ALWAYS 1 BYTE
    write(socket, &raw_type, 1);
    const auto raw_size = htonl(static_cast<uint32_t>(payload.size())); // ALWAYS 4 BYTES
    write(socket, &raw_size, 4);
    write(socket, payload.c_str(), payload.size());
    return 0;
}