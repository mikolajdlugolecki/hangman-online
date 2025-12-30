#ifndef GAME_H
#define GAME_H

#include <chrono>
#include <string>
#include <vector>

class Game
{
public:
    unsigned short int wordLength;
    unsigned short int maxErrors;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::seconds duration;
    bool inProgress;

    Game(unsigned short int maxErrors, unsigned int duration);
    std::string getGameStartedPayload() const;
    void start();
    void update();
    void stop();
    std::chrono::seconds getRemainingTime() const;

    std::string word;
    std::string wordWithHiddenChars;

private:
    static std::vector<std::string> availableWords;

    static std::vector<std::string> loadWords(const std::string &fileName);
};

#endif
