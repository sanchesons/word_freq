#include "word_counter.h"
#include <iostream>

void print_word_freq(const wf::WordCounter::GroupByFreq& word_freq)
{
    if(word_freq.empty()) {
        return;
    }
    const auto number_width = std::to_string(word_freq.begin()->first).size();
    for (const auto& [freq, words] : word_freq) {
        for (const auto& word : words) {
            std::cout << std::setw(number_width) << freq << " " << word << std::endl;
        }
    }
}

int main(int argc, const char* args[])
{
    if (argc < 2) {
        std::cout << "Bad args, example: " << args[0] << " <path>" << std::endl;
        return 1;
    }

    try {
        std::locale::global(std::locale(""));
    } catch(const std::exception& e) {
        std::cout << "Couldn't set locale: " << e.what() << std::endl;
    }

    auto fin = std::wifstream(args[1]);
    fin >> std::noskipws;

    print_word_freq(wf::count_words(fin).group_by_freq());

    return 0;
}