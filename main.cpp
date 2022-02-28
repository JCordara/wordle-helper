#include "words.h"

#include <regex>

// Presence of these letters is used for scoring
String common_letters = "eariot";
String uncommon_letters = "ywkv";
String very_uncommon_letters = "xzjq";

CharArray in_word;  
CharSet possible[5];


// Util functions
void init();
std::string get_input();
bool contains(CharArray, char);
bool contains(StringArray, String);
void merge_left(CharArray&, CharArray);

void print_remaining_words() {
    for (String word : remaining_words) std::cout << word << " ";
}

bool update_sets(String& input) {
    
    /* Check input validity */
    // Convert to lowercase
    for (int i = 0; i < input.length(); i++)
        if (input[i] > 'A' && input[i] < 'Z') input[i] += 'a' - 'A';
    // Use regex to confirm validity
    std::regex re("([a-z][!\?]?){5}");
    if (!std::regex_match(input, re)) return false;


    unsigned int position = 0;
    CharArray in_word_tmp;

    for (auto it = input.begin(); it != input.end(); it++) {
        
        char curr = *it;
        char next = '\0';
        if (it != input.end()) next = *(it + 1);
        
        // Letter in word and correct position
        if (next == '!') {
            in_word_tmp.push_back(curr);     // Letter must be in word
            possible[position].clear();      // No other letters possible
            possible[position].insert(curr); // Besides this letter
            // Advance iterator past qualifier character
            it++; 

        // Letter in word but incorrect position
        } else if (next == '?') { 
            in_word_tmp.push_back(curr);     // Letter must be in word
            possible[position].erase(curr);  // Letter not in this position
            // Advance iterator past qualifier character
            it++;

        // Letter not in correct posiiton and possibly not in word
        } else { 
            // If letter is in word, just mark position as incorrect
            if (contains(in_word, curr) || contains(in_word_tmp, curr))
                possible[position].erase(curr);
            // If not, remove possibility from all positions
            else for (auto& p : possible) p.erase(curr);
        }

        position++;
    }

    merge_left(in_word, in_word_tmp);

    return true;
}

void print_words() {

    std::vector<Word> words;

    // For each of the remaining words
    for (auto index = remaining_words.begin(); 
         index != remaining_words.end(); 
         index++) 
    {
        
        String word = *index;
        int score = 0;
        bool valid = true;

        // Check that each position contains a valid letter
        for (int i = 0; i < 5; i++) {
            if (possible[i].count(word[i]) == 0) {
                remaining_words.erase(index);
                index--;
                valid = false;
                break;
            }
        }
        
        if (!valid) continue;

        // Check that word contains confirmed letters
        String tmp_word = word;
        for (char c : in_word) {
            int pos = tmp_word.find_first_of(c);
            if (pos == String::npos) {
                remaining_words.erase(index);
                index--;
                valid = false;
                break;
            }
            tmp_word.erase(pos, 1);
        }

        if (!valid) continue;

        // Calculate scores of valid words
        for (int i = 0; i < 5; i++) {
            for (int j = (i + 1); j < 5; j++) {
                // Decrement score for every repeated letter
                if (word[i] == word[j]) score -= 4;
            }

            for (char l : uncommon_letters) {
                if (word.find(l) != String::npos)
                    score -= 1;
            }
            
            for (char l : very_uncommon_letters) {
                if (word.find(l) != String::npos)
                    score -= 2;
            }
            
            for (char l : common_letters) {
                if (word.find(l) != String::npos)
                    score += 1;
            }
        }

        if (contains(common_words, word)) score += 2;

        // Add valid words to list
        if(valid) {
            auto it = words.begin();
            for (; it != words.end(); it++)
                if (score >= it->score) break;
            words.emplace(it, word, score);
        }
    }

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

int main() {

    init();

    bool quit = false;

    while(!quit) {

        String input = get_input();
        if (input == "quit") {
            quit = true;
            continue;
        }

        if (!update_sets(input)) {
            std::cout << "Invalid input, try again\n\n";
            continue;
        }

        print_words();

    }

    return 0;
}