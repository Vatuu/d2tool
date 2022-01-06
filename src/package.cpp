#include "package.h"
#include "parse_helper.h"
#include "file_helper.h"

#include <assert.h>
#include <fstream>
#include <iostream>

namespace d2 {

    namespace p = parsing;

    Package::Package(const std::string& dir, const std::string& packageName, bool isLatest) {
        if(isLatest) {
            std::vector<std::string> pkgs = d2::files::file_get_patches(packageName, dir);
            path = pkgs[0];
            if(pkgs.size() > 1) {
                std::for_each(pkgs.begin() + 1, pkgs.end(), [&](const std::string& entry) {
                    u8 patchId = entry.at(entry.size() - 5);
                    patches.insert(std::pair<u8, Package>(patchId, Package(entry, "")));
                });
            }
        } else
            path = dir;


        std::ifstream input(path, std::ios::in);
        byte headerBuffer[HEADER_SIZE];
        input.read((char*)&headerBuffer, HEADER_SIZE);

        header = parseHeader(headerBuffer);
        name = parsing::format_hex(header.pkgId, 4);
        blockTable = create_block_table(input);
        if(isLatest) {
            entryTable = create_entry_table(input);
            set_nonce();
        }
        input.close();
    }

    std::string get_file_typename(u8 fileType) {
        switch(fileType) {
            case 8:
                return "8080xxxx Structure File";
            case 33:
                return "DirectX Bytecode Header";
            case 41:
                return "DirectX Bytecode Data";
            default:
                return "Unknown";
        }
    }

    u16 get_pkg_id(u32 referenceInfo) {
        u16 pkgId = (referenceInfo >> 13) & 0x3FF;
        u16 refDigits = (referenceInfo >> 23) & 0x3;
        return refDigits == 1 ? pkgId : pkgId | 0x100 << refDigits;
    }

    Entry decodeEntry(u32 referenceInfo, u32 fileInfo, u32 blockInfo, u32 metadata) {
        Entry e;
        e.refId = referenceInfo & 0x1FFF;
        e.refPackageId = get_pkg_id(referenceInfo);
        e.refUnknownId = (referenceInfo >> 23) & 0x1FF;
        e.subType = (fileInfo >> 6) & 0x7;
        e.type = (fileInfo >> 9) & 0x7F;
        e.startingBlock = blockInfo & 0x3FFF;
        e.startBlockOffset = ((blockInfo >> 14) & 0x3FFF) << 4;
        e.fileSize = (metadata & 0x3FFFFFF) << 4 | (blockInfo >> 28) & 0xF;
        e.unknown = (metadata >> 26) & 0x3F;
        e.fileType = get_file_typename(e.type);
        return e;
    }

    PackageHeader Package::parseHeader(byte *data) {
        return PackageHeader{
                .pkgId = p::read_offset<u16>(data, 0x10),
                .patchId = p::read_offset<u16>(data, 0x30),
                .entryTableOffset = p::read_offset<u32>(data, 0x44),
                .entryTableSize = p::read_offset<u32>(data, 0x60),
                .blockTableSize = p::read_offset<u32>(data, 0x68),
                .blockTableOffset = p::read_offset<u32>(data, 0x6C)
        };
    }

    std::vector<Entry> Package::create_entry_table(std::ifstream& data) {
        u32 tableSize = header.entryTableSize * Entry::ENTRY_SIZE;
        byte entryTableData[tableSize];
        data.seekg(header.entryTableOffset, std::ios::beg);
        data.read((char*)&entryTableData, tableSize);

        std::vector<Entry> entries;
        for(uint i = 0; i < tableSize; i += Entry::ENTRY_SIZE) {
            Entry entry = decodeEntry(
                    p::read_offset<u32>(entryTableData, i),
                    p::read_offset<u32>(entryTableData, i + 4),
                    p::read_offset<u32>(entryTableData, i + 8),
                    p::read_offset<u32>(entryTableData, i + 12));
            entry.fileName = name + '-' + parsing::format_hex(entries.size(), 2);
            entries.push_back(entry);
        }
        return entries;
    }

    std::vector<Block> Package::create_block_table(std::ifstream& data) {
        u32 tableSize = header.blockTableSize * Block::BLOCK_ENTRY_SIZE;
        byte blockTableData[tableSize];
        data.seekg(header.blockTableOffset, std::ios::beg);
        data.read((char*)&blockTableData, tableSize);

        std::vector<Block> blocks;
        for(int i = 0; i < tableSize; i += Block::BLOCK_ENTRY_SIZE) {
            Block block;
            block.offset = p::read_offset<u32>(blockTableData, i);
            block.size = p::read_offset<u32>(blockTableData, i + 4);
            block.patchId = p::read_offset<u16>(blockTableData, i + 8);
            block.flags = p::read_offset<u16>(blockTableData, i + 10);
            block.hash = p::get_flipped_string(blockTableData, 20, 12);
            block.gcmTag = p::get_flipped_string(blockTableData, 16, 32);
            block.id = blocks.size();
            blocks.push_back(block);
        }
        return blocks;
    }

    void Package::set_nonce() {
        std::copy(std::begin(NONCE_SEED), std::end(NONCE_SEED), std::begin(nonce));
        nonce[0] ^= (header.pkgId >> 8) & 0xFF;
        nonce[1] = 0xEA;
        nonce[11] ^= header.pkgId & 0xFF;
    }
}