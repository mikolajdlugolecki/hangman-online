#ifndef GAMESTATS_H
#define GAMESTATS_H

#include "Utils.h"

#include <algorithm>
#include <vector>

class Room;

class GameStats
{
public:
    GameStats(Room *room, const std::string &wordToGuess);
    ~GameStats();

    Room *room;

    int errors = 0;
    int score = 0;
    int numberOfGuessedLettersInWord = 0;
    bool fullWordGuessed = false;
    std::vector<char> guessedLetters;
    std::vector<char> usedLetters;

    std::string wordWithHiddenChars;
    std::string fullWord;

    void markLetterAsUsed(char letter);
    bool isLetterUsed(char letter) const;
    std::string usedCharactersToString();

    bool isLetterCorrect(char letter) const;

    void letterGuessed(const std::string &word, char letter);
    void recalculateScore();

private:
    void uncoverLetters(char letter);
};

#endif
