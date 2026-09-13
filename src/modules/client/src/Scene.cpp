#include <Scene.hpp>
#include <entt/entt.hpp>

namespace client {

Scene::Scene() {
	root_entity = registry.create();
};

entt::registry& Scene::get_registry() {
};

};
