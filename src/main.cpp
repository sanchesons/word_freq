#include "helper.h"
#include "cxxopts.hpp"

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, const char* argv[])
{
    cxxopts::Options options("wf", "wf is basic words analytics");

    options.add_options()
        ("f,file", "text file path to analyse", cxxopts::value<std::string>())
        ("h,help", "Print usage");

    auto args = options.parse(argc, argv);

    if (args.count("help"))
    {
      std::cout << options.help() << std::endl;
      return 0;
    }
    auto file_path = fs::path(args["file"].as<std::string>());

    auto word_counter = wf::count_words(file_path);
    auto grouped = word_counter.group_by_freq();
    wf::print(grouped);

    return 0;
}
