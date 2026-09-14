#include <systems/RenderSystem.hpp>

namespace systems {

#ifdef RENDERSYSTEM

RenderSystem::run(entt::registry& reg) {
    LightingUBOCPU l_ubo_cpu;
    auto basic_light_view = reg.view<component::basic_light, shared::component::position>();
    int no_lights = 0;
    for (auto [e, basic_light, pos] : basic_light_view.each()) { // Don't know if we want lights to have meshes or just a parent component that has both a light and a mesh, will have to see how it does
        if (no_lights >= 100) {
            break;
        };
        PointLightCPU pl;
        pl.position = glm::vec4(pos.value, 1.0f);
        pl.color = basic_light.color;
        l_ubo_cpu.point_lights[no_lights] = pl;
        no_lights++;
    }
    l_ubo_cpu.no_lights = no_lights;

    set_lighting_ubo(l_ubo_cpu); // This probabaly works for now actually. Just need to make it call the Renderer instead. ;) And make it take a list
		// of lights instead of the light ubu, maybe a struct with the size

    for (auto [e, mesh, transform] : view.each()) { // Basic lighting system, need to give this more thought but lets go with this for now
        mesh.handle;
        // We probabaly need to fix up the material thing before we can do this
        GPUMesh& gpu_mesh = resource_manager.mesh_assets.at(mesh.mesh_handle).gpu_mesh.value(); // We pass in the mesh handle instead
        //glBindVertexArray(gpu_mesh.vao);
        GLuint shader_program = resource_manager.shader_program_manager.program_map.at(resource_manager.shader_program_manager.default_program);
        // We pass in the shader program handle instead
        //glUseProgram(shader_program);

        utils::gl::set_model_mat(transform, shader_program);
        // Will have to look at this a bit more in depth

        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, gpu_mesh.texture);

        //GLint tex_location = glGetUniformLocation(shader_program, "uTex");
        //glUniform1i(tex_location, 0);

        //if (true) {
        //    glDrawElements(gpu_mesh.draw_mode, gpu_mesh.count, gpu_mesh.index_type, nullptr);
        //} else {
        //    glDrawArrays(gpu_mesh.draw_mode, 0, gpu_mesh.count);
        //}
    }


};

#endif

};

