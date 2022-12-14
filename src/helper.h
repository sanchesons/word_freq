#include "word_counter.h"
#include <iostream>

namespace {
    bool is_letter(wchar_t ch)
    {
        return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z';
    }
}

namespace wf
{

WordCounter count_words(std::wistream& in)
{
    WordCounter word_counter;
    for (auto ch = wchar_t(); !in.eof();) {
        std::string word;
        for (in >> ch; !in.eof() && is_letter(ch); in >> ch) {
            word.push_back(std::tolower(ch));
        }
        if (!word.empty()) {
            word_counter.push(std::move(word));
        }
    }
    return word_counter;
}

void print(const wf::GroupByFreqView& word_freq)
{
    if(word_freq.empty()) {
        return;
    }
    auto number_width = std::to_string(word_freq.begin()->first).size();
    for (auto& [freq, words] : word_freq) {
        for (auto& word : words) {
            std::cout << std::setw(number_width) << freq << " " << word << std::endl;
        }
    }
}

}