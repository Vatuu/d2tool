#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace d2::files {

    namespace fs = std::filesystem;

    static bool dir_is_empty(const std::string& path) {
        return fs::exists(path) &&  fs::is_directory(path) && fs::is_empty(path);
    }

    static bool dir_contains_extension(const std::string& path, const std::string extension) {
        if(dir_is_empty(path))
            return false;
        return std::ranges::any_of(fs::directory_iterator(path), [&](auto const &entry) { return fs::is_regular_file(entry) && entry.path().extension() == extension; });
    }

    static std::vector<std::string> dir_unique_packages(const std::string& path, const std::string& extension) {
        std::vector<std::string> vector;
        if(dir_contains_extension(path, extension))
            for(auto const &entry : fs::directory_iterator(path))
                if(fs::is_regular_file(entry) && entry.path().extension() == extension) {
                    std::string filename = entry.path().filename().string();
                    std::string name = filename.substr(4, filename.size() - (6 + extension.size()));
                    if(std::find(vector.begin(), vector.end(), name) == vector.end())
                        vector.push_back(name);
                }

        return vector;
    }

    static std::vector<std::string> file_get_patches(const std::string &name, const std::string &path) {
        std::string prefix_name = "w64_" + name;
        std::vector<std::string> vec;
        for(auto const &entry : fs::directory_iterator(path))
            if(entry.path().filename().string().starts_with(prefix_name))
                vec.push_back(entry.path().string());
        std::sort(vec.begin(), vec.end(), std::greater<std::string>());
        return vec;
    }

    static void mkdirs(const std::string& path) {
        fs::create_directories(path);
    }
}