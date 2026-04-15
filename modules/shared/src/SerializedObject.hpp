#pragma once

#include <string>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "crossguid/guid.hpp"

namespace core
{
    struct SerializedObject {
        std::string objectID;
        std::string model_path;
        xg::Guid model_ref;
        glm::vec3 position;
        glm::quat rotation;
        float scale = 1.0f;
    };
}
