//
// Created by Vatuu on 05/01/2022.
//

#pragma once

#include <vector>
#include <map>
#include <string>
#include <windows.h>

#include "package.h"
#include "window.h"

namespace d2 {

    class Extractor {

    private:
        ui::ExtractorWindows* window;
        std::string folder, target;
        std::map<std::string, Package> packages;
        int64_t oodle;
        HMODULE dll;

        bool initOodle();
        void decompress(Block *b, std::vector<byte> source, std::vector<byte> target) const;
        void decrypt(Block *b, const byte* nonce, std::vector<byte> source, std::vector<byte>& buffer);
        void process_entry(Entry& entry, Package pkg, std::map<u8, std::ifstream> streams, std::vector<byte>& buffer);
        void process_block(Block *b, u32 offset, std::ifstream& stream, std::vector<byte>&  buffer, const byte *nonce, bool isStarting);
    public:
        Extractor(const std::vector<std::string>& packages, const std::string& folder, const std::string& target, ui::ExtractorWindows *window);

        void extract();
    };
}
