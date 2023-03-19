#include "helper.h"

#include "cxxopts.hpp"

#include <exception>
#include <iostream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

void test_empty_words(const fs::path& base_dir)
{
    auto files = std::array<fs::path,2>{{
        base_dir / fs::path("no-words/empty-file.txt"),
        base_dir / fs::path("no-words/non-alphabetic.txt")
    }};

    for (const auto& file : files) {
        const auto wc = wf::count_words(file);
        if (!wc.empty()) {
            throw std::logic_error("test_empty_words: not empty result");
        }
    }
    std::cout << "PASSED" << std::endl;
}

void test_one_word(const fs::path& base_dir)
{
    struct Sample
    {
        fs::path filepath;
        std::string out_word;
    };

    auto test_cases = std::array<Sample,2>{{
        {base_dir / fs::path("one-word/en-word.txt"), "word"},
        {base_dir / fs::path("one-word/china-word.txt"), "網站"}
    }};

    for (const auto& [filepath, expexted_word] : test_cases) {
        const auto wc = wf::count_words(filepath);
        if (wc.size() != 1) {
            throw std::logic_error("test_one_word: empty result");
        }
    }
    std::cout << "PASSED" << std::endl;
}

void test_multi_words(const fs::path& base_dir)
{
    struct Sample
    {
        using Out = std::vector<std::pair<uint32_t,std::string>>;

        fs::path filepath;
        Out out;
    };

    auto test_cases = std::array<Sample,2>{{
        { base_dir / fs::path("multi-words/only-ascii.txt"), {{1,"not"}, {1,"start"}, {1,"why"}, {1,"with"}}},
        { base_dir / fs::path("multi-words/china-ru-en.txt"), {{3,"word"}, {1,"sentence"}, {1, "привет"}, {1, "網站"} }}
    }};

    for (const auto& [filepath, expexted] : test_cases) {
        const auto wc = wf::count_words(filepath);
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
    std::cout << "PASSED" << std::endl;
}


int main(int argc, const char* argv[])
{
    cxxopts::Options options("test", "unit tests for wf application");

    options.add_options()
        ("d,dir", "base dir of test's resources", cxxopts::value<std::string>())
        ("h,help", "print usage");

    auto args = options.parse(argc, argv);
    if (args.count("help")) {
      std::cout << options.help() << std::endl;
      return 0;
    }

    auto base_dir = fs::path(args["dir"].as<std::string>());

    try {
        test_empty_words(base_dir);
        test_one_word(base_dir);
        test_multi_words(base_dir);
    } catch (const std::logic_error& e) {
        std::cout << "FILED: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
