#include <gtest/gtest.h>

#include <entt/entt.hpp>
#include <systems/UserControl.hpp>
#include <component/Core.hpp>
#include <SharedComponents.hpp>
#include <TestHelpers.hpp>

TEST(UserControlTests, PressingWMovesEntityForward) {
    entt::registry r;
    add_input_context(r);
    auto e = make_controlled_entity(r);

    r.ctx().get<component::keyboard_state>().down[SDL_SCANCODE_W] = true;

    systems::UserControl(r);

    auto pos = r.get<shared::component::position>(e).value;

    EXPECT_LT(pos.z, 0.0f);
}