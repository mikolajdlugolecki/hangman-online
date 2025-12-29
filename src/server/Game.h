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

    Game(unsigned short int maxErrors, unsigned short int maxSeconds);
    std::string getGameStartedPayload() const;

    std::string word;
    std::string wordWithHiddenChars;

private:
    static std::vector<std::string> availableWords;

    static std::vector<std::string> loadWords(const std::string &fileName);
};

#endif
