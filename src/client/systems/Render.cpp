#include <entt/entt.hpp>
#include <core/MeshManager.hpp>
#include <core/Mesh.hpp>
#include "component/MeshRef.hpp"
#include <component/MatRef.hpp>
#include <core/ShaderProgramManager.hpp>
#include <component/Core.hpp>
#include <utils/gl/helpers.hpp>
#include <systems/Render.hpp>

namespace systems {
    void NewRender(entt::registry& reg) {
        GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
        auto& model_manager = reg.ctx().get<component::model_manager>().manager;
        auto& material_mgr  = reg.ctx().get<component::material_manager>().manager;

        glm::mat4 projection = glm::perspective(
            glm::radians(60.0f),
            static_cast<float>(INIT_SCREEN_WIDTH) / static_cast<float>(INIT_SCREEN_HEIGHT),
            0.1f, 1000.0f // (near=0.1 avoids bad depth precision)
        );
            auto& curr_cam = reg.ctx().get<component::current_camera>();
        glm::mat4 view_matrix = glm::inverse(reg.get<component::transform>(curr_cam.e));
            glm::vec3 camera_position = reg.get<component::position>(curr_cam.e);

        auto view = reg.view<component::model_ref, component::mat_ref, component::transform>();

        for (auto [e, mod_ref, mat_ref, tr] : view.each()) {
            const rendering::Model& model = model_manager.get_model(mod_ref.id);

            GLuint defaultProgram = material_mgr.get_program(mat_ref.id);

            glm::mat4 entity_model(1.0f);
            if (auto t = reg.try_get<component::transform>(e)) {
                entity_model = *t;
            }

            auto bindProgramAndSetGlobals = [&](GLuint program) {
                glUseProgram(program);
                utils::gl::set_view_mat(view_matrix, program);
                utils::gl::set_projection_mat(projection, program);
                // utils::gl::set_model_mat(tr, program);
                utils::gl::set_campos(camera_position, program);
            };

            std::function<void(const rendering::Node&, const glm::mat4&, GLuint)> drawNode;
            drawNode = [&](const rendering::Node& node, const glm::mat4& parentModel, GLuint program) {
                glm::mat4 model_mat = parentModel;
                if (node.has_local_transform) {
                    model_mat = parentModel * node.local_transform;
                }
                for (const auto& prim : node.mesh.primitives) {
                    GLuint programToUse = program;
                    bindProgramAndSetGlobals(programToUse);

                    glBindVertexArray(prim.vao);
                    utils::gl::set_model_mat(model_mat, program);
                    unsigned int view_loc = glGetUniformLocation(program, "uDistance");
                    // unsigned int cam_pos = glGetUniformLocation(program, "uCamPos");
                    // glUniform3fv(cam_pos, 3, glm::value_ptr(camera_position));
                    if (prim.indexCount > 0) {
                        glDrawElements(prim.mode, prim.indexCount, prim.indexType, nullptr);
                    } else {
                        // TODO: store vertexCount in GpuPrimitive for non-indexed draws
                        glDrawArrays(prim.mode, 0, prim.vertexCount);
                    }
                }

                for (const auto& child : node.children) {
                    drawNode(child, model_mat, program);
                }
            };

            bindProgramAndSetGlobals(defaultProgram);
            for (const auto& root : model.root_nodes) {
                drawNode(root, tr, defaultProgram);
            }

            glBindVertexArray(0);
            glUseProgram(0);
        }
    }

}