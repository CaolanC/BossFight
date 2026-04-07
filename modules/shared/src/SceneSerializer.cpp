//
// Created by ethan on 05/04/2026.
//

#include "SceneSerializer.hpp"
#include <nlohmann/json.hpp>
#include <SerializedObject.hpp>
#include <fstream>

using json = nlohmann::json;

namespace core {
    static json serialize_object(const SerializedObject& obj) {
        return json{
            {"objectID", obj.objectID},
            {"modelpath", obj.model_path},
            {"model_ref", obj.model_ref.str()},
            {"position", {obj.position.x, obj.position.y, obj.position.z}},
            {"rotation", {obj.rotation.w, obj.rotation.x, obj.rotation.y, obj.rotation.z}},
            {"scale", obj.scale}
        };
    }

    static SerializedObject deserialize_object(const nlohmann::json& j) {
        SerializedObject obj;

        obj.objectID = j.at("objectID").get<std::string>();
        obj.model_path = j.at("modelpath").get<std::string>();
        obj.model_ref = xg::Guid(j.at("model_ref").get<std::string>());
        auto pos = j.at("position");
        obj.position = glm::vec3(pos[0], pos[1], pos[2]);
        auto rot = j.at("rotation");
        obj.rotation = glm::quat(rot[0], rot[1], rot[2], rot[3]);
        obj.scale = j.at("scale").get<float>();

        return obj;
    }

    bool SceneSerializer::save(const SceneSnapshot& snapshot, const std::string& path) {
        json j;
        for (const auto& [id, obj] : snapshot.getmap()) {
            j["objects"][id] = serialize_object(obj);
        }

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

        nlohmann::json j;
        file >> j;

        if (!j.contains("objects")) {
            return false;
        }

        for (auto& [id, value] : j["objects"].items()) {
            SerializedObject obj = deserialize_object(value);
            out_snapshot.insert(id, obj);
        }

        return true;
    }
}