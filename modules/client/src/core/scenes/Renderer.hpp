#pragma once

#include <core/Scene.hpp>
#include <core/defines.hpp>
#include <core/MeshManager.hpp>
#include <core/ShaderProgramManager.hpp>
#include <stdio.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp> 
#include <vector>
#include <thread>


namespace core
{
    // class Renderer
    // {
    // public:
    //
    //     Renderer(core::MeshManager meshes, core::ShaderProgramManager programs) : meshes(meshes), programs(programs) {
    //
    //     }
    //
    //     void render(core::Scene& scene) {
    //         const std::vector<core::Object> objects = scene.get_objects();
    //
    //         for(core::Object obj : objects) {
    //             if (obj.is_renderable()) {
    //                 render_object(std::ref(obj), scene);
    //             }
    //             std::jthread([this](core::Object &o) {
    //                  o.run_frame_scripts(); // Scripts that run every frame
    //             }, std::ref(obj));
    //
    //         }
    //
    //     };
    //
    //
    //     void render_object(core::Object &obj, core::Scene& scene) {
    //         core::Mesh mesh = meshes.get_mesh(obj.get_mesh_handle());
    //         unsigned int program = programs.get_program(obj.get_program_handle());
    //         set_view_mat(glm::inverse(scene.get_camera_transform()), program);
    //         mesh.bind_vao();
    //
    //         glUseProgram(program);
    //         set_projection_mat(program);
    //         glDrawElements(
    //             GL_TRIANGLES,      // mode
    //             mesh.get_index_count(),    // count
    //             GL_UNSIGNED_INT,   // type
    //             (void*)0           // element array buffer offset
    //         );
    //     }
    //
    //     void set_projection_mat(unsigned int program) {
    //        unsigned int projection_loc = glGetUniformLocation(program, "uProjection");
    //         if (projection_loc == -1) SDL_Log("WARN: uProjection not found"); {
    //             glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));
    //         };
    //     }
    //
    //     void set_view_mat(const glm::mat4& view_mat, unsigned int program) {
    //         unsigned int view_loc = glGetUniformLocation(program, "uView");
    //         if (view_loc == -1) SDL_Log("WARN: uView not found"); {
    //             glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_mat));
    //         };
    //     }
    //
    //     private:
    //         glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float) INIT_SCREEN_WIDTH / (float) INIT_SCREEN_HEIGHT, 0.0f, 100.0f);
    //         core::ShaderProgramManager programs;
    //         core::MeshManager meshes;
    // };
}
