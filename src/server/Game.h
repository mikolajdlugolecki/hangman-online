#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

class Game {
public:
    unsigned short int word_length;
    unsigned short int max_errors;
    static Game* create(unsigned short int max_errors);
    std::string getGameStartedPayload();
private:
    std::string word;
    static std::vector<std::string> available_words;
    static std::vector<std::string> loadWords(const std::string& file_name);
    Game(unsigned short int max_errors);
};

#endif
