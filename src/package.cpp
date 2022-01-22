#include "package.h"
#include "parse_helper.h"
#include "file_helper.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <ranges>

namespace d2 {

    namespace p = parsing;

    Package::Package(const std::string& dir, const std::string& packageName, bool isLatest) {
        if(isLatest) {
            std::vector<std::string> pkgs = d2::files::file_get_patches(packageName, dir);
            path = pkgs[0];
            if(pkgs.size() > 1)
                for(auto &entry : pkgs | std::views::drop(1))
                    patches.emplace_back(entry, "");
        } else
            path = dir;

        std::ifstream input(path, std::ios::in | std::ios::binary);

        std::vector<byte> headerBuffer(HEADER_SIZE);
        input.read((char*)headerBuffer.data(), headerBuffer.size());
        header = parseHeader(headerBuffer);
        name = parsing::format("%04X", header.pkgId);

        if(isLatest) {
            std::vector<byte> entryBuffer(header.entryTableSize * Entry::ENTRY_SIZE);
            input.seekg(header.entryTableOffset, std::ios::beg);
            input.read((char*)entryBuffer.data(), entryBuffer.size());
            entryTable = create_entry_table(entryBuffer);
            set_nonce();
        }

        std::vector<byte> blockBuffer(header.blockTableSize * Block::BLOCK_ENTRY_SIZE);
        input.seekg(header.blockTableOffset, std::ios::beg);
        input.read((char*)blockBuffer.data(), blockBuffer.size());
        blockTable = create_block_table(blockBuffer);

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

    Entry decodeEntry(u32 referenceInfo, u32 fileInfo, u64 parsingInfo) {
        Entry e;
        e.refUnknownId = (referenceInfo >> 23) & 0x1FF;
        e.refPackageId = get_pkg_id(referenceInfo);
        e.refId = referenceInfo & 0x1FFF;

        e.type = (fileInfo >> 9) & 0x7F;
        e.subType = (fileInfo >> 6) & 0x7;

        e.unknown = parsingInfo >> 58;
        e.fileSize = (parsingInfo >> 28) & 0x3FFFFFFF;
        e.startBlockOffset = (parsingInfo >> 14) & 0x3FFF;
        e.startingBlock = parsingInfo & 0x3FFF;

        e.fileType = get_file_typename(e.type);
        return e;
    }

    PackageHeader Package::parseHeader(std::vector<byte> headerData) {
        return PackageHeader{
                .pkgId = p::read_offset<u16>(headerData.data(), 0x10),
                .patchId = p::read_offset<u16>(headerData.data(), 0x30),
                .entryTableOffset = p::read_offset<u32>(headerData.data(), 0x44),
                .entryTableSize = p::read_offset<u32>(headerData.data(), 0x60),
                .blockTableSize = p::read_offset<u32>(headerData.data(), 0x68),
                .blockTableOffset = p::read_offset<u32>(headerData.data(), 0x6C)
        };
    }

    std::vector<Entry> Package::create_entry_table(std::vector<byte> tableData) const {
        std::vector<Entry> entries;
        for(size_t i = 0; i < tableData.size(); i += Entry::ENTRY_SIZE) {
            Entry entry = decodeEntry(
                    p::read_offset<u32>(tableData.data(), i),
                    p::read_offset<u32>(tableData.data(), i + 4),
                    p::read_offset<u64>(tableData.data(), i + 8));
            entry.id = i / Entry::ENTRY_SIZE;
            entry.fileName = name + '-' + parsing::format("%04X", entry.id);
            entries.push_back(entry);
        }
        return entries;
    }

    std::vector<Block> Package::create_block_table(std::vector<byte> tableData) const {
        std::vector<Block> blocks;
        for(size_t i = 0; i < tableData.size(); i += Block::BLOCK_ENTRY_SIZE) {
            Block block;
            block.id = i / Block::BLOCK_ENTRY_SIZE;
            block.offset = p::read_offset<u32>(tableData.data(), i);
            block.size = p::read_offset<u32>(tableData.data(), i + 4);
            block.patchId = p::read_offset<u16>(tableData.data(), i + 8);
            block.flags = p::read_offset<u16>(tableData.data(), i + 10);
            block.hash = p::get_flipped_string(tableData.data(), 20, i + 12);
            block.gcmTag = p::get_bytes(tableData.data(), 16, i + 32);
            blocks.push_back(block);
        }
        return blocks;
    }

    void Package::set_nonce() {
        std::copy(std::begin(NONCE_SEED), std::end(NONCE_SEED), std::begin(nonce));
        nonce[0] ^= (header.pkgId >> 8) & 0xFF;
        nonce[11] ^= header.pkgId & 0xFF;
    }
}