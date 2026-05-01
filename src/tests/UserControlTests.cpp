#include <gtest/gtest.h>

#include <entt/entt.hpp>
#include <systems/UserControl.hpp>
#include <component/Core.hpp>
#include <SharedComponents.hpp>
#include <TestHelpers.hpp>

// Tests methods related to user control in systems. More details in docs

TEST(UserControlTests, PressingWMovesEntityForward) {
    entt::registry r;
    add_input_context(r);
    auto e = make_controlled_entity(r);

    r.ctx().get<component::keyboard_state>().down[SDL_SCANCODE_W] = true;

    systems::UserControl(r);

    auto pos = r.get<shared::component::position>(e).value;

    EXPECT_LT(pos.z, 0.0f);
}

TEST(UserControlTests, PressingSMovesEntityBackward) {
    entt::registry r;
    add_input_context(r);
    auto e = make_controlled_entity(r);

    r.ctx().get<component::keyboard_state>().down[SDL_SCANCODE_S] = true;

    systems::UserControl(r);

    auto pos = r.get<shared::component::position>(e).value;

    EXPECT_GT(pos.z, 0.0f);
}

TEST(UserControlTests, PressingAMovesEntityLeft) {
    entt::registry r;
    add_input_context(r);
    auto e = make_controlled_entity(r);

    r.ctx().get<component::keyboard_state>().down[SDL_SCANCODE_A] = true;

    systems::UserControl(r);

    auto pos = r.get<shared::component::position>(e).value;

    EXPECT_LT(pos.x, 0.0f);
}

TEST(UserControlTests, PressingDMovesEntityRight) {
    entt::registry r;
    add_input_context(r);
    auto e = make_controlled_entity(r);

    r.ctx().get<component::keyboard_state>().down[SDL_SCANCODE_D] = true;

    systems::UserControl(r);

    auto pos = r.get<shared::component::position>(e).value;

    EXPECT_GT(pos.x, 0.0f);
}

TEST(UserControlTests, NoInputDoesNotMoveEntity) {
    entt::registry r;
    add_input_context(r);
    auto e = make_controlled_entity(r);

    systems::UserControl(r);

    auto pos = r.get<shared::component::position>(e).value;

    EXPECT_EQ(pos, glm::vec3(0.0f));
}