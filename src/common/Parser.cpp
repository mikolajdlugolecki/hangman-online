#include "Parser.h"

#include <cstring>
#include <string>
#include <netinet/in.h>
#include "Constants.h"

bool Parser::parse(std::vector<char> &buffer, Message *out) {
    while (true) {
        if (this->next_state == ParserState::READ_TYPE) {
            if (buffer.size() < TYPE_SIZE) return false;
            auto type = static_cast<uint8_t>(buffer[0]);
            out->type = type;
            buffer.erase(buffer.begin());
            this->next_state = ParserState::READ_LENGTH;
        }
        if (this->next_state == ParserState::READ_LENGTH) {
            if (buffer.size() < LENGTH_SIZE) return false;
            uint32_t length;
            memcpy(&length, buffer.data(), LENGTH_SIZE);
            length = ntohl(length);
            out->length = length;
            this->expected_length = length;
            buffer.erase(buffer.begin(), buffer.begin() + LENGTH_SIZE);
            this->next_state = ParserState::READ_PAYLOAD;
        }
        if (this->next_state == ParserState::READ_PAYLOAD) {
            if (buffer.size() < this->expected_length) return false;
            std::vector<char> payload(buffer.begin(), buffer.begin() + this->expected_length);
            buffer.erase(buffer.begin(), buffer.begin() + this->expected_length);
            std::string text(payload.begin(), payload.end());
            out->payload = text;
            this->next_state = ParserState::READ_TYPE;
            this->expected_length = 0;
        }
        return true;
    }
}
