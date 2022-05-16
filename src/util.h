#include "words.h"   // List of all possible wordle answers
#include <regex>     // For input validation when using the command line
#include <algorithm> // Find an item in a vector


// --- Utility functions ---

/* Debugging */
void print_remaining_words();

/* Initialize character sets */
void init();

/* Convert a newline-terminated, space-separated c string to a string vector */
StringArray splitOnSpaces(String data);

/* Return true if a character vector contains a character */
bool contains(CharArray v, char c);

/* Return true if a string vector contains a string */
bool contains(StringArray v, String s);

/* Append unique characters from right vector to left vector */
void merge_left(CharArray& l, CharArray r);


// --- State updating functions ---

/* Update sets of possible letters and required letters from string input */
bool update_sets(String& input);

/* Recalculate sets from a space-separated list of 6 input strings 
 * String must be null-terminated
 */
bool recalculate_sets(String data);

/* Get valid words */
std::vector<Word> valid_words();
