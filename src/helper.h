#include "word_counter.h"
#include "stream/utf8_input_stream.h"

#include <unicode/unistr.h>
#include <unicode/uchar.h>

#include <iostream>
#include <vector>
#include <codecvt>
#include <filesystem>

namespace fs = std::filesystem;

namespace wf
{

void test(const std::string& filepath)
{
    auto in = Utf8InputStream(filepath);
    while (!in.eof()) {
        auto ch = in.get();
    }
}

void test(std::wistream& in)
{
    for (auto ch = wchar_t(); !in.eof();) {
        in >> ch;
    }
}

WordCounter count_words(const std::string& filepath)
{
    // TODO limit word size
    WordCounter word_counter;
    auto in = Utf8InputStream(filepath);
    std::string word;
    while (!in.eof()) {
        auto ch = in.get();
        if (u_isUAlphabetic(ch)) {
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

WordCounter count_words(std::wistream& in)
{
    WordCounter word_counter;
    for (auto ch = wchar_t(); !in.eof();) {
        std::string word;
        while (in >> ch && u_isUAlphabetic(ch)) {
            auto lower_ch = u_tolower(ch);
            char buffer[MB_CUR_MAX];
            auto pos = 0;
            U8_APPEND_UNSAFE(buffer, pos, lower_ch);
            word.append(buffer, pos);
        }
        if (!word.empty()) {
            word_counter.push(std::move(word));
        }
    }
    return word_counter;
}

WordCounter count_words()
{
    const auto converter = new std::codecvt_utf8<wchar_t>();
    const std::locale utf8_locale = std::locale(std::locale(), converter);
    std::locale::global(utf8_locale);

    std::ios_base::sync_with_stdio(false);
    std::wcin.imbue(utf8_locale);
    std::wcin.unsetf(std::ios_base::skipws);
    return count_words(std::wcin);
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