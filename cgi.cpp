#include "util.h"

int main (int argc, char** argv) {

    init();

    String input;
    getline(std::cin, input);
    input += '\n';

    recalculate_sets(input);
    std::vector<Word> words = valid_words();

    String output = "";
    for (auto& word : words) {
        output += word.letters + " ";
    }
    *output.end() = '\n';

    std::cout << "Content-type: text/plain\n\n"
              << output << "\n";
    return 0;
}
