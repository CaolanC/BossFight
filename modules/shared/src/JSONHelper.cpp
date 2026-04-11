//
// Created by ethan on 07/04/2026.
//

#include <JSONHelper.hpp>

namespace shared {

    std::string JSONHelper::make_handshake(xg::Guid client_id, bool is_host) {
        nlohmann::json message;
        message["type"] = "handshake";
        message["payload"] = serialize_client_info(client_id, is_host);

        return message.dump();
    }

    std::string JSONHelper::make_handshake_ack() {
        nlohmann::json message;
        message["type"] = "handshake_ack";

        return message.dump();
    }

    std::string JSONHelper::make_snapshot_message(const core::SceneSnapshot& snapshot) {
        nlohmann::json message;
        message["type"] = "snapshot";
        message["payload"] = serialize_snapshot(snapshot);

        return message.dump();
    }

    nlohmann::json JSONHelper::serialize_snapshot(const core::SceneSnapshot& snapshot) {
        nlohmann::json j;
        for (const auto& [id, obj] : snapshot.getmap()) {
            j["objects"][id] = serialize_object(obj);
        }

        return j;
    }

    nlohmann::json JSONHelper::serialize_object(const core::SerializedObject& obj) {
        return nlohmann::json{
                {"objectID", obj.objectID},
                {"modelpath", obj.model_path},
                {"model_ref", obj.model_ref.str()},
                {"position", {obj.position.x, obj.position.y, obj.position.z}},
                {"rotation", {obj.rotation.w, obj.rotation.x, obj.rotation.y, obj.rotation.z}},
                {"scale", obj.scale}
        };
    }

    core::SerializedObject JSONHelper::deserialize_object(const nlohmann::json& j) {
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

    nlohmann::json JSONHelper::serialize_client_info(xg::Guid client_id, bool is_host) {
        return nlohmann::json{
            {"client_id", client_id.str()},
            {"role", is_host ? "host" : "client"}
        };
    }

    core::SceneSnapshot JSONHelper::deserialize_snapshot_string(const std::string& msg) {
        nlohmann::json j = nlohmann::json::parse(msg);
        return deserialize_snapshot(j);
    }

    core::SceneSnapshot JSONHelper::deserialize_snapshot(const nlohmann::json& j) {
        core::SceneSnapshot snapshot;

        for (auto& [id, value] : j["objects"].items()) {
            core::SerializedObject obj = deserialize_object(value);
            snapshot.insert(id, obj);
        }

        return snapshot;
    }

    void JSONHelper::deserialize_snapshot_pointer(const nlohmann::json& j, core::SceneSnapshot& snapshot) {
        for (auto& [id, value] : j["objects"].items()) {
            core::SerializedObject obj = deserialize_object(value);
            snapshot.insert(id, obj);
        }
    }
}