#pragma once

#include "types.h"

#include <iomanip>
#include <mem.h>
#include <algorithm>

namespace d2::parsing {

    template <class T>
    static T read_offset(byte *headerData, u32 offset) {
        T value;
        memcpy(&value, headerData + offset, sizeof(value));
        return value;
    }

    static std::string get_flipped_string(byte *data, u32 length, u32 offset) {
        char buffer[length];
        memcpy(&buffer, data + offset, length);
        std::reverse(buffer, buffer + sizeof(buffer) / sizeof(buffer[0]));
        std::string s(buffer);
        return s;
    }

    template <class T>
    static T read(byte *headerData) {
        return read_offset<T>(headerData, 0);
    }

    template <typename T>
    std::string format_hex(T value, u8 padding, bool prefix = false) {
        std::stringstream builder;
        std::stringstream() << (prefix ? "0x" : "") << std::setfill('0') << std::setw(padding) << std::hex << value;
        return builder.str();
    }

    template<typename ... Args>
    static std::string format(const std::string& format, Args ... args) {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1;
        if(size_s <= 0)
            throw std::runtime_error("Error during formatting.");
        auto size = static_cast<size_t>(size_s);
        auto buf = std::make_unique<char[]>(size);
        std::snprintf(buf.get(), size, format.c_str(), args ...);
        return (buf.get(), buf.get() + size - 1);
    }
}
