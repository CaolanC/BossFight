//
// Created by ethan on 07/04/2026.
//

#include <nlohmann/json.hpp>
#include <SceneSnapshot.hpp>
#include <SerializedObject.hpp>

namespace shared
{
    class JSONHelper {
    public:
        JSONHelper();
        std::string make_snapshot_message(core::SceneSnapshot& snapshot);
        nlohmann::json serialize_snapshot(core::SceneSnapshot& snapshot);
        nlohmann::json serialize_object(const core::SerializedObject& obj);
        core::SerializedObject deserialize_object(const nlohmann::json& j);
        core::SceneSnapshot deserialize_snapshot(const nlohmann::json& j);
    };

}
