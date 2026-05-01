#include <gtest/gtest.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <spawn/Spawn.hpp>
#include <systems/Transform.hpp>
#include <component/Core.hpp>
#include <SharedComponents.hpp>

#include <TestHelpers.hpp>

// Tests notable tests in the spawn class. More details in docs

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

TEST(SpawnTests, EnttGetsSerializedObjValues) {
    entt::registry registry;

    auto obj = make_test_object();
    auto model_ref = xg::newGuid();
    auto mat_ref = xg::newGuid();

    auto entity = spawn::model(registry, model_ref, mat_ref, obj.model_path, obj.position, obj.scale, obj.rotation, obj.objectID, obj.name);

    EXPECT_EQ(registry.get<component::object_id>(entity).value, "obj-1");
    EXPECT_EQ(registry.get<component::name>(entity).value, "Object");
    EXPECT_EQ(registry.get<component::model_ref>(entity).id, model_ref);
    EXPECT_EQ(registry.get<component::model_path>(entity).value, obj.model_path);
    EXPECT_EQ(registry.get<shared::component::position>(entity).value, obj.position);
    EXPECT_EQ(registry.get<shared::component::rotation>(entity), obj.rotation);
    EXPECT_EQ(registry.get<component::scale>(entity).s, obj.scale);
}

TEST(SpawnTests, FreecamCreatesUserCamera) {
    entt::registry registry;

    auto e = spawn::freecam(registry);

    EXPECT_TRUE(registry.all_of<component::camera>(e));
    EXPECT_TRUE(registry.all_of<component::user_control>(e));
    EXPECT_TRUE(registry.all_of<shared::component::position>(e));
    EXPECT_TRUE(registry.all_of<shared::component::rotation>(e));
    EXPECT_TRUE(registry.all_of<shared::component::transform>(e));
    EXPECT_TRUE(registry.all_of<component::scale>(e));
}