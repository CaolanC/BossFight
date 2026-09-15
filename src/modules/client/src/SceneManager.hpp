#pragma once

#include <entt/entt.hpp>

#include <map>
#include <string>
#include <vector>

namespace runtime {

class SceneManager {
	public:
	SceneManager();
	entt::registry& get_scene(const std::string scene_name);
	entt::registry& new_scene(const std::string scene_name);
	std::vector<std::string> get_scene_names();

	private:
	std::unordered_map<std::string, entt::registry> scene_map;
	entt::entity spawn(std::function<entt::entity(entt::registry& registry)>const& spawn_function);

};


};
