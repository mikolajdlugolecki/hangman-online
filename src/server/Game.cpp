#include "Game.h"

#include <fstream>
#include <random>

#include "Constants.h"

Game* Game::create(unsigned short int max_errors) {
    return new Game(max_errors);
}

std::string Game::getGameStartedPayload() {
    return std::to_string(this->word_length) + "|" + std::to_string(this->max_errors);
}

std::vector<std::string> Game::loadWords(const std::string& file_name) {
    std::fstream file;
    file.open(file_name, std::ios::in);
    if (!file.good()) {
        perror("Error opening file with words");
        exit(EXIT_FAILURE);
    }
    std::vector<std::string> result;
    std::string line;
    while (std::getline(file, line)) {
        result.push_back(line);
    }
    file.close();
    return result;
}

Game::Game(unsigned short int max_errors) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, Game::available_words.size() - 1);

    this->word = Game::available_words[dis(gen)];
    this->word_length = this->word.length();
    this->max_errors = max_errors;
}

std::vector<std::string> Game::available_words = Game::loadWords(WORDS_FILE);
