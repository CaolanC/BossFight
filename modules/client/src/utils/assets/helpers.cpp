#include <string>
#include <iostream>
#include <utils/assets/helpers.hpp>

namespace utils::assets {
    std::string get_asset(std::string const& path) {
        auto s = SLJA_ASSETS_DIR + path;
        return s;
    }
}