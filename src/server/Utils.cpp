#include "Utils.h"

namespace Utils
{
int countLettersInWord(std::string word, char letter)
{
    int count = 0;
    for (char c : word)
    {
        if (c == letter)
        {
            count++;
        }
    }
    return count;
}
} // namespace Utils
