#include "Game.h"

#include "Constants.h"

#include <fstream>
#include <random>

std::string Game::getGameStartedPayload() const
{
    return std::to_string(this->wordLength) + "|" + std::to_string(this->maxErrors) + "|" +
           std::to_string(this->maxSeconds) + "|" + wordWithHiddenChars;
}

static void stringToUpper(std::string &str)
{
    for (char &c : str) {
        c = std::toupper((unsigned char)c);
    }
}

std::vector<std::string> Game::loadWords(const std::string &fileName)
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
        stringToUpper(line);
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

    std::string wordWithHiddenChars = std::string(this->word);
    for (char &letter : wordWithHiddenChars)
    {
        if (letter != ' ')
        {
            letter = '_';
        }
    }
    this->wordWithHiddenChars = wordWithHiddenChars;

    this->wordLength = this->word.length();
    this->maxErrors = maxErrors;
    this->maxSeconds = maxSeconds;
}

std::vector<std::string> Game::availableWords = Game::loadWords(WORDS_FILE);
