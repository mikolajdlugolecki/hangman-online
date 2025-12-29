#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "Message.h"

class Client;
class Room;

namespace Utils
{
   int countLettersInWord(std::string word, char letter);
   Room *findRoom(const std::vector<std::unique_ptr<Room>> &rooms, const std::string &id);
   void writeDebugLog(const Client *client, const std::string &message);
   void writeMessageAsHex(const Message *message);
}

#endif
