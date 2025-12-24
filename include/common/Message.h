#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <string>

struct Message {
    uint8_t type;
    uint32_t length;
    std::string payload;
};

#endif