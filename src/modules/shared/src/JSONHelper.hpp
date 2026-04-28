#pragma once

#include <nlohmann/json.hpp>
#include <SceneSnapshot.hpp>
#include <SerializedObject.hpp>

namespace shared
{
    class JSONHelper {
    public:
        static std::string make_handshake(xg::Guid client_id, bool is_host);
        static std::string make_handshake_ack();
        static std::string make_update_message(const std::string& type, const core::SerializedObject& obj);
        static std::string make_snapshot_message(const core::SceneSnapshot& snapshot);
        static std::string make_session_closed_message();
        static nlohmann::json serialize_snapshot(const core::SceneSnapshot& snapshot);
        static nlohmann::json serialize_object(const core::SerializedObject& obj);
        static nlohmann::json serialize_client_info(xg::Guid client_id, bool is_host);
        static core::SerializedObject deserialize_object_string(const std::string& msg);
        static core::SerializedObject deserialize_object(const nlohmann::json& j);
        static core::SceneSnapshot deserialize_snapshot_string(const std::string& msg);
        static core::SceneSnapshot deserialize_snapshot(const nlohmann::json& j);
        static void deserialize_snapshot_pointer(const nlohmann::json& j, core::SceneSnapshot& snapshot);
    };

}
