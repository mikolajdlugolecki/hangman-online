#include "Utils.h"

#include "Client.h"
#include "Room.h"
#include "Serializer.h"

#include <iostream>

namespace Utils
{
int countLettersInWord(const std::string &word, const char letter)
{
    int count = 0;
    for (const char c : word)
    {
        if (c == letter)
        {
            count++;
        }
    }
    return count;
}

Room *findRoom(const std::vector<std::unique_ptr<Room>> &rooms, const std::string &id)
{
    for (auto &room : rooms)
    {
        if (room->id == id)
        {
            return room.get();
        }
    }
    return nullptr;
}

void writeDebugLog(const Client *client, const std::string &message)
{
    std::cout << client->addressToString() << " --- " << message << std::endl;
}

void writeMessageAsHex(const Message *message)
{
    const auto buf = Serializer::serialize(*message);
    for (const unsigned char c : buf)
    {
        std::cout << std::hex << static_cast<int>(c) << " ";
    }
    std::cout << std::endl;
}

}
