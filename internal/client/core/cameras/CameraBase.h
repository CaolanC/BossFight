#pragma once

#include <core/Object.hpp>

#include "glm/fwd.hpp"
#include "glm/detail/type_quat.hpp"

namespace core::cameras
{
    class IViewMatrix {
        public:
        IViewMatrix() = default;
        virtual ~IViewMatrix() = default;
        [[nodiscard]]
        virtual glm::mat4 get_view_matrix() const = 0;
    };

    class CameraBase : public core::Object
    {
        public:
        CameraBase() : Object() {

        };
    };

    class FreeCamera : public CameraBase, public IViewMatrix
    {
        public:
        explicit FreeCamera() : CameraBase(), IViewMatrix() {
            // auto script = RunScript([](){
            //     return 0;
            // });
            // push_init_script(script);
        };

        [[nodiscard]]
        glm::mat4 get_view_matrix() const override {
            return glm::lookAt(
                glm::vec3(0, 0, 0),
                glm::vec3(0, 0, 1),
                glm::vec3(0, 0, 1)
            );
        }


    };
}
