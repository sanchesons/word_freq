#pragma once

#include "utf_convertor.h"

#include "unicode/uchriter.h"
#include "unicode/utypes.h"
#include "unicode/ucnv.h"
#include "unicode/ucnv_err.h"
#include <unicode/unistr.h>

#include <istream>
#include <streambuf>
#include <memory>
#include <cstring>
#include <stdexcept>
#include <array>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

class UtfInputStreamSimple final
{
public:
    using UConverterPtr = std::unique_ptr<UConverter, void(*)(UConverter*)>;
    using InputBuffer = std::array<char, 32*1024>;

private:
    int fd;
    InputBuffer input_buffer;
    bool is_eof = false;

    const char* input_cur = input_buffer.begin();
    const char* input_end = input_buffer.begin();

    UtfConvertor convertor;

public:
    explicit UtfInputStreamSimple(const std::string& filepath) : 
        fd(open_file(filepath))
    {
        posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
    }

    ~UtfInputStreamSimple()
    {
        close(fd);
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

        return get_output();
    }

    bool eof() const
    {
        return is_eof;
    }

private:
    char32_t get_output()
    {
        if (input_cur < input_end) {
            char32_t ch;
            size_t index = input_cur - input_buffer.begin();
            size_t size = input_end - input_buffer.begin();
            U8_NEXT(input_buffer.begin(), index, size, ch);
            if (ch < 0) {
                throw std::runtime_error("couldn't convert");
            }
            input_cur = input_buffer.begin() + index;
            return ch;
        }
        return 0;
    }

    static int open_file(const std::string& filepath)
    {
        auto fd = open(filepath.c_str(), O_RDONLY);
        if (fd == -1){
            throw std::runtime_error("couldn't open file");
        }
        return fd;
    }

    std::pair<const char*, const char*> read_file()
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
            throw std::runtime_error("couldn't read file, " + std::string((const char*)strerror(errno)));
        }
        return {input_buffer.begin(), input_buffer.begin() + n + rest};
    }
};