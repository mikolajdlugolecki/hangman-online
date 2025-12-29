#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

class Client;
class Room;

namespace Utils
{
   int countLettersInWord(std::string word, char letter);
   Room *findRoom(const std::vector<std::unique_ptr<Room>> &rooms, const std::string &id);
   void writeDebugLog(const Client *client, const std::string &message);
}

#endif
