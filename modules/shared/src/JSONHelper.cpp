//
// Created by ethan on 07/04/2026.
//

#include <JSONHelper.hpp>

namespace shared {
    JSONHelper::JSONHelper() {}

    std::string JSONHelper::make_snapshot_message(core::SceneSnapshot& snapshot) {
        nlohmann::json message;
        message["type"] = "snapshot";
        message["payload"] = serialize_snapshot(snapshot);

    }

    nlohmann::json JSONHelper::serialize_snapshot(core::SceneSnapshot& snapshot) {
        nlohmann::json j;
        for (const auto& [id, obj] : snapshot.getmap()) {
            j["objects"][id] = serialize_object(obj);
        }
    }

    nlohmann::json serialize_object(const core::SerializedObject& obj) {
        return nlohmann::json{
                {"objectID", obj.objectID},
                {"modelpath", obj.model_path},
                {"model_ref", obj.model_ref.str()},
                {"position", {obj.position.x, obj.position.y, obj.position.z}},
                {"rotation", {obj.rotation.w, obj.rotation.x, obj.rotation.y, obj.rotation.z}},
                {"scale", obj.scale}
        };
    }

    core::SerializedObject deserialize_object(const nlohmann::json& j) {
        core::SerializedObject obj;

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

    core::SceneSnapshot deserialize_snapshot(const nlohmann::json& j) {
        core::SceneSnapshot snapshot;

        for (auto& [id, value] : j["objects"].items()) {
            core::SerializedObject obj = deserialize_object(value);
            snapshot.insert(id, obj);
        }
    }
}