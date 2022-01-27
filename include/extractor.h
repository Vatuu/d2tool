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
#include "callbacks.h"

namespace d2::extract {

    class Extractor {

    private:
        ExtractCallbacks *callbacks;
        std::string folder, target;
        std::map<std::string, Package> packages;
        int64_t oodle;
        HMODULE dll;
        bool error;

        bool initOodle();
        bool decompress(Block *b, std::vector<byte> &data) const;
        bool decrypt(Block *b, const byte* nonce, std::vector<byte> &buffer);
        bool process_entry(Entry& entry, Package pkg, std::map<u8, std::ifstream> &streams, std::vector<byte>& buffer);
        bool process_block(Block *b, u32 offset, std::ifstream& stream, std::vector<byte> &buffer, const byte *nonce, bool isStarting);
    public:
        Extractor(const std::vector<std::string> &packages, const std::string &folder, const std::string &target, ExtractCallbacks *window);

        void extract();
    };
}
