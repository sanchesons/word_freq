#include "helper.h"
#include <exception>
#include <iostream>
#include <vector>

void test_empty_words()
{
    auto texts = std::array<std::wstring,3>{{
        L"",
        L"\n\n\0\n\n",
        L"網站 !;%:;%:;%:\"привет мир!@#!網"
    }};

    for (const auto& text : texts) {
        auto stream = std::wistringstream(text);
        stream >> std::noskipws;
        const auto wc = wf::count_words(stream);
        if (!wc.empty()) {
            throw std::logic_error("test_empty_words: not empty result");
        }
    }
}

void test_one_word()
{
    struct Sample
    {
        std::wstring in_text;
        std::string out_word;
    };

    auto texts = std::array<Sample,2>{{
        {L"word", "word"},
        {L"網站word\nпривет мир", "word"}
    }};

    for (const auto& [text, expexted_word] : texts) {
        auto stream = std::wistringstream(text);
        stream >> std::noskipws;
        const auto wc = wf::count_words(stream);
        if (wc.size() != 1) {
            throw std::logic_error("test_one_word: empty result");
        }
    }
}

void test_multi_words()
{
    struct Sample
    {
        using Out = std::vector<std::pair<uint32_t,std::string>>;

        std::wstring in;
        Out out;
    };

    auto texts = std::array<Sample,2>{{
        {L"Why not start with.", {{1,"not"}, {1,"start"}, {1,"why"}, {1,"with"}}},
        {L"網站word\nприветwordмир word網sentence.", {{3,"word"}, {1,"sentence"}}}
    }};

    for (const auto& [text, expexted] : texts) {
        auto stream = std::wistringstream(text);
        stream >> std::noskipws;
        const auto wc = wf::count_words(stream);
        auto group = wc.group_by_freq();

        auto i = size_t(0);
        for (const auto& [freq, words] : group) {
            for (const auto& word : words) {
                if (freq != expexted[i].first || word != expexted[i].second) {
                    throw std::logic_error("test_multi_words: unexpected result: freq="+std::to_string(freq)+", word="+std::string(word));
                }
                ++i;
            }
        }
    }
}

int main(int argc, const char* args[])
{
    try {
        test_empty_words();
        test_one_word();
        test_multi_words();
    } catch (const std::logic_error& e) {
        std::cout << "FILED: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "PASSED" << std::endl;

    return 0;
}