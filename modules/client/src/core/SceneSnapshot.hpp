//
// Created by ethan on 31/03/2026.
//

#include <unordered_map>
#include <optional>
#include <core/SerializedObject.hpp>

namespace core {
    class SceneSnapshot {
    public:
        SceneSnapshot();

        void insert(const std::string& objectID, const SerializedObject& object);
        SerializedObject* find(const std::string& object_id);
        std::unordered_map<std::string, SerializedObject> getmap();

    private:
        std::unordered_map<std::string, SerializedObject> object_map;
    };
}

