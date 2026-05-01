#include <gtest/gtest.h>

#include <entt/entt.hpp>
#include <systems/Transform.hpp>
#include <component/Core.hpp>
#include <SharedComponents.hpp>
#include <TestHelpers.hpp>

// Tests methods related to transforms. More details in docs

TEST(TransformSystemTests, AppliesPositionToTransformMatrix) {
    entt::registry r;
    auto e = make_transform_entity(r, glm::vec3(3.0f, 4.0f, 5.0f), 1.0f);

    systems::Transform(r);

    auto t = r.get<shared::component::transform>(e);

    EXPECT_FLOAT_EQ(t[3][0], 3.0f);
    EXPECT_FLOAT_EQ(t[3][1], 4.0f);
    EXPECT_FLOAT_EQ(t[3][2], 5.0f);
}

TEST(TransformSystemTests, AppliesScaleToTransformMatrix) {
    entt::registry r;
    auto e = make_transform_entity(r, glm::vec3(0.0f), 3.0f);

    systems::Transform(r);

    auto t = r.get<shared::component::transform>(e);

    EXPECT_FLOAT_EQ(t[0][0], 3.0f);
    EXPECT_FLOAT_EQ(t[1][1], 3.0f);
    EXPECT_FLOAT_EQ(t[2][2], 3.0f);
}

TEST(TransformSystemTests, ReRunningTransformUsesUpdatedPosition) {
    entt::registry r;
    auto e = make_transform_entity(r, glm::vec3(1.0f), 1.0f);

    systems::Transform(r);

    r.get<shared::component::position>(e).value = glm::vec3(9.0f, 8.0f, 7.0f);

    systems::Transform(r);

    auto t = r.get<shared::component::transform>(e);

    EXPECT_FLOAT_EQ(t[3][0], 9.0f);
    EXPECT_FLOAT_EQ(t[3][1], 8.0f);
    EXPECT_FLOAT_EQ(t[3][2], 7.0f);
}