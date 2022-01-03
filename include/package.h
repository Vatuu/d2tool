#pragma once

#include "types.h"

namespace d2 {
    class Package {

        const u16 HEADER_SIZE = 0x16F;

        const u8 AES_KEY_0[16] = {
                0xD6, 0x2A, 0xB2, 0xC1,
                0x0C, 0xC0,0x1B, 0xC5,
                0x35, 0xDB, 0x7B,0x86,
                0x55, 0xC7, 0xDC, 0x3B };

        const u8 AES_KEY_1[16] = {
                0x3A, 0x4A, 0x5D, 0x36,
                0x73, 0xA6,0x60, 0x58,
                0x7E, 0x63, 0xE6,0x78,
                0xE4, 0x08, 0x92, 0xB5 };

    private:
        std::string name, path;
        PackageHeader header;
        std::vector<Entry> entryTable;
        std::vector<Block> blockTable;

        static PackageHeader parseHeader(byte *data);

        std::vector<Entry> create_entry_table(std::ifstream& data);
        std::vector<Block> create_block_table(std::ifstream& data);
    public:
        explicit Package(const std::string& packageDir);
    };
}

