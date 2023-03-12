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

class Utf8InputStream
{
public:
    using FilePtr = std::unique_ptr<FILE, int(*)(FILE*)>;
    using UConverterPtr = std::unique_ptr<UConverter, void(*)(UConverter*)>;

protected:
    FilePtr file_ptr;
    UConverterPtr converter_ptr;
    // std::array<char, 2048> input_buffer;
    // std::array<char16_t, 4096> uBuffer;
    std::array<char, 8192> input_buffer;
    std::array<char16_t, 8192> uBuffer;
    char16_t* us = uBuffer.begin();
    int32_t uBuffIdx = 0;
    bool is_eof = false;

public:
    explicit Utf8InputStream(const std::string& filepath) : 
        file_ptr(open_file(filepath)),
        converter_ptr(open_convertor())
    {
    }

    char16_t get()
    {
        if (auto ch = next(); ch != 0) {
            return ch;
        }

        const char *source, *source_limit;
        while (!is_eof) {
            auto n = fread(input_buffer.begin(), sizeof(char), input_buffer.size(), file_ptr.get());
            if (feof(file_ptr.get())) {
                is_eof = true;
            }
            source = input_buffer.begin();
            source_limit = input_buffer.begin() + n;
            UErrorCode errorCode = U_ZERO_ERROR;
            ucnv_toUnicode(converter_ptr.get(), &us, uBuffer.end(), &source, source_limit, 0, is_eof, &errorCode);
            if(U_FAILURE(errorCode)) {
                throw std::runtime_error("Couldn't convert");
            }
            if (us != uBuffer.begin()) { // Was the character consumed?
                return next();
            }
        }
        return 0;
    }

    bool eof() const
    {
        return is_eof && us == uBuffer.begin();
    }

private:
    char16_t next()
    {
        auto size = static_cast<int32_t>(us-uBuffer.begin());
        if (uBuffIdx < size) {
            char16_t ch;
            // U16_NEXT(uBuffer.begin(), uBuffIdx, size, ch);
            U16_NEXT_UNSAFE(uBuffer.begin(), uBuffIdx, ch);
            return ch;
        }
        us = uBuffer.begin();
        uBuffIdx = 0;
        return 0;
    }

    FilePtr open_file(const std::string& filepath)
    {
        auto* file = fopen(filepath.c_str(), "r");
        if (file == nullptr){
            throw std::runtime_error("Couldn't open file");
        }
        return FilePtr(file, fclose);
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
};