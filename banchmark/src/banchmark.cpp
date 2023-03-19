#include "../helper.h"
#include "../stream/utf_input_stream.h"
#include "../stream/utf_input_stream_simple.h"
#include "../stream/icu_utf_input_stream.h"

#include "cxxopts.hpp"

#include <iostream>
#include <filesystem>

void banchmark_iostream(const cxxopts::ParseResult& args)
{
    auto file_path = fs::path(args["file"].as<std::string>());

    const auto converter = new std::codecvt_utf8<wchar_t>();
    const std::locale utf8_locale = std::locale(std::locale(), converter);

    auto in = std::wifstream(file_path);
    in.imbue(utf8_locale);
    in.unsetf(std::ios_base::skipws);

    auto begin = std::chrono::system_clock::now();

    for (auto ch = wchar_t(); !in.eof();) {
        in >> ch;
    }

    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "banchmark iosteam, duration = " << elapsed.count() << "ms" << std::endl;
}


void banchmark_custom(const cxxopts::ParseResult& args)
{
    auto file_path = args["file"].as<std::string>();
    auto in = UtfInputStream(file_path);

    auto begin = std::chrono::system_clock::now();

    auto ch = wchar_t();
    while (!in.eof()) {
        ch = in.get();
    }

    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "banchmark custom, duration = " << elapsed.count() << "ms" << std::endl;
}

void banchmark_custom_simple(const cxxopts::ParseResult& args)
{
    auto file_path = args["file"].as<std::string>();
    auto in = UtfInputStreamSimple(file_path);

    auto begin = std::chrono::system_clock::now();

    auto ch = wchar_t();
    while (!in.eof()) {
        ch = in.get();
    }

    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "banchmark custom simple, duration = " << elapsed.count() << "ms" << std::endl;
}

void banchmark_custom_icu(const cxxopts::ParseResult& args)
{
    auto file_path = args["file"].as<std::string>();
    auto in = IcuUtfInputStream(file_path);

    auto begin = std::chrono::system_clock::now();

    auto ch = wchar_t();
    while (!in.eof()) {
        ch = in.get();
    }

    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "banchmark custom icu, duration = " << elapsed.count() << "ms" << std::endl;
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

    banchmark_custom(args);
    banchmark_iostream(args);
    banchmark_custom_simple(args);
    banchmark_custom_icu(args);

    return 0;
}