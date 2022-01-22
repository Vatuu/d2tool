#pragma once

#include "types.h"

#include <iomanip>
#include <mem.h>
#include <algorithm>

namespace d2::parsing {

    template <class T>
    static T read_offset(const byte *headerData, u32 offset) {
        T value;
        memcpy(&value, headerData + offset, sizeof(value));
        return value;
    }

    static std::string get_flipped_string(const byte *data, u32 length, u32 offset) {
        char buffer[length];
        memcpy(&buffer, data + offset, length);
        std::reverse(buffer, buffer + sizeof(buffer) / sizeof(buffer[0]));
        std::string s(buffer);
        return s;
    }

    static std::vector<byte> get_bytes(const byte *data, u32 length, u32 offset) {
        std::vector<byte> vec(length);
        memcpy(vec.data(), data + offset, length);
        return vec;
    }

    template <class T>
    static T read(const byte *headerData) {
        return read_offset<T>(headerData, 0);
    }

    template<typename... Args>
    static std::string format(const std::string& format, Args... args) {
        const auto size = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
        const auto buffer = std::make_unique<char[]>(size);

        std::snprintf(buffer.get(), size, format.c_str(), args...);

        return std::string(buffer.get(), buffer.get() + size - 1);
    }
}
