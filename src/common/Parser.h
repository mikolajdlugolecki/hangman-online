#ifndef PARSER_H
#define PARSER_H

#include "Message.h"

#include <cstdint>
#include <vector>

enum ParserState
{
    READ_TYPE,
    READ_LENGTH,
    READ_PAYLOAD
};

class Parser
{
public:
    bool parse(std::vector<char> &buffer, Message *out);
    static std::vector<std::string> splitMessage(const std::string &payload);
    static std::vector<std::vector<std::string>> splitGameStateMessage(const std::string &payload);

private:
    ParserState nextState = ParserState::READ_TYPE;
    uint32_t expectedLength = 0;
};

#endif
