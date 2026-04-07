//
// Created by ethan on 05/04/2026.
//

#pragma once

#include <string>
#include <SceneSnapshot.hpp>

#include <JSONHelper.hpp>

namespace core {
    class SceneSerializer {
    public:
        static bool save(const SceneSnapshot& snapshot, const std::string& path);
        static bool load_from_file(const std::string& path, SceneSnapshot& out_snapshot);

    private:
        shared::JSONHelper json_helper;
    };
}
