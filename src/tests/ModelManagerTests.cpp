#include <gtest/gtest.h>
// #include <glad/glad.h>
#include <core/ModelManager.hpp>
#include <rendering/Model.hpp>
#include <crossguid/guid.hpp>

TEST(ModelManagerTests, AddModelWithGeneratedRefCanBeFound) {
    core::ModelManager manager;
    rendering::Model model;

    xg::Guid ref = manager.add_model(model);

    EXPECT_TRUE(manager.check_ref(ref));
}

TEST(ModelManagerTests, AddModelWithProvidedRefCanBeFound) {
    core::ModelManager manager;
    rendering::Model model;
    xg::Guid ref = xg::newGuid();

    manager.add_model(model, ref);

    EXPECT_TRUE(manager.check_ref(ref));
}

TEST(ModelManagerTests, AddModelWithPathCanBeFoundByPath) {
    core::ModelManager manager;
    rendering::Model model;
    xg::Guid ref = xg::newGuid();

    manager.add_model(model, "models/chair/scene.gltf", ref);

    EXPECT_TRUE(manager.has_model_path("models/chair/scene.gltf"));
}

TEST(ModelManagerTests, GetModelRefFromPathReturnsProvidedRef) {
    core::ModelManager manager;
    rendering::Model model;
    xg::Guid ref = xg::newGuid();

    manager.add_model(model, "models/chair/scene.gltf", ref);

    EXPECT_EQ(manager.get_model_ref_from_path("models/chair/scene.gltf"), ref);
}

TEST(ModelManagerTests, GetLoadedModelsExcludesModelsWithoutPath) {
    core::ModelManager manager;

    rendering::Model platform_model;
    rendering::Model user_model;

    manager.add_model(platform_model, xg::newGuid());

    xg::Guid ref = xg::newGuid();
    manager.add_model(user_model, "models/chair/scene.gltf", ref);

    auto loaded = manager.get_loaded_models();

    ASSERT_EQ(loaded.size(), 1);
    EXPECT_EQ(loaded[0].model_ref, ref);
    EXPECT_EQ(loaded[0].model_path, "models/chair/scene.gltf");
}