#include "util.h"

// Presence of these letters is used for scoring
String common_letters = "eariot";
String uncommon_letters = "ywkv";
String very_uncommon_letters = "xzjq";

CharArray in_word;   // Characters that must be in the word
CharSet possible[5]; // Set of possible characters for each position

// --- Utility functions ---

// Debugging
void print_remaining_words() {
    for (String word : remaining_words) std::cout << word << " ";
}

/* Initialize character sets */
void init() {
    // Initialize values to allow any word
    for (char letter : alphabet)
        for (CharSet& position : possible) position.insert(letter);
    
    remaining_words = all_words;

    in_word.clear();
}

/* Convert a newline-terminated, space-separated c string to a string vector */
StringArray splitOnSpaces(String data) {
    StringArray input_strings;
    int last_space = 0;

    int i = 0;
    while (data[i] != '\n') {

        // On spaces, append a new string to the list
        if (data[i] == ' ') {
            input_strings.push_back(String(&data[last_space], i - last_space));
            last_space = i + 1;
        }

        i++;
    }

    // Append final string
    input_strings.push_back(String(&data[last_space], i - last_space));

    return input_strings;
}

/* Return true if a character vector contains a character */
bool contains(CharArray v, char c) {
    return std::count(v.begin(), v.end(), c);
}

/* Return true if a string vector contains a string */
bool contains(StringArray v, String s) {
    return std::count(v.begin(), v.end(), s);
}

/* Append unique characters from right vector to left vector */
void merge_left(CharArray& l, CharArray r) {
    
    for (auto it = r.begin(); it != r.end(); it++) {
        if (contains(l, *it)) {
            r.erase(it);
            it--;
        }
    }

    l.insert(l.end(), r.begin(), r.end());
}

// --- state updating functions ---

/* Update sets of possible letters and required letters from string input */
bool update_sets(String& input) {
    
    /* Check input validity */
    // Convert to lowercase
    for (int i = 0; i < input.length(); i++)
        if (input[i] > 'A' && input[i] < 'Z') input[i] += 'a' - 'A';
    // Use regex to confirm validity
    std::regex re("([a-z-][!\?]?){5}\n?");
    if (!std::regex_match(input, re)) return false;


    unsigned int position = 0;
    CharArray in_word_tmp;

    for (auto it = input.begin(); it != input.end(); it++) {
        
        char curr = *it;
        char next = '\0';
        if (it != input.end()) next = *(it + 1);
        
        // Ignore dashes (and qualifiers in edge cases)
        if (curr == '-' || curr == '!' || curr == '?') {
            continue; // Do nothing

        // Letter in word and correct position
        } else if (next == '!') {
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

/* Recalculate sets from a space-separated list of 6 input strings 
 * String must be null-terminated
 */
bool recalculate_sets(String data) {
    // Reset restrictions
    init();

    // Get vector of strings from data
    StringArray input_strings = splitOnSpaces(data);

    if (input_strings.size() != 6) return false;

    // Update the sets for each input string
    for (auto& input : input_strings) {
        // If any of the updates fail, reset and return a failure
        if (!update_sets(input)) {
            init();
            return false;
        }
    }

    return true;
}

/* Get valid words */
std::vector<Word> valid_words() {

    // Array of valid Words
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

    return words;
}
