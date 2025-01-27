#include "dictionary.h"

#include "exceptions.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>

using namespace std;

string lower(string str) {
    transform(str.cbegin(), str.cend(), str.begin(), ::tolower);
    return str;
}

Dictionary Dictionary::read(const std::string& file_path) {
    ifstream file(file_path);
    if (!file) {
        throw FileException("cannot open dictionary file!");
    }

    std::string word;
    Dictionary dictionary;

    // TODO: complete this. Words in dictionary should all be lowercase.
    while ((file >> word)) {
        dictionary.words.insert(word);
    }

    return dictionary;
}

bool Dictionary::is_word(const std::string& word) const { return this->words.find(word) != this->words.end(); }
