#pragma once

#include <entt/entt.hpp>
#include <core/MeshManager.hpp>
#include <core/Mesh.hpp>
#include "component/MeshRef.hpp"
#include <systems/RenderUtils.hpp>
#include <core/ShaderProgramManager.hpp>
#include <component/Core.hpp>

namespace systems {
    void Render(entt::registry& reg) {
        core::MeshManager& mesh_manager = reg.ctx().get<component::mesh_manager>().manager;
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float) INIT_SCREEN_WIDTH / (float) INIT_SCREEN_HEIGHT, 0.0f, 100.0f);
        glm::mat4 view_matrix = glm::inverse(reg.get<component::transform>(reg.ctx().get<component::current_camera>().e));

        auto& mesh_m = reg.ctx().get<component::mesh_manager>();
        auto view = reg.view<component::mesh_ref, component::mat_ref>();
        for (auto [e, mesh_ref, mat_ref]: view.each()) {
            core::Mesh mesh = mesh_manager.get_mesh(mesh_ref.id);
            unsigned int program = reg.ctx().get<component::material_manager>().manager.get_program(mat_ref.id);

            systems::Utils::RenderUtils::set_view_mat(view_matrix, program);
            systems::Utils::RenderUtils::set_projection_mat(projection, program);

            mesh.bind_vao();
            glUseProgram(program);
            glDrawElements(
                GL_TRIANGLES,      // mode
                mesh.get_index_count(),    // count
                GL_UNSIGNED_INT,   // type
                (void*)0           // element array buffer offset
            );
        }
    }

void NewRender(entt::registry& reg) {

    auto& model_manager = reg.ctx().get<component::model_manager>().manager;
    auto& material_mgr  = reg.ctx().get<component::material_manager>().manager;

    glm::mat4 projection = glm::perspective(
        glm::radians(60.0f),
        static_cast<float>(INIT_SCREEN_WIDTH) / static_cast<float>(INIT_SCREEN_HEIGHT),
        0.1f, 100.0f // (near=0.1 avoids bad depth precision)
    );
        auto& curr_cam = reg.ctx().get<component::current_camera>();
    glm::mat4 view_matrix = glm::inverse(reg.get<component::transform>(curr_cam.e));

    auto view = reg.view<component::model_ref, component::mat_ref>();

    for (auto [e, mod_ref, mat_ref] : view.each()) {
        const utils::Model& model = model_manager.get_model(mod_ref.id);

        GLuint defaultProgram = material_mgr.get_program(mat_ref.id);

        glm::mat4 entity_model(1.0f);
        if (auto t = reg.try_get<component::transform>(e)) {
            entity_model = *t;
        }

        auto bindProgramAndSetGlobals = [&](GLuint program) {
            glUseProgram(program);
            systems::Utils::RenderUtils::set_view_mat(view_matrix, program);
            systems::Utils::RenderUtils::set_projection_mat(projection, program);
        };

        std::function<void(const utils::Node&, const glm::mat4&, GLuint)> drawNode;
        drawNode = [&](const utils::Node& node, const glm::mat4& parentModel, GLuint program) {

            glm::mat4 model_mat = parentModel;
            for (const auto& prim : node.mesh.primitives) {
                GLuint programToUse = program;

                bindProgramAndSetGlobals(programToUse);

                glBindVertexArray(prim.vao);
                unsigned int view_loc = glGetUniformLocation(program, "uDistance");
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
            drawNode(root, entity_model, defaultProgram);
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }
}

}