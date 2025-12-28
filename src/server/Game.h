#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

class Game
{
public:
    unsigned short int wordLength;
    unsigned short int maxErrors;
    unsigned short int maxSeconds;

    static Game* create(unsigned short int maxErrors, unsigned short int maxSeconds);
    std::string getGameStartedPayload() const;
    std::string letterInWord(const std::string& letter);
private:
    std::string word;
    static std::vector<std::string> availableWords;

    Game(unsigned short int maxErrors, unsigned short int maxSeconds);
    static std::vector<std::string> loadWords(const std::string& fileName);
};

#endif
