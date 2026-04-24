#include <string>
#include <iostream>
#include <utils/assets/helpers.hpp>
#include <filesystem>

namespace utils::assets {
    std::string get_asset(std::string const& path) {
        auto s = SLJA_ASSETS_DIR + path;
        return s;
    }

    std::string get_filepath(std::string const& path) {
        auto s = SLJA_SAVES_DIR + path;
        return s;
    };

    bool asset_exists(std::string const& path) {
        auto s = SLJA_ASSETS_DIR + path;
        if (std::filesystem::exists(s)) {
            return true;
        }
        return false;
    }

    bool filepath_exists(std::string const& path) {
        auto s = SLJA_SAVES_DIR + path;
        if (std::filesystem::exists(s)) {
            return true;
        }
        return false;
    }
}