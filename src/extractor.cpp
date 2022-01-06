//
// Created by Vatuu on 05/01/2022.
//
#include "extractor.h"
#include "file_helper.h"
#include "parse_helper.h"

#include <cmath>
#include <fstream>
#include <cassert>
#include <iostream>

#include <cryptlib.h>
#include <gcm.h>
#include <aes.h>
#include <hex.h>

namespace d2 {

    Extractor::Extractor(const std::vector<std::string>& packages, const std::string& folder, const std::string& target, ui::ExtractorWindows* window) {
        this->folder = folder;
        this->target = target;
        this->window = window;
        window->clear_output();
        if(!initOodle()) {
            throw std::runtime_error("Failed to set up Oodle Decompressor.");
        } else {
            for(const std::string& name : packages)
                this->packages.insert(std::pair<std::string, Package>(name, Package(folder, name, true)));
            u32 pkgCount = this->packages.size();
            u32 patchCount = 0;
            for(const auto& entry : this->packages)
                patchCount += entry.second.patches.size();
            window->add_output_line(parsing::format("Ready to extract %d packages. (%d Patches)", pkgCount, patchCount));
        }
    }

    bool Extractor::initOodle() {
        dll = LoadLibrary(L"oo2core_8_win64.dll");
        if(dll == nullptr)
            return false;
        oodle = (int64_t)GetProcAddress(dll, "OodleLZ_Decompress");
        return oodle;
    }

    void Extractor::decompress(Block* b, std::vector<byte> data, std::vector<byte> output) const {
        ((OddleLZ64_DecompressDef)oodle)(data.data(), b->size, output.data(), Block::BLOCK_SIZE, 0, 0, 0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, 3);
    }

    void Extractor::decrypt(Block *b, const byte* nonce, std::vector<byte> source, std::vector<byte>& buffer) {
        namespace cpp = CryptoPP;
        try {
            using Flags = cpp::AuthenticatedDecryptionFilter::Flags;
            std::vector<byte> output;
            cpp::GCM<cpp::AES>::Decryption crypto;
            crypto.SetKeyWithIV((b->flags & 0x4 ? AES_KEY_1 : AES_KEY_0), sizeof(AES_KEY_0), nonce);
            cpp::AuthenticatedDecryptionFilter filter(crypto, new cpp::VectorSink(output), Flags::MAC_AT_END | Flags::THROW_EXCEPTION, 16);
            filter.ChannelPut(CryptoPP::DEFAULT_CHANNEL, source.data(), source.size());
            filter.ChannelPut(CryptoPP::DEFAULT_CHANNEL, (const byte*)b->gcmTag.c_str(), b->gcmTag.size());
            filter.MessageEnd();
            buffer.insert(buffer.begin(), output.begin(), output.end());
            assert(filter.GetLastResult() == true);
        } catch(cpp::Exception& e) {
            window->add_output_line(parsing::format("--Failed to decrypt!\n%s", e.what()));
        }
    }

    void Extractor::extract() {
        d2::files::mkdirs(target);
       for(const auto& p : packages) {
           std::map<u8, std::ifstream> streams;
           window->add_output_line(parsing::format("Extracting %s... (%d Patches)", p.first.c_str(), p.second.patches.size()));
           window->add_output_line(parsing::format("Entry Count: %d", p.second.entryTable.size()));
           for(const auto& patch : p.second.patches)
               streams.insert(std::make_pair(patch.first, std::ifstream(patch.second.path, std::ios::in)));
           for(Entry e : p.second.entryTable) {
               window->add_output_line(parsing::format("-Extracting Entry #%d...", e.id));
               window->add_output_line(parsing::format("-Name: %s", e.fileName.c_str()));
               window->add_output_line(parsing::format("-Size: %d Bytes", e.fileSize));
               std::vector<byte> buffer(e.fileSize);
               process_entry(e, p.second, streams, buffer);
               std::filesystem::path targetFolder(target);
               targetFolder.append(p.second.name);
               files::mkdirs(targetFolder.string());
               targetFolder.append(e.fileName + (e.fileType == "Unknown" ? ".bin" : e.fileType));
               std::ofstream output(targetFolder, std::ios::out);
               output.write((char*)buffer.data(), e.fileSize);
               window->add_output_line("-Entry complete.");
           }
           window->add_output_line("Package complete.");
       }
        window->add_output_line("----------------------");
        window->add_output_line("Extraction complete.");
    }

    void Extractor::process_entry(Entry& entry, Package pkg, std::map<u8, std::ifstream>& streams, std::vector<byte>& buffer) {
        u16 currentBlock = entry.startingBlock;
        u32 blockOffset = entry.startBlockOffset;
        auto blockCount = (size_t)(std::floor(blockOffset + entry.fileSize - 1) / Block::BLOCK_SIZE);
        window->add_output_line(parsing::format("-Block Count: %d", blockCount));
        u16 lastBlock = currentBlock + blockCount;
        while(currentBlock <= lastBlock) {
            Block b = pkg.blockTable[currentBlock];
            window->add_output_line(parsing::format("--Extracting Block #%d...", b.id));
            if(!streams.count(b.patchId)) {
                window->add_output_line(parsing::format("--Patch #%d not found! Skipping...", b.patchId));
                currentBlock++;
                continue;
            }
            std::ifstream &stream = streams[b.patchId];
            process_block(&b, blockOffset, stream, buffer, pkg.nonce, currentBlock == entry.startingBlock);
            window->add_output_line(parsing::format("--Block complete.", b.patchId));
            currentBlock++;
        }
    }

    void Extractor::process_block(Block *b, u32 offset, std::ifstream& stream, std::vector<byte>& buffer, const byte *nonce, bool isStarting) {
        std::vector<byte> blockData(b->size);
        stream.seekg(b->offset, std::ios::beg);
        stream.read((char*)blockData.data(), blockData.size());
        if(b->flags & 0x2) {
            window->add_output_line(parsing::format("--Decrypting Block #%d", b->id));
            decrypt(b, nonce, blockData, blockData);
        }
        if(b->flags & 0x1) {
            window->add_output_line(parsing::format("--Decompressing Block #%d", b->id));
            decompress(b, blockData, blockData);
        }

        if(isStarting)
            buffer.insert(buffer.begin(), blockData.begin() + offset, blockData.end());
        else
            buffer.insert(buffer.end(), blockData.begin(), blockData.end());
    }
}
