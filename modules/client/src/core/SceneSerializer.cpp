//
// Created by ethan on 05/04/2026.
//

#include "SceneSerializer.hpp"
#include <nlohmann/json.hpp>
#include <core/SerializedObject.hpp>
#include <fstream>

using json = nlohmann::json;

namespace core {
    static json serialize_object(SerializedObject& obj) {
        return json{
            {"objectID", obj.objectID},
            {"modelpath", obj.model_path},
            {"model_ref", obj.model_ref.str()},
            {"position", {obj.position.x, obj.position.y, obj.position.z}},
            {"rotation", {obj.rotation.w, obj.rotation.x, obj.rotation.y, obj.rotation.z}},
            {"scale", obj.scale}
        };
    }

    bool save(SceneSnapshot& snapshot, std::string& path) {

    }
}