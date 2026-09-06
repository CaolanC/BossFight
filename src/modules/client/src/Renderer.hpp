#pragma once

#include <rendering/ResourceManager.hpp>
#include <glm/glm.hpp>

namespace client {
	class Renderer {
		public:
			Renderer(rendering::ResourceManager& resource_manager);
			void render(entt::registry& reg, int viewport_width, int viewport_height, core::ModelManager model_manager, core::ShaderProgramManager material_mgr);
			void new_render(entt::registry& reg, int viewport_width, int viewport_height);
			void set_camera_ubo(const glm::vec3& camera_position, const glm::mat4& projection_matrix, const glm::mat4& view_matrix);
			void set_lighting_ubo(const glm::vec4& ambient);
			void init();
			void init_ubos();
		private:
			unsigned int camera_ubo, lighting_ubo;
			rendering::ResourceManager& resource_manager;
			unsigned int default_shader_program;
	};
}
