#pragma once

#include <variant>
#include <optional>

#include "types.h"
#include "package.h"

namespace d2::extract {

    enum class Error {
        OODLE_SETUP_FAILED,
        OODLE_DECOMPRESS_FAILED,
        DECRYPT_FAILED,
        MISSING_PATCH
    };

    struct ExtractCallbacks {

        virtual ~ExtractCallbacks() = default;

        virtual void on_error(d2::extract::Error errorType, std::variant<Package*, Entry*, Block*, std::monostate> object, std::optional<std::string> additionalInfo) = 0;
        virtual void log(const std::string& line) = 0;

        inline virtual void on_extract_start() {};
        inline virtual void on_extract_end(bool errors) {};

        virtual bool on_package_start(const Package *package, const std::string& packageName) = 0;
        inline virtual void on_package_finish(const Package *package, const std::string& packageName) {};

        virtual bool on_entry_start(const Entry *entry) = 0;
        inline virtual void on_entry_finish(const Entry *entry) {};

        virtual bool on_block_start(const Block *block) = 0;
        inline virtual void on_block_finish(const Block *block) {};
    };
}