#pragma once

namespace utils::assets {
    std::string get_asset(std::string const& path);
    std::string get_filepath(std::string const& path);
    bool asset_exists(std::string const& path);
    bool filepath_exists(std::string const& path);
}