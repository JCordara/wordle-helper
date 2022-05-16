#include "util.h"

/* Read input from the command line */
std::string get_input() {
    String input;
    std::cout << "> ";
    getline(std::cin, input);
    return input + '\n';
}

/* Print all remaining words to the console */
void print_words() {

    // List of valid words
    std::vector<Word> words = valid_words();

    // Print out all the valid words
    for (int i = 0; i < words.size(); i++) {
        if ((i % 16) == 0) std::cout << "\n";
        std::cout << words[i].letters;
        std::cout << " ";
    }

    if (words.empty()) std::cout << "\nNo words! Did you make a typo?";
    else std::cout << "\n" << words.size() << 
        (words.size() != 1 ? " words left" : " word left!");

    std::cout << "\n\n";
}

/* Main terminal interface loop */
int main() {

    init();

    bool quit = false;

    while(!quit) {

        String input = get_input();
        if (input == "quit\n") {
            quit = true;
            continue;
        }

        if (!update_sets(input)) {
            if (!recalculate_sets(input)) {
                std::cout << "Invalid input, try again\n\n";
                continue;
            }
        }

        print_words();

    }

    return 0;
}