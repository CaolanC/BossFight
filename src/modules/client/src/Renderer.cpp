#include <entt/entt.hpp>
#include <core/MeshManager.hpp>
#include <core/Mesh.hpp>
#include "component/MeshRef.hpp"
#include <component/MatRef.hpp>
#include <core/ShaderProgramManager.hpp>
#include <component/Core.hpp>
#include <utils/gl/helpers.hpp>
#include <systems/Render.hpp>
#include <SharedComponents.hpp>
#include <Renderer.hpp>
#include <rendering/ResourceManager.hpp>
#include <glad/glad.h>

#include <iostream>


namespace client {

	Renderer::Renderer(rendering::ResourceManager& resource_manager) : resource_manager(resource_manager) {

	}

    void Renderer::init() {
    }

    void Renderer::init_ubos() {
	glGenBuffers(1, &camera_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, camera_ubo);
	glBufferData(
		GL_UNIFORM_BUFFER,
		sizeof(glm::mat4) * 2 + sizeof(glm::vec4),
		nullptr,
		GL_DYNAMIC_DRAW
	);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &lighting_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, lighting_ubo);
	glBufferData(
		GL_UNIFORM_BUFFER,
		sizeof(LightingUBOCPU),
		nullptr,
		GL_DYNAMIC_DRAW
	);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, lighting_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

    }

    void Renderer::set_camera_ubo(const glm::vec3& camera_position, const glm::mat4& projection_matrix, const glm::mat4& view_matrix) {
	glBindBuffer(GL_UNIFORM_BUFFER, camera_ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection_matrix));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view_matrix));
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(camera_position));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
    };

    void Renderer::set_lighting_ubo(const LightingUBOCPU& lighting_ubo_cpu) {
	glBindBuffer(GL_UNIFORM_BUFFER, lighting_ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightingUBOCPU), &lighting_ubo_cpu);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
    };

    void Renderer::new_render(entt::registry& reg, int viewport_width, int viewport_height) {
	float aspect = 1.0f;
	if (viewport_height > 0) {
		aspect = static_cast<float>(viewport_width) / static_cast<float>(viewport_height);
	};
	
	glm::mat4 projection_matrix = glm::perspective(
	    glm::radians(60.0f),
	    aspect,
	    0.1f,
	    1000.0f
	);

	auto& curr_cam = reg.ctx().get<component::current_camera>();
	glm::mat4 view_matrix = glm::inverse(
	    reg.get<shared::component::transform>(curr_cam.e)
	);
	glm::vec3 camera_position = reg.get<shared::component::position>(curr_cam.e).value;

	set_camera_ubo(camera_position, projection_matrix, view_matrix);


	// Get all the lights then upload them to the shader I think to start.
	
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
	
	set_lighting_ubo(l_ubo_cpu);

	auto view = reg.view<component::mesh, shared::component::transform>(); // Need the material as well once it's implemented, but start with ambient for now.

	for (auto [e, mesh, transform] : view.each()) { // Basic lighting system, need to give this more thought but lets go with this for now
		GPUMesh& gpu_mesh = resource_manager.mesh_assets.at(mesh.mesh_handle).gpu_mesh.value();
		glBindVertexArray(gpu_mesh.vao);
		GLuint shader_program = resource_manager.shader_program_manager.program_map.at(resource_manager.shader_program_manager.default_program);
		glUseProgram(shader_program);

        utils::gl::set_model_mat(transform, shader_program);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gpu_mesh.texture);

		GLint tex_location = glGetUniformLocation(shader_program, "uTex");
		glUniform1i(tex_location, 0);

        if (true) {
        	glDrawElements(gpu_mesh.draw_mode, gpu_mesh.count, gpu_mesh.index_type, nullptr);
        } else {
                    // TODO: store vertexCount in GpuPrimitive for non-indexed draws
			std::cout << "ye\n";
            glDrawArrays(gpu_mesh.draw_mode, 0, gpu_mesh.count);
        }
                //else {
                //    // TODO: store vertexCount in GpuPrimitive for non-indexed draws
                //    glDrawArrays(prim.mode, 0, prim.vertexCount);
                //}
		// Okay so what we want to do tommorow is get it so that we have basic mesh showing, with its color based purely on the
		// ambient light. That's our starting ground, material architecture decisions should emerge after that! :)
	}


	// Okay to continue the project we are going to set up the lighting next
	// basically I want the basic lighting implemented to start,
	// lighting the sink model.
	// We don't need to give the light a geometry, it is something
	// that gets attached to entities as a component
	// if that entity has a component that's the lights mesh,
	// as such light will need a local transform.
	// But then for the new render function it should be fairly easy.
	// We are starting with simple shaders that will take in the 
	// position VBO for the mesh, basic color component <-
	// 	That's how we implement different lighting styles,
	// 	the render does them in order, we can add macros to
	// 	remove in the final compile if the developer is only
	// 	using one lighting method. Basic color is just an 
	// 	rgb value. That's our first step.
	// 	Then we can add a texture component.
	// 	Then upgrade to PBR.
	// 	Better to do it in code now,
	// 	using a default texture,basic color, sink mesh
	// 	and a separate light source.
	//
	// 	The Light Component
	// 		Differnt types of components.
	// 		First one I am making is the sun.
	// 		Let's leave it at that
    }

    void Renderer::render(entt::registry& reg, int viewport_width, int viewport_height, core::ModelManager model_manager, core::ShaderProgramManager material_mgr) {
        // GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        // glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

        float aspect = 1.0f;
        if (viewport_height > 0) {
            aspect = static_cast<float>(viewport_width) / static_cast<float>(viewport_height);
        }

        glm::mat4 projection = glm::perspective(
            glm::radians(60.0f),
            aspect,
            0.1f,
            1000.0f
        );
        // std::cout << "We expect to crash here\n";
        auto& curr_cam = reg.ctx().get<component::current_camera>(); // This doesn't exist in our default registry, we can hardcode it for now maybe
        glm::mat4 view_matrix = glm::inverse(
            reg.get<shared::component::transform>(curr_cam.e)
        );
        glm::vec3 camera_position = reg.get<shared::component::position>(curr_cam.e).value;

        auto view = reg.view<component::model_ref, component::mat_ref, shared::component::transform>();

        for (auto [e, mod_ref, mat_ref, tr] : view.each()) {
            const rendering::Model& model = model_manager.get_model(mod_ref.id);

            GLuint defaultProgram = material_mgr.get_program(mat_ref.id);
            // std::cout << mat_ref.id << ' ' << mod_ref.id << '\n';

            glm::mat4 entity_model(1.0f);
            if (auto t = reg.try_get<shared::component::transform>(e)) {
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
                    glActiveTexture(GL_TEXTURE0 + 0);
                    glBindTexture(GL_TEXTURE_2D,                     prim.texture.ID);
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
        }
    }

}
