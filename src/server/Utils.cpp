#include "Utils.h"

#include "Room.h"
#include "Client.h"

namespace Utils
{
int countLettersInWord(std::string word, char letter)
{
    int count = 0;
    for (char c : word)
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

} // namespace Utils
