#include <SceneManager.hpp>
#include <component/Core.hpp>
#include <spawn/Spawn.hpp>

#include <entt/entt.hpp>


namespace runtime {

SceneManager::SceneManager() {
	new_scene("Default Scene");
};

entt::registry& SceneManager::new_scene(std::string scene_name) {
	entt::registry reg;

    reg.ctx().emplace<component::keyboard_state>();
    reg.ctx().emplace<component::mouse_state>();
	spawn::freecam(reg);
    //reg.ctx().emplace<component::current_camera>(spawn(spawn::freecam));
	
	auto [it, inserted] = scene_map.emplace(scene_name, std::move(reg));
	return it->second;
};

entt::entity SceneManager::spawn(std::function<entt::entity(entt::registry& registry)>const& spawn_function) {
    //return spawn_function(std::ref(active_registry));
};


entt::registry& SceneManager::get_scene(std::string scene_name) {
	return scene_map.at(scene_name); // Obviously need to guard against invalid and duplicate scene names.
};

std::vector<std::string> SceneManager::get_scene_names() {
	std::vector<std::string>scene_names;
	for (const auto& [key, value] : scene_map) {
		scene_names.push_back(key);
	}

	return scene_names;
};

};
