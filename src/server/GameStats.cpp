#include "GameStats.h"

#include "Game.h"
#include "Room.h"

GameStats::GameStats(Room *room, const std::string &wordToGuess)
{
    this->room = room;
    this->fullWord = wordToGuess;

    auto wordWithHiddenCharacters = std::string(wordToGuess);
    for (char &letter : wordWithHiddenCharacters)
    {
        if (letter != ' ')
        {
            letter = '_';
        }
    }
    this->wordWithHiddenChars = wordWithHiddenCharacters;
}

GameStats::~GameStats() = default;

void GameStats::uncoverLetters(const char letter)
{
    for (size_t i = 0; i < this->wordWithHiddenChars.size(); i++)
    {
        if (this->fullWord[i] == letter)
        {
            this->wordWithHiddenChars[i] = letter;
        }
    }
}

std::string GameStats::usedCharactersToString()
{
    std::string s = "";
    for (char c : usedLetters)
    {
        s += c;
    }
    return s;
}

void GameStats::markLetterAsUsed(const char letter)
{
    this->usedLetters.push_back(letter);
}

bool GameStats::isLetterUsed(const char letter) const
{
    for (const char c : this->usedLetters)
    {
        if (c == letter)
        {
            return true;
        }
    }
    return false;
}

bool GameStats::isLetterCorrect(const char letter) const
{
    for (const char c : this->fullWord)
    {
        if (c == letter)
        {
            return true;
        }
    }
    return false;
}

void GameStats::letterGuessed(const std::string &word, const char letter)
{
    for (const char guessedChar : this->guessedLetters)
    {
        if (guessedChar == letter)
        {
            return;
        }
    }

    this->numberOfGuessedLettersInWord += Utils::countLettersInWord(word, letter);
    this->guessedLetters.push_back(letter);

    if (this->numberOfGuessedLettersInWord == static_cast<int>(word.length()))
    {
        this->fullWordGuessed = true;
    }

    uncoverLetters(letter);
}

void GameStats::recalculateScore()
{
    constexpr int maxScore = 100;
    constexpr int errorValueMultiplier = 2;

    const int value = 1.0f * this->numberOfGuessedLettersInWord / this->room->game->word.size() * maxScore -
                      errorValueMultiplier * this->errors;
    this->score = std::max(0, value);
}
