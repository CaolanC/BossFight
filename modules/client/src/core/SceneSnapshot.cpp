//
// Created by ethan on 31/03/2026.
//

#include <core/SceneSnapshot.hpp>
#include <core/SerializedObject.hpp>

namespace core{
    SceneSnapshot::SceneSnapshot() {
    }

    void SceneSnapshot::insert(const std::string& objectID, const SerializedObject& object) {
        object_map[objectID] = object;
    }

    SerializedObject* SceneSnapshot::find(const std::string& object_id) {
        auto it = object_map.find(object_id);
        if (it == object_map.end()) {
            return nullptr;
        }
        return &it->second;
    }

    std::unordered_map<std::string, SerializedObject> SceneSnapshot::getmap() {
        return object_map;
    }

    const std::unordered_map<std::string, SerializedObject>& SceneSnapshot::getmap() const {
        return object_map;
    }
}