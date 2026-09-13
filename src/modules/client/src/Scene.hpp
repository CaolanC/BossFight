#pragma once

#include <entt/entt.hpp>
#include <rendering/EntityFactory.hpp>

namespace client {

class Scene {
	public:
	Scene();
	entt::registry& get_registry();
	void ent_from_mtree();
	
	private:
	entt::entity root_entity;
	entt::registry registry;
	rendering::EntityFactory entity_factory;
};

};
