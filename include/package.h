#pragma once

#include <map>

#include "types.h"

namespace d2 {
    class Package {

        const u16 HEADER_SIZE = 0x16F;

    private:
        PackageHeader header;

        static PackageHeader parseHeader(byte *data);
        std::vector<Entry> create_entry_table(std::ifstream& data);
        std::vector<Block> create_block_table(std::ifstream& data);
        void set_nonce();

    public:
        std::vector<Entry> entryTable;
        std::vector<Block> blockTable;
        std::map<u8, Package> patches;
        std::string path;
        byte nonce[12];

        Package(const std::string& dir, const std::string& name, bool isLatest = false);

        std::string name;
    };
}

