#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "Message.h"

#include <vector>

class Serializer
{
public:
    static std::vector<char> serialize(const Message &message);
};

#endif
