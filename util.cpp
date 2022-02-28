#include "words.h"

#include <algorithm>

void init() {
    // Initialize values to allow any word
    for (char letter : alphabet)
        for (CharSet& position : possible) position.insert(letter);
}

std::string get_input() {
    String input;
    std::cout << "> ";
    std::cin >> input;
    return input;
}

bool contains(CharArray v, char c) {
    return std::count(v.begin(), v.end(), c);
}

bool contains(StringArray v, String s) {
    return std::count(v.begin(), v.end(), s);
}

void merge_left(CharArray& l, CharArray r) {
    
    for (auto it = r.begin(); it != r.end(); it++) {
        if (contains(l, *it)) {
            r.erase(it);
            it--;
        }
    }

    l.insert(l.end(), r.begin(), r.end());
}
