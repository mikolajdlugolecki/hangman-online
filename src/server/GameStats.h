#ifndef GAMESTATS_H
#define GAMESTATS_H

#include "Utils.h"

#include <vector>
#include <algorithm>

class Room;

class GameStats
{
public:
   GameStats(Room *room);
   ~GameStats();

    Room *room;

   int errors = 0;
   int score = 0;
   int numberOfGuessedLettersInWord = 0;
   std::vector<char> guessedLetters;

   void letterGuessed(std::string word, char c);
   void recalculateScore();
};

#endif
