//
// Created by ethan on 05/04/2026.
//

#include "SceneSerializer.hpp"
#include <nlohmann/json.hpp>
#include <SerializedObject.hpp>
#include <fstream>

using json = nlohmann::json;

namespace core {

    bool SceneSerializer::save(const SceneSnapshot& snapshot, const std::string& path) {
        json j = shared::JSONHelper::serialize_snapshot(snapshot);

        std::ofstream file(path);

        if (!file.is_open()) {
            return false;
        }

        file << j.dump(4);

        std::cout << "File written\n";

        return true;
    }

    bool SceneSerializer::load_from_file(const std::string& path, SceneSnapshot& out_snapshot) {
        std::ifstream file(path);

        if (!file.is_open()) {
            return false;
        }

        json j;
        file >> j;

        if (!j.contains("objects")) {
            return false;
        }

        std::cout << j << "\n";

        shared::JSONHelper::deserialize_snapshot_pointer(j, out_snapshot);

        return true;
    }
}