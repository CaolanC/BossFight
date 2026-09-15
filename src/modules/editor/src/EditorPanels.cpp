#include <imgui.h>
#include <imgui_internal.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <cstring>

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <Client.hpp>
#include <Editor.hpp>
#include <SerializedObject.hpp>
#include <LoadedModelInfo.hpp>
#include <EditorPanels.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>

namespace gui {

    // Draws the "tools" panel in the GUI.
    // Is in control of hosting, joining and saving + quitting the application.
    // Uses enums inside EditorApp to tell the state of the client, presents different buttons
    // depending on the state

    void EditorPanels::draw_tools(AppContext& app) {


        ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_NoTitleBar);

        ImGui::Text("Scene");
		//app.runtime.create_scene("New Scene");
		std::vector<std::string> scene_names = app.runtime.get_scene_names();
		for(std::string name : scene_names) {
			ImGui::Text(name.c_str());
		}

        ImGui::End();
    }


    // Draws the "viewport" of the GUI. It's what lets you see the actual client in the center.
    // Also handles client updates.
    void EditorPanels::draw_viewport(AppContext& app) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoTitleBar);

        // app.client.process_network_messages();

        // bool ok = app.client.start_host_blank(std::string(app.host_ip_input) + ":30000", app.session_port);
        // if (!app.client.is_scene_ready()) {
        //     ImGui::Dummy(ImVec2(0.0f, 40.0f));
        //     ImGui::TextWrapped("No active scene yet.");
        //     ImGui::Spacing();
        //     ImGui::TextWrapped("Choose one of the options in the Tools panel:");
        //     ImGui::BulletText("Host Session -> New Scene");
        //     ImGui::BulletText("Host Session -> Load From File");
        //     ImGui::BulletText("Join Session -> enter port and join");

        //     ImGui::End();
        //     ImGui::PopStyleVar();
        //     return;
        //}

        ImVec2 size = ImGui::GetContentRegionAvail();

        int w = (int)size.x;
        int h = (int)size.y;

        if (w > 0 && h > 0) {
            //app.client.update();
            //app.client.render_to_texture(w, h);

            //GLuint tex = app.client.get_render_texture();
            app.runtime.update();
			app.runtime.render_to_texture(w, h);
			GLuint tex = app.runtime.get_render_texture();

            ImGui::Image((ImTextureID)(intptr_t)tex, size, ImVec2(0,1), ImVec2(1,0));
            

        }

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) {
            app.client.set_input_mode(client::InputMode::Client);
            SDL_SetWindowMouseGrab(app.window, true);
            SDL_CaptureMouse(true);
            SDL_SetWindowRelativeMouseMode(app.window, true);
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }


    // Draws the right panel. This panel shows a list of objects on the scene.
    // Enables editing when an object is selected - position, rotation, scale, name, etc.
    void EditorPanels::draw_right(AppContext& app) {
        ImGui::Begin("RightPanel", nullptr, ImGuiWindowFlags_NoTitleBar);

        if (ImGui::Button("Demo Window")) {
            app.show_demo_window = !app.show_demo_window;
        }
        ImGui::Text("Entities");
        ImGui::Separator();

        if (ImGui::Button("Create Entity")) {
            auto id = app.client.active_registry.create();
            //app.client.active_registry.emplace<components::entity_name>(id, "Entity");
        }

       // if (ImGui::Button("Load Test Model")) {
       //     std::cout << app.client.add_test_model() << '\n';
       // }

        if (ImGui::Button("Load GLTF ModelTree.")) {
            auto m_tree = app.client.resource_manager.load_model("models/sink/scene.gltf");
	    	app.client.entity_factory.from_model_tree(app.client.active_registry, m_tree);
        }


        if (ImGui::Button("Load GLTF ModelTree Runtime.")) {
        	auto m_tree = app.runtime.resource_manager.load_model("models/sink/scene.gltf");
	    	app.runtime.entity_factory.from_model_tree(app.runtime.active_scene, m_tree);
        }


    if (app.selected_entity.has_value()) {
        ImGui::Text("Selected Entity: %u", static_cast<uint32_t>(app.selected_entity.value()));
    }

    if (ImGui::BeginMenu("Components")) {
        ImGui::MenuItem("Click me");
        ImGui::MenuItem("No click me");
        ImGui::EndMenu();
    }

    //const auto& view = app.client.active_registry.view<entt::entity>();
    
    auto& active_scene = app.runtime.active_scene;
    const auto& view = active_scene.view<entt::entity>();

    if (ImGui::BeginChild("Entity List", ImVec2(0, 300), true, ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
        for (auto entity : view) {
            // Build node flags
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow 
                                    | ImGuiTreeNodeFlags_SpanAvailWidth
                                    | ImGuiTreeNodeFlags_Leaf;

            if (app.selected_entity.has_value() && app.selected_entity.value() == entity) {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            // Render Tree Node
            bool isOpen = ImGui::TreeNodeEx((void*)(uintptr_t)entity, flags, "Entity %u", static_cast<uint32_t>(entity));

            // Selection logic on left-click
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
                app.selected_entity = entity;
            }

            // -------------------------------------------------------------
            // RIGHT-CLICK CONTEXT MENU
            // -------------------------------------------------------------
            if (ImGui::BeginPopupContextItem()) {
                // Automatically select the right-clicked entity if it wasn't selected
                app.selected_entity = entity;

                ImGui::Text("Entity Operations");
                ImGui::Separator();

                // 1. Add Component Sub-menu
                if (ImGui::BeginMenu("Add Component")) {
                    if (ImGui::MenuItem("Transform")) {
                        // Example EnTT emplace:
                        active_scene.emplace_or_replace<shared::component::position>(entity, glm::vec3(0, 0, 0));
                        active_scene.emplace_or_replace<shared::component::rotation>(entity, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
                        active_scene.emplace_or_replace<component::scale>(entity);
                        active_scene.emplace_or_replace<shared::component::transform>(entity);
                    }
                    if (ImGui::MenuItem("Model")) {
                        app.client.active_registry.emplace_or_replace<component::model_ref>(entity, app.selected_model_ref);
                    }
                    if (ImGui::MenuItem("Material")) {
                        app.client.active_registry.emplace_or_replace<component::mat_ref>(entity, app.selected_material_ref);
                    }
                    if (ImGui::MenuItem("Debug Spin")) {
                        app.client.active_registry.emplace_or_replace<component::debug_spin>(entity);
                    }
                    if (ImGui::MenuItem("Point Light")) {
                        app.client.active_registry.emplace_or_replace<component::basic_light>(entity);
                    }
                    ImGui::EndMenu();
                }

                ImGui::Separator();

                // 2. Duplicate Entity
                if (ImGui::MenuItem("Duplicate Entity")) {
                    // Handle entity duplication logic here
                }

                // 3. Delete Entity
                if (ImGui::MenuItem("Delete Entity")) {
                    app.client.active_registry.destroy(entity);
                    if (app.selected_entity == entity) {
                        app.selected_entity.reset();
                    }
                }

                ImGui::EndPopup();
            }

            // Tree Pop if open
            if (isOpen) {
                if (active_scene.all_of<shared::component::transform>(entity)) {
                    ImGui::TreeNodeEx("Comp_Transform", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Transform");
                    auto& pos = active_scene.get<shared::component::position>(entity);
                    float v[3] = {pos.value[0], pos.value[1], pos.value[2]};
                    ImGui::DragFloat3("Position", v, 0.05f);
                    
                    auto& rot = active_scene.get<shared::component::rotation>(entity);

                    glm::vec3 euler_deg = glm::degrees(glm::eulerAngles(rot));

                    float u_rot[3] = {
                        euler_deg.x,
                        euler_deg.y,
                        euler_deg.z
                    };

                    ImGui::DragFloat3("Rotation", u_rot, 1.0f);

                    auto& scale = active_scene.get<component::scale>(entity);
                    float s = scale.s;
                    ImGui::DragFloat("Scale", &s, 0.05f, 0.01f, 100.0f);


                    pos.value = glm::vec3(v[0], v[1], v[2]);

                    glm::vec3 euler_rad = glm::radians(glm::vec3(u_rot[0], u_rot[1], u_rot[2]));
                    rot = glm::quat(euler_rad);
                    scale.s = s;
                }
                if (active_scene.all_of<component::basic_light>(entity)) {
                    ImGui::TreeNodeEx("Comp_Light", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Light");

                    auto& light = active_scene.get<component::basic_light>(entity);

                    float u_col[4] = {
			light.color[0],
			light.color[1],
			light.color[2],
			light.color[3]
                    };

		    ImGui::DragFloat4("Color", u_col, 0.01f);

		    light.color = glm::vec4(u_col[0], u_col[1], u_col[2], u_col[3]);

                }
                if (active_scene.all_of<component::mat_ref>(entity)) {
                    ImGui::TreeNodeEx("Comp_Material", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Material");
                }
                if (active_scene.all_of<component::debug_spin>(entity)) {
                    ImGui::TreeNodeEx("Comp_DebugSpin", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Debug Spin");
                }
                ImGui::TreePop();
            }
        }
    ImGui::EndChild();
    }

    
        // Logic for editor tab below. Shows name, position, rotation and scale fields.
        // Rotation is symbolized by pitch, yaw and roll.
        // Pitch = up and down, yaw = left and right, roll = front and back

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Selected Object");
        ImGui::Spacing();


        ImGui::Spacing();
        ImGui::Text("Transform");

        bool changed = false;

        ImGui::End();
    }

    // Draws the bottom panel.
    // Has two tabs. One tab shows a list of loaded models, the other allows you to import models.
    void EditorPanels::draw_bottom(AppContext& app) {
        ImGui::Begin("BottomPanel", nullptr, ImGuiWindowFlags_NoTitleBar);

        if (ImGui::BeginTabBar("BottomTabs")) {

            if (ImGui::BeginTabItem("Help")) {
                ImGui::Text("Controls");
                ImGui::Separator();
                ImGui::BulletText("Click inside the viewport to control the camera.");
                ImGui::BulletText("W/A/S/D: move camera.");
                ImGui::BulletText("Mouse movement: look around.");
                ImGui::BulletText("Press escape to return control to the editor.");
                ImGui::BulletText("Imported models must use paths relative to res/assets.");
                ImGui::BulletText("Example: models/office_chair_gltf/scene.gltf");
                ImGui::BulletText("For loading a save file, must include .json.");
                ImGui::BulletText("For saving a file, just enter in name. Program will add .json extension");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
    }

}
