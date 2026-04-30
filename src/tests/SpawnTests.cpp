#include <gtest/gtest.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <spawn/Spawn.hpp>
#include <systems/Transform.hpp>
#include <component/Core.hpp>
#include <SharedComponents.hpp>

TEST(SpawnTests, ModelSpawnCreatesEntityWithObjectId) {
    entt::registry registry;

    auto model_ref = xg::newGuid();
    auto mat_ref = xg::newGuid();

    auto entity = spawn::model(registry, model_ref, mat_ref, "models/chair/scene.gltf",
                               glm::vec3(1, 2, 3), 2.0f,
                               glm::quat(1, 0, 0, 0),
                               "obj-1", "Object");

    ASSERT_TRUE(registry.valid(entity));
    EXPECT_EQ(registry.get<component::object_id>(entity).value, "obj-1");
}

TEST(SpawnTests, ModelSpawnUsesDefaultNameWhenEmpty) {
    entt::registry registry;

    auto entity = spawn::model(registry, xg::newGuid(), xg::newGuid());

    EXPECT_EQ(registry.get<component::name>(entity).value, "Object");
}