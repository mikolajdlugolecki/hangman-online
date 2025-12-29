#include "GameStats.h"

#include "Game.h"
#include "Room.h"

GameStats::GameStats(Room *room, std::string wordToGuess)
{
    this->room = room;
    this->fullWord = wordToGuess;

    std::string wordWithHiddenChars = std::string(wordToGuess);
    for (char &letter : wordWithHiddenChars)
    {
        if (letter != ' ')
        {
            letter = '_';
        }
    }
    this->wordWithHiddenChars = wordWithHiddenChars;
}

GameStats::~GameStats() {}

void GameStats::uncoverLetters(char letter)
{
    for (size_t i = 0; i < wordWithHiddenChars.size(); i++)
    {
        if (fullWord[i] == letter)
        {
            wordWithHiddenChars[i] = letter;
        }
    }
}

void GameStats::markLetterAsUsed(char letter)
{
    usedLetters.push_back(letter);
}

bool GameStats::isLetterUsed(char letter)
{
    for(char c : usedLetters)
    {
        if(c == letter)
        {
            return true;
        }
    }
    return false;
}

bool GameStats::isLetterCorrect(char letter)
{
    for(char c : fullWord)
    {
        if(c == letter)
        {
            return true;
        }
    }
    return false;
}

void GameStats::letterGuessed(std::string word, char letter)
{
    for (char guessedChar : guessedLetters)
    {
        if (guessedChar == letter)
        {
            return;
        }
    }

    numberOfGuessedLettersInWord += Utils::countLettersInWord(word, letter);
    guessedLetters.push_back(letter);

    uncoverLetters(letter);
}

void GameStats::recalculateScore()
{
    const int maxScore = 100;
    const int errorValueMultiplier = 2;

    int value = 1.0f * numberOfGuessedLettersInWord / room->game->word.size() * maxScore - errorValueMultiplier * errors;
    score = std::max(0, value);
}
