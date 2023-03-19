#include "word_counter.h"
#include "stream/utf_input_stream.h"

#include <unicode/unistr.h>
#include <unicode/uchar.h>

#include <iostream>
#include <vector>
#include <codecvt>
#include <filesystem>

namespace fs = std::filesystem;

namespace wf
{

static const int MAX_WORD_SIZE = 1024;

WordCounter count_words(UtfInputStream& in)
{
    WordCounter word_counter;
    std::string word;
    while (!in.eof()) {
        auto ch = in.get();
        if (u_isUAlphabetic(ch) && word.size() < MAX_WORD_SIZE) {
            auto lower_ch = u_tolower(ch);
            char buffer[MB_CUR_MAX];
            auto pos = 0;
            U8_APPEND_UNSAFE(buffer, pos, lower_ch);
            word.append(buffer, pos);
        } else if (!word.empty()) {
            word_counter.push(word);
            word.clear();
        }
    }
    return word_counter;
}

WordCounter count_words(const fs::path& filepath)
{
    auto in = UtfInputStream(filepath.string());
    return count_words(in);
}

WordCounter count_words()
{
    auto in = UtfInputStream();
    return count_words(in);
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