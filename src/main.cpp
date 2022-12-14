#include "helper.h"
#include <iostream>

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

    auto group = wf::count_words(fin).group_by_freq();
    wf::print(group);

    return 0;
}
