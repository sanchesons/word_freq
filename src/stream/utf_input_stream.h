#pragma once

#include "utf_convertor.h"

#include "unicode/uchriter.h"
#include "unicode/utypes.h"
#include "unicode/ucnv.h"
#include "unicode/ucnv_err.h"
#include <unicode/unistr.h>

#include <filesystem>
#include <memory>
#include <cstring>
#include <stdexcept>
#include <array>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>

namespace fs = std::filesystem;

class UtfInputStream final
{
public:
    using UConverterPtr = std::unique_ptr<UConverter, void(*)(UConverter*)>;
    using InputBuffer = std::array<char8_t, 32*1024>;
    using OutputBuffer = std::array<char32_t, 32*1024>;

private:
    int fd;
    InputBuffer input_buffer;
    OutputBuffer output_buffer;
    bool is_eof = false;

    const char8_t* input_cur = input_buffer.begin();
    const char8_t* input_end = input_buffer.begin();

    char32_t* output_cur = output_buffer.begin();
    char32_t* output_end = output_buffer.begin();

    UtfConvertor convertor;

public:
    UtfInputStream() noexcept : 
        fd(STDIN_FILENO)
    {
        
    }

    explicit UtfInputStream(const fs::path& filepath) : 
        fd(open_file(filepath))
    {
        posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
    }

    ~UtfInputStream()
    {
        if (fd != STDIN_FILENO) {
            close(fd);
        }
    }

    char32_t get()
    {
        if (auto ch = get_output(); ch != 0) {
            return ch;
        }

        std::tie(input_cur, input_end) = read_file();
        if (eof()) {
            return 0;
        }

        if (auto [src, dst, error] = convertor.convert(input_cur, input_end, output_buffer.begin()); error != 1) {
            input_cur = src;
            output_cur = output_buffer.begin();
            output_end = dst;
        } else {
            throw std::runtime_error("invalid utf-8 input");
        }

        return get_output();
    }

    bool eof() const
    {
        return is_eof;
    }

private:
    static int open_file(const fs::path& filepath)
    {
        auto fd = open(filepath.c_str(), O_RDONLY);
        if (fd == -1) {
            throw std::runtime_error("couldn't open file");
        }
        return fd;
    }

    char32_t get_output()
    {
        if (output_cur < output_end) {
            return *output_cur++;
        }
        return 0;
    }

    std::pair<const char8_t*, const char8_t*> read_file()
    {
        auto rest = input_end - input_cur;
        if (rest > 0) {
            std::memmove(input_buffer.begin(), input_cur, rest);
        }

        auto n = read(fd, input_buffer.begin() + rest, input_buffer.size() - rest);
        if (n == 0) {
            is_eof = true;
            if (rest != 0) {
                throw std::runtime_error("invalid utf-8 input at the end of file, rest=" + std::to_string(rest));
            }
        } else if(n == (size_t)-1) {
            throw std::runtime_error("couldn't read file");
        }
        return {input_buffer.begin(), input_buffer.begin() + n + rest};
    }
};