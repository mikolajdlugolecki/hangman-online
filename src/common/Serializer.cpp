#include "Serializer.h"

#include <netinet/in.h>

#include "Constants.h"

std::vector<char> Serializer::serialize(const Message& message)
{
    std::vector<char> buffer;
    buffer.push_back(static_cast<uint8_t>(message.type));
    uint32_t len = htonl(static_cast<uint32_t>(message.length));
    buffer.insert(buffer.end(), reinterpret_cast<char*>(&len), reinterpret_cast<char*>(&len) + LENGTH_SIZE);
    buffer.insert(buffer.end(), message.payload.begin(), message.payload.end());
    return buffer;
}
