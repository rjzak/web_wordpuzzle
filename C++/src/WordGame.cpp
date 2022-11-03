#include "WordGame.h"
#include "assets.h"
#include <cstdlib>
#include <iostream>
using namespace std;

WordGame::WordGame() {
    srand((unsigned) time(NULL));
    unsigned int random_index = rand() % (sizeof(wordlist)/6u);
    while(wordlist[random_index] != 0x0A) {
        random_index++;
    }
    random_index++;
    the_word = string(wordlist+random_index, wordlist+random_index+WORD_SIZE);
    cout << "The word: " << the_word << endl;
}

string testGuess(string guess) {
    return string("");
}