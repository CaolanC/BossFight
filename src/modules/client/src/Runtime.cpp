#include <Runtime.hpp>
#include <systems/Transform.hpp>
#include <systems/Debug.hpp>

namespace runtime {

Runtime::Runtime() {
	systems.push_back(systems::Transform);
	systems.push_back(systems::Debug);
};

void Runtime::update() {
	for(auto& system : systems) {
		system(active_scene);
	}
};

entt::entity Runtime::create_entity() {
	return active_scene.create();
};

entt::registry& Runtime::get_scene_registry() {
	return active_scene;
};

const std::vector<std::string> Runtime::get_scene_names() {

};

};
