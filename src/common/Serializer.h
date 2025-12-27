#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <vector>

#include "Message.h"

class Serializer {
public:
    static std::vector<char> serialize(const Message& message);
};

#endif
