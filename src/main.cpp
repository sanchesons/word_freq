#include "helper.h"
#include "cxxopts.hpp"

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

wf::WordCounter build_word_counter(const cxxopts::ParseResult& args)
{
    if (args.count("file") != 0) {
        auto file_path = fs::path(args["file"].as<std::string>());

        return wf::count_words(file_path);
    }
    return wf::count_words();
}

int main(int argc, const char* argv[])
{
    cxxopts::Options options("wf", "wf is basic words analytics");

    options.add_options()
        ("f,file", "text file path to analyse, if not specified stdin will be used", cxxopts::value<std::string>())
        ("h,help", "print usage");

    auto args = options.parse(argc, argv);
    if (args.count("help"))
    {
      std::cout << options.help() << std::endl;
      return 0;
    }

    auto word_counter = build_word_counter(args);
    auto grouped = word_counter.group_by_freq();
    wf::print(grouped);
    return 0;
}
