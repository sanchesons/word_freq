#pragma once

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


class IcuUtfInputStream
{
public:
    using UConverterPtr = std::unique_ptr<UConverter, void(*)(UConverter*)>;
    using InputBuffer = std::array<char, 32*1024>;
    using OutputBuffer = std::array<char16_t, 32*1024>;

protected:
    int fd;
    UConverterPtr converter_ptr;
    InputBuffer input_buffer;
    OutputBuffer output_buffer;

    const char* input_cur = input_buffer.begin();
    const char* input_end = input_buffer.begin();

    char16_t* output_cur = output_buffer.begin();
    char16_t* output_end = output_buffer.begin();

    bool is_eof = false;

public:
    explicit IcuUtfInputStream(const std::string& filepath) : 
        fd(open_file(filepath)),
        converter_ptr(open_convertor())
    {
        posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
    }

    ~IcuUtfInputStream()
    {
        close(fd);
    }

    char16_t get()
    {
        if (auto ch = get_output(); ch != 0) {
            return ch;
        }

        std::tie(input_cur, input_end) = read_file();
        if (eof()) {
            return 0;
        }

        if (auto [src, dst, error] = convert(input_cur, input_end, output_buffer.begin(), output_buffer.end()); error == U_ZERO_ERROR) {
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
    char16_t get_output()
    {
        if (output_cur < output_end) {
            return *output_cur++;
        }
        return 0;
    }

    static int open_file(const std::string& filepath)
    {
        auto fd = open(filepath.c_str(), O_RDONLY);
        if (fd == -1) {
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
            throw std::runtime_error("couldn't read file");
        }
        return {input_buffer.begin(), input_buffer.begin() + n + rest};
    }

    UConverterPtr open_convertor()
    {
        UErrorCode errorCode = U_ZERO_ERROR;
        auto* converter = ucnv_open("utf-8", &errorCode);
        auto converter_ptr = UConverterPtr(converter, ucnv_close);
        if(U_FAILURE(errorCode)) {
            throw std::runtime_error("Couldn't init convert");
        }
        return converter_ptr;
    }

    std::tuple<const char*, char16_t*, UErrorCode> convert(const char* src, const char* src_end, char16_t* dst, char16_t* dst_end)
    {
        UErrorCode errorCode = U_ZERO_ERROR;
        ucnv_toUnicode(converter_ptr.get(), &dst, dst_end, &src, src_end, 0, is_eof, &errorCode);
        if(U_FAILURE(errorCode)) {
            throw std::runtime_error("Couldn't convert");
        }
        return {src, dst, errorCode};
    }
};