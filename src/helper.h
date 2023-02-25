#include "word_counter.h"

#include <unicode/unistr.h>
#include <unicode/uchar.h>

#include <iostream>
#include <vector>
#include <codecvt>
#include <filesystem>

namespace fs = std::filesystem;

namespace wf
{
WordCounter count_words(std::wistream& in)
{
    WordCounter word_counter;
    for (auto ch = wchar_t(); !in.eof();) {
        std::string word;
        for (in >> ch; !in.eof() && u_isUAlphabetic(ch); in >> ch) {
            auto lower_ch = u_tolower(ch);
            char buffer[MB_CUR_MAX];
            auto pos = 0;
            U8_APPEND_UNSAFE(buffer, pos, lower_ch);
            word.append(std::string(buffer, pos));
        }
        if (!word.empty()) {
            word_counter.push(std::move(word));
        }
    }
    return word_counter;
}

WordCounter count_words(const fs::path& filepath)
{
    const auto converter = new std::codecvt_utf8<wchar_t>();
    const std::locale utf8_locale = std::locale(std::locale(), converter);

    auto in = std::wifstream(filepath);
    in.imbue(utf8_locale);
    in.unsetf(std::ios_base::skipws);
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