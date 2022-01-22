#pragma once

#include <map>

#include "types.h"

namespace d2 {
    class Package {

        static const u16 HEADER_SIZE = 0x16F;

    private:

        static PackageHeader parseHeader(std::vector<byte> data);
        std::vector<Entry> create_entry_table(std::vector<byte> data) const;
        std::vector<Block> create_block_table(std::vector<byte> data) const;
        void set_nonce();

    public:
        std::vector<Entry> entryTable;
        std::vector<Block> blockTable;
        std::vector<Package> patches;
        std::string path;
        byte nonce[12];

        Package(const std::string& dir, const std::string& name, bool isLatest = false);

        std::string name;
        PackageHeader header;
    };
}

