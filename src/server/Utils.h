#ifndef UTILS_H
#define UTILS_H

#include "Message.h"

#include <memory>
#include <string>
#include <vector>

class Client;
class Room;

namespace Utils
{
int countLettersInWord(const std::string &word, char letter);
Room *findRoom(const std::vector<std::unique_ptr<Room>> &rooms, const std::string &id);
void writeDebugLog(const std::shared_ptr<Client>& client, const std::string &message);
void writeMessageAsHex(const Message *message);
}

#endif
