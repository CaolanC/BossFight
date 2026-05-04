#include "SceneSerializer.hpp"
#include <nlohmann/json.hpp>
#include <SerializedObject.hpp>
#include <fstream>

using json = nlohmann::json;

namespace core {

    // Saves snapshot to file.
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

    // Loads a snapshot from file.
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