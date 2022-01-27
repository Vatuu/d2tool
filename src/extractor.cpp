#include "extractor.h"
#include "file_helper.h"
#include "parse_helper.h"

#include <cmath>
#include <fstream>
#include <cassert>
#include <iostream>

#include <openssl/err.h>
#include <openssl/evp.h>

namespace d2::extract {

    /*static void debug_write_block(std::string_view target, std::string_view name, const byte *data, const size_t size) {
        std::filesystem::path bin(target);
        bin.append("debug");
        std::filesystem::create_directories(bin);
        bin.append(name);
        std::ofstream binOut(bin, std::ios::out | std::ios::binary);
        binOut.write((const char *)data, size);
    }*/

    Extractor::Extractor(const std::vector<std::string>& packages, const std::string& folder, const std::string& target, ExtractCallbacks *callbacks) {
        this->folder = folder;
        this->target = target;
        this->callbacks = callbacks;
        if(!initOodle()) {
            callbacks->on_error(Error::OODLE_SETUP_FAILED, (std::monostate()), std::nullopt);
            throw std::runtime_error("Failed to set up Oodle Decompressor.");
        } else {
            for(const std::string& name : packages)
                this->packages.insert(std::pair<std::string, Package>(name, Package(folder, name, true)));
            u32 pkgCount = this->packages.size();
            u32 patchCount = 0;
            for(const auto& entry : this->packages)
                patchCount += entry.second.patches.size();
        }
    }

    bool Extractor::initOodle() {
        dll = LoadLibrary(L"oo2core_8_win64.dll");
        if(dll == nullptr)
            return false;
        oodle = (int64_t)GetProcAddress(dll, "OodleLZ_Decompress");
        return oodle;
    }

    bool Extractor::decompress(Block* b, std::vector<byte> &data) const {
        std::vector<byte> buffer(Block::BLOCK_SIZE);
        s64 ret = ((OodleLZ_Decompress)oodle)(data.data(), b->size, buffer.data(), Block::BLOCK_SIZE, 0, 0, 0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, 3);
        if(ret > 0) {
            data = buffer;
            return true;
        } else {
            callbacks->on_error(Error::OODLE_DECOMPRESS_FAILED, (b), parsing::format("Oodle is being a bitch! [%u]", ret));
            return false;
        }
    }

    bool Extractor::decrypt(Block *b, const byte* nonce, std::vector<byte>& buffer) {
        buffer.resize(b->size);
        int decryptedLength, finalLength;
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        assert(EVP_DecryptInit_ex(ctx, EVP_aes_128_gcm(), nullptr, b->flags & 0x4 ? AES_KEY_1 : AES_KEY_0, nonce));
        assert(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, b->gcmTag.size(), b->gcmTag.data()));
        assert(EVP_DecryptUpdate(ctx, buffer.data(), &decryptedLength, buffer.data(), b->size));
        int ret = EVP_DecryptFinal_ex(ctx, buffer.data() + decryptedLength, &finalLength);
        EVP_CIPHER_CTX_free(ctx);
        if(ret > 0)
            return true;
        else
            callbacks->on_error(Error::DECRYPT_FAILED, (b), "GCM Tag Verification failed!");
        return false;
    }

    void Extractor::extract() {
        d2::files::mkdirs(target);
       for(const auto& p : packages) {
           callbacks->on_package_start(&p.second, p.first);
           std::map<u8, std::ifstream> streams;
           streams[p.second.header.patchId] = std::ifstream (p.second.path, std::ios::in | std::ios::binary);
           for(const auto& patch : p.second.patches)
               streams[patch.header.patchId] = std::ifstream(patch.path, std::ios::in | std::ios::binary);
           for(Entry e : p.second.entryTable) {
               if(!callbacks->on_entry_start(&e))
                   continue;
               std::vector<byte> buffer(e.fileSize);
               if(!process_entry(e, p.second, streams, buffer)) {
                   callbacks->on_extract_end(true);
                   return;
               }
               std::filesystem::path targetFolder(target);
               targetFolder.append(p.second.name);
               files::mkdirs(targetFolder.string());
               targetFolder.append(e.fileName + (e.fileType == "Unknown" ? ".bin" : e.fileType));
               std::ofstream output(targetFolder, std::ios::out);
               output.write((char*)buffer.data(), e.fileSize);
               callbacks->on_entry_finish(&e);
           }
           callbacks->on_package_finish(&p.second, p.first);
       }
       callbacks->on_extract_end(false);
    }

    bool Extractor::process_entry(Entry& entry, Package pkg, std::map<u8, std::ifstream>& streams, std::vector<byte>& buffer) {
        u16 currentBlock = entry.startingBlock;
        u32 blockOffset = entry.startBlockOffset;
        auto blockCount = (size_t)(std::floor(blockOffset + entry.fileSize - 1) / Block::BLOCK_SIZE);
        callbacks->log(parsing::format("-Block Count: %d", blockCount + 1));
        u16 lastBlock = currentBlock + blockCount;
        while(currentBlock <= lastBlock) {
            Block b = pkg.blockTable[currentBlock];
            callbacks->on_block_start(&b);
            if(streams.count(b.patchId) != 1) {
                callbacks->on_error(Error::MISSING_PATCH, (&b), std::nullopt);
                return false;
            }
            std::ifstream &stream = streams[b.patchId];
            if(!process_block(&b, blockOffset, stream, buffer, pkg.nonce, currentBlock == entry.startingBlock))
                return false;
            currentBlock++;
            callbacks->on_block_finish(&b);
        }
        return true;
    }

    bool Extractor::process_block(Block *b, u32 offset, std::ifstream& stream, std::vector<byte>& buffer, const byte *nonce, bool isStarting) {
        std::vector<byte> blockData(b->size);
        stream.seekg(b->offset, std::ios::beg);
        stream.read((char*)blockData.data(), blockData.size());

        if(b->flags & 0x2) {
            callbacks->log("--Decrypting...");
            if(!decrypt(b, nonce, blockData))
                return false;
        }
        if(b->flags & 0x1) {
            callbacks->log("--Decompressing...");
            if(!decompress(b, blockData))
                return false;
        }

        if(isStarting)
            buffer.insert(buffer.begin(), blockData.begin() + offset, blockData.end());
        else
            buffer.insert(buffer.end(), blockData.begin(), blockData.end());

        return true;
    }
}
