#pragma once

#include <entt/entt.hpp>
#include <rendering/ResourceManager.hpp>
#include <rendering/EntityFactory.hpp>
#include <SceneManager.hpp>
#include <Renderer.hpp>

#include <vector>

namespace runtime {

class Runtime {
	public:
	Runtime();
	entt::entity create_entity();
	entt::registry& get_scene_registry();
	const std::vector<std::string> get_scene_names();
	void create_scene(const std::string);
	rendering::EntityFactory entity_factory; // Only public for now, this stuff should happen internally soon.	
	SceneManager scene_manager; // This too.
	rendering::ResourceManager resource_manager = rendering::ResourceManager(); // This too.
	entt::registry& active_scene = scene_manager.get_scene("Default Scene"); // This too.

	private:
	client::Renderer renderer = client::Renderer(resource_manager);
	std::vector<void (*)(entt::registry&)> systems;

	void update();
};

#ifdef RUNTIME_EDITOR_API


// Scene API
switch_scene();
create_scene();
delete_scene();
run_scene();
// Maybe potentially snapshot scene we need to think about how we're handling running the runtime in the editor.

// Entity API
create_entity();
delete_entity();
add_component(e, component);


// Editor Calls for Functionality

get_entities(); // For our entity list; We potentially just want to hand back a const reference to the registry so we can do our component things too.
// Need to account for if the scene switches then.
get_scenes(); // For our scene list and switcher



#endif

};
