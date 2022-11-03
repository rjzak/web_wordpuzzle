#ifndef _WORDGAME_H__
#define _WORDGAME_H__

#include <string>
using namespace std;

const unsigned int WORD_SIZE = 5;

class WordGame {
public:
    WordGame();
    int wordSize();
    string testGuess(string guess);

private:
    string the_word;
};

#endif