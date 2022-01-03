//
// Created by Vatuu on 02/01/2022.
//

#pragma once

#include <cstdint>
#include <string>
#include <vector>

typedef uint8_t byte, u8;
typedef uint16_t ushort, u16;
typedef uint32_t uint, u32;

typedef int16_t s32;

struct PackageHeader {
    u16 pkgId, patchId;
    u32 entryTableOffset, entryTableSize;
    u32 blockTableSize, blockTableOffset;
};

struct Entry {
    u16 id;
    std::string fileName, fileType;
    u16 refId, refPackageId, refUnknownId;
    u8 type, subType;
    u16 startingBlock;
    u32 startBlockOffset, fileSize;
    u8 unknown;

    static const u8 ENTRY_SIZE = 16;
};

struct Block {
    u32 id;
    u32 offset, size;
    u16 patchId, flags;
    std::string hash, gcmTag;

    static const u16 BLOCK_SIZE = 0x4000;
    static const u8 BLOCK_ENTRY_SIZE = 48;
};