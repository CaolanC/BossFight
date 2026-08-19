#pragma once

#include <rendering/ResourceManager.hpp>

namespace client {
	class Renderer {
		public:
			Renderer();
			void render(entt::registry& reg, int viewport_width, int viewport_height, core::ModelManager model_manager, core::ShaderProgramManager material_mgr);
			void new_render(entt::registry& reg, int viewport_width, int viewport_height, rendering::ResourceManager resource_manager);
	};
}
