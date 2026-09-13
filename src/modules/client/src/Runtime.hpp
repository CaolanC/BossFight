#pragma once

#include <entt/entt.hpp>
#include <rendering/ResourceManager.hpp>
#include <Renderer.hpp>

#include <vector>

namespace client {

class Runtime {
	public:
	Runtime();
	private:
	rendering::ResourceManager resource_manager = rendering::ResourceManager();
	client::Renderer renderer = client::Renderer(resource_manager);
	std::vector<void (**)(entt::registry&)> systems;
};

};
