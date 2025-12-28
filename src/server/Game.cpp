#include "Game.h"

#include <fstream>
#include <random>

#include "Constants.h"

Game* Game::create(const unsigned short int maxErrors, const unsigned short int maxSeconds)
{
    return new Game(maxErrors, maxSeconds);
}

std::string Game::getGameStartedPayload() const
{
    return std::to_string(this->wordLength) + "|" + std::to_string(this->maxErrors) + "|" + std::to_string(this->maxSeconds);
}

std::string Game::letterInWord(const std::string& letter)
{
    std::vector<size_t> positions;

    size_t position = this->word.find(letter, 0);
    while(position != std::string::npos)
    {
        positions.push_back(position);
        position = this->word.find(letter, position + 1);
    }

    std::string positionsAsString;
    for (size_t i = 0; i < positions.size(); i++)
    {
        positionsAsString += std::to_string(positions[i]);
        if (i < positions.size() - 1)
        {
            positionsAsString += "|";
        }
    }
    return positionsAsString;
}

std::vector<std::string> Game::loadWords(const std::string& fileName)
{
    std::fstream file;
    file.open(fileName, std::ios::in);

    if (!file.good())
    {
        perror("Error opening file with words");
        exit(EXIT_FAILURE);
    }

    std::vector<std::string> result;
    std::string line;
    while (std::getline(file, line))
    {
        result.push_back(line);
    }

    file.close();
    return result;
}

Game::Game(const unsigned short int maxErrors, const unsigned short int maxSeconds)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, Game::availableWords.size() - 1);

    this->word = Game::availableWords[dis(gen)];
    this->wordLength = this->word.length();
    this->maxErrors = maxErrors;
    this->maxSeconds = maxSeconds;
}

std::vector<std::string> Game::availableWords = Game::loadWords(WORDS_FILE);
