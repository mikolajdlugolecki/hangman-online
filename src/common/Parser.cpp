#include "Parser.h"

#include <cstring>
#include <sstream>
#include <string>
#include <netinet/in.h>

#include "Constants.h"

bool Parser::parse(std::vector<char> &buffer, Message *out)
{
    if (this->nextState == ParserState::READ_TYPE)
    {
        if (buffer.size() < TYPE_SIZE)
        {
            return false;
        }
        const auto type = static_cast<uint8_t>(buffer[0]);
        out->type = type;
        buffer.erase(buffer.begin());
        this->nextState = ParserState::READ_LENGTH;
    }

    if (this->nextState == ParserState::READ_LENGTH)
    {
        if (buffer.size() < LENGTH_SIZE)
        {
            return false;
        }
        uint32_t length;
        memcpy(&length, buffer.data(), LENGTH_SIZE);
        length = ntohl(length);
        out->length = length;
        this->expectedLength = length;
        buffer.erase(buffer.begin(), buffer.begin() + LENGTH_SIZE);
        this->nextState = ParserState::READ_PAYLOAD;
    }
    
    if (this->nextState == ParserState::READ_PAYLOAD)
    {
        if (buffer.size() < this->expectedLength)
        {
            return false;
        }
        std::vector<char> payload(buffer.begin(), buffer.begin() + this->expectedLength);
        buffer.erase(buffer.begin(), buffer.begin() + this->expectedLength);
        const std::string text(payload.begin(), payload.end());
        out->payload = text;
        this->nextState = ParserState::READ_TYPE;
        this->expectedLength = 0;
    }

    return true;
}

std::vector<std::string> Parser::splitMessage(const std::string& payload)
{
    std::vector<std::string> result;
    std::string token;
    std::stringstream ss(payload);
    while (std::getline(ss, token, '|'))
    {
        result.push_back(token);
    }
    return result;
}

std::vector<std::vector<std::string>> Parser::splitGameStateMessage(const std::string &payload) {
    std::vector<std::vector<std::string>> result;
    const std::vector<std::string> playersStats = Parser::splitMessage(payload);
    for (auto& playerString : playersStats) {
        std::vector<std::string> playerStats;
        std::string token;
        std::stringstream ss(playerString);
        while (std::getline(ss, token, ':'))
        {
            playerStats.push_back(token);
        }
        result.push_back(playerStats);
    }
    return result;
}
