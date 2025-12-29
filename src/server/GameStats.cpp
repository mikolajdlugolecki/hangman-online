#include "GameStats.h"

#include "Room.h"
#include "Game.h"

GameStats::GameStats(Room *room)
{
    this->room = room;
}

GameStats::~GameStats() 
{

}

void GameStats::letterGuessed(std::string word, char c)
{
    for(char guessedChar : guessedLetters)
    {
        if(guessedChar == c)
        {
            return;
        }
    }

    numberOfGuessedLettersInWord += Utils::countLettersInWord(word, c);
    guessedLetters.push_back(c);
}

void GameStats::recalculateScore()
{
    int value = 1.0f * numberOfGuessedLettersInWord / room->game->word.size() * 100 - 2 * errors;
    score = std::max(0, value);
}