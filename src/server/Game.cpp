#include "Game.h"

#include "Constants.h"

#include <fstream>
#include <random>

std::string Game::getGameStartedPayload() const
{
    return std::to_string(this->wordLength) + "|" + std::to_string(this->maxErrors) + "|" +
           std::to_string(static_cast<int>(this->duration.count())) + "|" + wordWithHiddenChars;
}

void Game::start()
{
    this->startTime = std::chrono::steady_clock::now();
}

void Game::update()
{
    if (auto now = std::chrono::steady_clock::now(); now - this->startTime >= this->duration)
    {
        this->stop();
    }
}

std::chrono::seconds Game::getRemainingTime() const
{
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - this->startTime);
    const auto remaining = duration - elapsed;
    return std::max(remaining, std::chrono::seconds(0));
}

static void stringToUpper(std::string &str)
{
    for (char &c : str)
    {
        c = std::toupper(static_cast<unsigned char>(c));
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

void Game::stop()
{
    this->inProgress = false;
}

Game::Game(const unsigned short int maxErrors, const unsigned int duration)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, static_cast<int>(Game::availableWords.size()) - 1);

    this->word = Game::availableWords[dis(gen)];

    auto wordWithHiddenCharacters = std::string(this->word);
    for (char &letter : wordWithHiddenCharacters)
    {
        if (letter != ' ')
        {
            letter = '_';
        }
    }
    this->wordWithHiddenChars = wordWithHiddenCharacters;

    this->wordLength = this->word.length();
    this->maxErrors = maxErrors;
    this->duration = std::chrono::seconds(duration);
    this->inProgress = true;
}

std::vector<std::string> Game::availableWords = Game::loadWords(WORDS_FILE);
