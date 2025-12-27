#ifndef PARSER_H
#define PARSER_H

#include <cstdint>
#include <vector>

#include "Message.h"

enum ParserState {
    READ_TYPE,
    READ_LENGTH,
    READ_PAYLOAD
};

class Parser {
public:
    bool parse(std::vector<char> &buffer, Message *out);
    std::vector<std::string> split_message(const std::string& payload);
private:
    ParserState next_state = ParserState::READ_TYPE;
    uint32_t expected_length = 0;
};

#endif
