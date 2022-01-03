#pragma once

#include "types.h"

#include <iomanip>
#include <mem.h>
#include <algorithm>

namespace d2::parsing {
    template <class T>
    T read_offset(byte *headerData, u32 offset) {
        T value;
        memcpy(&value, headerData + offset, sizeof(value));
        return value;
    }

    std::string get_flipped_string(byte *data, u32 length, u32 offset) {
        char buffer[length];
        memcpy(&buffer, data + offset, length);
        std::reverse(buffer, buffer + sizeof(buffer) / sizeof(buffer[0]));
        std::string s(buffer);
        return s;
    }

    template <class T>
    T read(byte *headerData) {
        return read_offset<T>(headerData, 0);
    }

    template <typename T>
    std::string format_hex(T value, u8 padding, bool prefix = false) {
        std::stringstream builder;
        std::stringstream() << (prefix ? "0x" : "") << std::setfill('0') << std::setw(padding) << std::hex << value;
        return builder.str();
    }
}
