#pragma once

#include <cstdint>
#include <string>
#include <vector>

typedef uint8_t byte, u8;
typedef uint16_t ushort, u16;
typedef uint32_t uint, u32;

typedef int16_t s32;

typedef int64_t(*OddleLZ64_DecompressDef)(unsigned char* Buffer, int64_t BufferSize, unsigned char* OutputBuffer, int64_t OutputBufferSize, int32_t a, int32_t b, int64_t c, void* d, void* e, void* f, void* g, void* h, void* i, int32_t ThreadModule);

const byte AES_KEY_0[16] = {
        0xD6, 0x2A, 0xB2, 0xC1,
        0x0C, 0xC0,0x1B, 0xC5,
        0x35, 0xDB, 0x7B,0x86,
        0x55, 0xC7, 0xDC, 0x3B };

const byte AES_KEY_1[16] = {
        0x3A, 0x4A, 0x5D, 0x36,
        0x73, 0xA6,0x60, 0x58,
        0x7E, 0x63, 0xE6,0x78,
        0xE4, 0x08, 0x92, 0xB5 };

const byte NONCE_SEED[12] {
        0x84, 0xDF, 0x11, 0xC0,
        0xAC, 0xAB, 0xFA, 0x20,
        0x33, 0x11, 0x26, 0x99
};


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