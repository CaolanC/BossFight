#pragma once

#include <rendering/ResourceManager.hpp>
#include <glm/glm.hpp>

namespace client {

	struct PointLightCPU { // This don't belong here but I'm lazy, we can move it to the rendering namespace and its own file once the renderer is migrating as well
	    glm::vec4 position;
	    glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	};

	struct LightingUBOCPU {
		glm::vec4 ambient = glm::vec4(1.0f, 1.0f, 1.0f, 0.05f);
		PointLightCPU point_lights[100];
		int32_t no_lights = 0;
	};

	class Renderer {
		public:
			Renderer(rendering::ResourceManager& resource_manager);
			void render(entt::registry& reg, int viewport_width, int viewport_height, core::ModelManager model_manager, core::ShaderProgramManager material_mgr);
			void new_render(entt::registry& reg, int viewport_width, int viewport_height);
			void set_camera_ubo(const glm::vec3& camera_position, const glm::mat4& projection_matrix, const glm::mat4& view_matrix);
			void set_lighting_ubo(const LightingUBOCPU& lighting_ubo_cpu);
			void init();
			void init_ubos();
		private:
			unsigned int camera_ubo, lighting_ubo;
			rendering::ResourceManager& resource_manager;
			unsigned int default_shader_program;
	};
}
