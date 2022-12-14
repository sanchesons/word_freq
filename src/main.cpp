#include "helper.h"
#include "cxxopts.hpp"
#include <iostream>

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

    auto file_path = args["file"].as<std::string>();

    try {
        std::locale::global(std::locale(""));
    } catch(const std::exception& e) {
        std::cout << "Couldn't set locale: " << e.what() << std::endl;
    }

    auto fin = std::wifstream(file_path);
    fin >> std::noskipws;

    auto group = wf::count_words(fin).group_by_freq();
    wf::print(group);

    return 0;
}
