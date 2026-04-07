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
        static std::string make_snapshot_message(const core::SceneSnapshot& snapshot);
        static nlohmann::json serialize_snapshot(const core::SceneSnapshot& snapshot);
        static nlohmann::json serialize_object(const core::SerializedObject& obj);
        static core::SerializedObject deserialize_object(const nlohmann::json& j);
        static core::SceneSnapshot deserialize_snapshot(const nlohmann::json& j);
        static void deserialize_snapshot_pointer(const nlohmann::json& j, core::SceneSnapshot& snapshot);
    };

}
