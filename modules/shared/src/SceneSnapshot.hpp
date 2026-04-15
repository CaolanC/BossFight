//
// Created by ethan on 31/03/2026.
//

#pragma once

#include <unordered_map>
#include <optional>
#include <SerializedObject.hpp>

namespace core {
    class SceneSnapshot {
    public:
        SceneSnapshot();

        void insert(const std::string& objectID, const SerializedObject& object);
        void delete_object(const std::string& objectID);
        SerializedObject* find(const std::string& object_id);
        std::unordered_map<std::string, SerializedObject> getmap();
        const std::unordered_map<std::string, SerializedObject>& getmap() const;
        void debug_print();

    private:
        std::unordered_map<std::string, SerializedObject> object_map;
    };
}

