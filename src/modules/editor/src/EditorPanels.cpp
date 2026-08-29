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

        if (!app.client.is_scene_ready()) {
            ImGui::Text("Session");
            ImGui::Separator();

            if (ImGui::Button("Host Session", ImVec2(-1, 30))) {
                app.flow_mode = AppContext::SessionFlowMode::Host;
                app.host_scene_mode = AppContext::HostSceneMode::None;
                app.status_text = "Hosting session";
            }
            if (ImGui::Button("Join Session", ImVec2(-1, 30))){
                app.flow_mode = AppContext::SessionFlowMode::Join;
                app.host_scene_mode = AppContext::HostSceneMode::None;
                app.status_text = "Joining session";
            }

            ImGui::Separator();

            // Host flow

            // if (app.flow_mode == AppContext::SessionFlowMode::Host) {
            //     ImGui::Text("Host Session Setup");
            //     ImGui::Spacing();

            //     ImGui::InputText("##host_ip_input", app.host_ip_input, sizeof(app.host_ip_input));

            //     if (ImGui::Button("New Scene", ImVec2(-1, 28))) {
            //         app.host_scene_mode = AppContext::HostSceneMode::Blank;
            //         app.status_text = "Blank scene selected";
            //     }

            //     if (ImGui::Button("Load From File", ImVec2(-1, 28))) {
            //         app.host_scene_mode = AppContext::HostSceneMode::FromFile;
            //         app.status_text = "Load-from-file selected";
            //     }

            //     ImGui::Spacing();

            //     if (app.host_scene_mode == AppContext::HostSceneMode::Blank) {
            //         ImGui::TextWrapped("A blank scene will be created.");

            //         if (ImGui::Button("Start Host Session", ImVec2(-1, 30))) {
            //             bool ok = app.client.start_host_blank(std::string(app.host_ip_input) + ":30000", app.session_port);
            //             if (ok) {
            //                 app.status_text = "Host session started (blank scene)";
            //                 app.session_info = std::string(app.host_ip_input);
            //             }
            //             else {
            //                 app.status_text = "Host session started failed";
            //             }
            //         }

            //     }
            //     else if (app.host_scene_mode == AppContext::HostSceneMode::FromFile) {
            //         ImGui::Text("Scene file");
            //         ImGui::InputText("##scene_file", app.file_input, sizeof(app.file_input));

            //         if (ImGui::Button("Start Host Session", ImVec2(-1, 30))) {
            //             bool ok = app.client.start_host_file(std::string(app.host_ip_input) + ":30000", std::string(app.file_input), app.session_port);
            //             if (ok) {
            //                 app.status_text = std::string("Host session started from file: ") + app.file_input;
            //                 app.session_info = std::string(app.host_ip_input);
            //             }
            //             else {
            //                 app.status_text = "Failed to start host session";
            //             }
            //         }


            //     }
            // }

            // Joining

            // else if (app.flow_mode == AppContext::SessionFlowMode::Join) {
            //     ImGui::Text("Join Session");
            //     ImGui::Spacing();

            //     ImGui::Text("Server IP");
            //     ImGui::InputText("##ip", app.guest_ip_input, sizeof(app.guest_ip_input));

            //     ImGui::Text("Port");
            //     ImGui::InputText("##port", app.port_input, sizeof(app.port_input));

            //     if (ImGui::Button("Join", ImVec2(-1, 30))) {
            //         app.client.setIsHost(false);
            //         int port = std::atoi(app.port_input);
            //         bool ok = app.client.start_guest(std::string(app.guest_ip_input), port);
            //         if (ok) {
            //             app.status_text = std::string("Joining session at ") + app.guest_ip_input + ":" + app.port_input;
            //             app.session_info = std::string(app.guest_ip_input);
            //             app.session_port = port;
            //         }
            //         else {
            //             app.status_text = "Failed to join session.";
            //         }
            //     }
            // }

        }

        // Once succeed, get rid of buttons and show if client is a host or guest respectively.
        // Also, display save and quit for host

        else if (app.client.is_scene_ready() && app.client.getIsHost()) {
            // ImGui::Text("Host Client");
            // ImGui::Separator();

            // ImGui::Text("Enter save file name below:");
            // ImGui::InputText("##save_file", app.savetofile_input, sizeof(app.savetofile_input));
            // if (ImGui::Button("Save and Quit", ImVec2(-1, 30))) {
            //     if (app.client.save_and_quit(app.savetofile_input)) {
            //         app.done = true;
            //     }
            //     else {
            //         app.status_text = "Failed to save to file, client still open.";
            //     }
            // }
        }
        else if (app.client.is_scene_ready() && !(app.client.getIsHost())) {
            ImGui::Text("Guest Client");
        }

        // Small info tab

        ImGui::Separator();
        ImGui::Text("Info");
        ImGui::Separator();
        ImGui::TextWrapped("Status: %s", app.status_text.c_str());
        ImGui::TextWrapped("Session IP: %s", app.session_info.c_str());
        ImGui::TextWrapped("Session Port: %d", app.session_port);

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
            app.client.update();
            app.client.render_to_texture(w, h);

            GLuint tex = app.client.get_render_texture();

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

        if (ImGui::Button("Load Test Model")) {
            std::cout << app.client.add_test_model() << '\n';
        }

        if (ImGui::Button("Load GLTF ModelTree.")) {
            auto m_tree = app.client.resource_manager.load_model("models/sink/scene.gltf");
	    app.client.entity_factory.from_model_tree(app.client.active_registry, m_tree);
        }


    if (app.selected_entity.has_value()) {
        ImGui::Text("Selected Entity: %u", static_cast<uint32_t>(app.selected_entity.value()));
    }

    if (ImGui::BeginMenu("Components")) {
        ImGui::MenuItem("Click me");
        ImGui::MenuItem("No click me");
        ImGui::EndMenu();
    }

    const auto& view = app.client.active_registry.view<entt::entity>();


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
                        app.client.active_registry.emplace_or_replace<shared::component::position>(entity, glm::vec3(0, 0, 0));
                        app.client.active_registry.emplace_or_replace<shared::component::rotation>(entity, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
                        app.client.active_registry.emplace_or_replace<component::scale>(entity);
                        app.client.active_registry.emplace_or_replace<shared::component::transform>(entity);
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
                if (app.client.active_registry.all_of<shared::component::transform>(entity)) {
                    ImGui::TreeNodeEx("Comp_Transform", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Transform");
                    auto& pos = app.client.active_registry.get<shared::component::position>(entity);
                    float v[3] = {pos.value[0], pos.value[1], pos.value[2]};
                    ImGui::DragFloat3("Position", v, 0.05f);
                    \
                    auto& rot = app.client.active_registry.get<shared::component::rotation>(entity);

                    glm::vec3 euler_deg = glm::degrees(glm::eulerAngles(rot));

                    float u_rot[3] = {
                        euler_deg.x,
                        euler_deg.y,
                        euler_deg.z
                    };

                    ImGui::DragFloat3("Rotation", u_rot, 1.0f);

                    auto& scale = app.client.active_registry.get<component::scale>(entity);
                    float s = scale.s;
                    ImGui::DragFloat("Scale", &s, 0.05f, 0.01f, 100.0f);


                    pos.value = glm::vec3(v[0], v[1], v[2]);

                    glm::vec3 euler_rad = glm::radians(glm::vec3(u_rot[0], u_rot[1], u_rot[2]));
                    rot = glm::quat(euler_rad);
                    scale.s = s;
                }
                if (app.client.active_registry.all_of<component::model_ref>(entity)) {
                    ImGui::TreeNodeEx("Comp_Model", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Model");
                }
                if (app.client.active_registry.all_of<component::mat_ref>(entity)) {
                    ImGui::TreeNodeEx("Comp_Material", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Material");
                }
                if (app.client.active_registry.all_of<component::debug_spin>(entity)) {
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

        // if (app.selected_entity.has_value()) {
        //     ImGui::TextWrapped("Select an object from the list above.");
        //     ImGui::End();
        //     return;
        // }

        // if (!(app.selected_object_id.empty()) && !(app.client.scene.check_registry(app.selected_object.objectID))) {
        //     app.selected_object_id = "";
        //     app.selected_object = core::SerializedObject();
        // }

        ImGui::TextWrapped("Object ID: %s", app.selected_object.objectID.c_str());
        ImGui::TextWrapped("Model Path: %s", app.selected_object.model_path.c_str());

        char name[128];
        std::strncpy(name, app.selected_object.name.c_str(), 128);
        name[sizeof(name) - 1] = '\0';

        if (ImGui::InputText("Name", name, sizeof(name))) {
            app.selected_object.name = name;
        }

        float pos[3] = {
            app.selected_object.position.x,
            app.selected_object.position.y,
            app.selected_object.position.z
        };

        glm::vec3 euler_deg = glm::degrees(glm::eulerAngles(app.selected_object.rotation));

        float rot[3] = {
            euler_deg.x,
            euler_deg.y,
            euler_deg.z
        };

        float scale = app.selected_object.scale;

        ImGui::Spacing();
        ImGui::Text("Transform");

        bool changed = false;
        changed |= ImGui::DragFloat3("Position", pos, 0.05f);
        ImGui::Text("Rotation (Pitch/Yaw/Roll)");
        changed |= ImGui::DragFloat3("Rotation", rot, 1.0f);
        changed |= ImGui::DragFloat("Scale", &scale, 0.05f, 0.01f, 100.0f);

        if (changed) {
            app.selected_object.position = glm::vec3(pos[0], pos[1], pos[2]);
            glm::vec3 euler_rad = glm::radians(glm::vec3(rot[0], rot[1], rot[2]));
            app.selected_object.rotation = glm::quat(euler_rad);
            app.selected_object.scale = scale;
        }

        if (ImGui::Button("Apply Edit", ImVec2(-1, 30))) {
            // bool ok = app.client.apply_gui_edit(app.selected_object);
            // if (ok) {
            //     app.status_text = "Object edited successfully";
            // }
            // else {
            //     app.status_text = "Failed to edit object";
            // }

            // if (ok) {
            //     core::SerializedObject latest;
            //     if (app.client.get_scene_object(app.selected_object_id, latest)) {
            //         app.selected_object = latest;
            //     }
            // }
        }

        if (ImGui::Button("Delete Object", ImVec2(-1, 30))) {
            // bool ok = app.client.apply_gui_delete(app.selected_object);
            // if (ok) {
            //     app.status_text = "Object deleted successfully";
            // }
            // else {
            //     app.status_text = "Failed to delete object";
            // }

            // if (ok) {
            //     app.selected_object = core::SerializedObject();
            //     app.selected_object_id = "";
            // }
        }

        ImGui::End();
    }

    // Draws the bottom panel.
    // Has two tabs. One tab shows a list of loaded models, the other allows you to import models.
    void EditorPanels::draw_bottom(AppContext& app) {
        ImGui::Begin("BottomPanel", nullptr, ImGuiWindowFlags_NoTitleBar);

        if (ImGui::BeginTabBar("BottomTabs")) {

            if (ImGui::BeginTabItem("Loaded Models")) {

                auto loaded_models = app.client.model_manager.get_loaded_models();

                if (loaded_models.empty()) {
                    ImGui::TextWrapped("No loaded models.");
                } else {
                    for (const auto& model : loaded_models) {
                        std::string label = model.model_path;

                        bool selected = (app.selected_model_ref == model.model_ref);

                        if (ImGui::Selectable(label.c_str(), selected)) {
                            app.selected_model_ref = model.model_ref;
                            app.selected_model_path = model.model_path;
                        }
                    }

                    ImGui::Spacing();
                    ImGui::Separator();

                    if (app.selected_model_ref.isValid()) {
                        ImGui::TextWrapped(
                            "Selected model: %s",
                            app.selected_model_path.empty()
                                ? app.selected_model_ref.str().c_str()
                                : app.selected_model_path.c_str()
                        );

                        ImGui::InputText("##objectname", app.objectname, sizeof(app.objectname));

                        if (ImGui::Button("Add Object From Selected Model", ImVec2(-1, 30))) {
                            core::LoadedModelInfo info;
                            info.model_ref = app.selected_model_ref;
                            info.model_path = app.selected_model_path;

                            // bool ok = app.client.add_object_from_loaded_model(info, app.objectname);
                            // if (ok) {
                            //     app.status_text = "Object added from selected model";
                            // }
                            // else {
                            //     app.status_text = "Failed to add object from selected model";
                            // }
                        }
                    }
                }
                
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Loaded Materials")) {
                auto loaded_materials = app.client.material_manager.get_loaded_materials();

                if (loaded_materials.empty()) {
                    ImGui::TextWrapped("No loaded models.");
                } else {
                    for (const auto& material : loaded_materials) {
                        std::string label = material.ref;

                        bool selected = (app.selected_material_ref == material.ref);

                        if (ImGui::Selectable(label.c_str(), selected)) {
                            app.selected_material_ref = material.ref;
                        }
                    }

                    ImGui::Spacing();
                    ImGui::Separator();

                    if (app.selected_model_ref.isValid()) {
                        ImGui::TextWrapped(
                            "Selected model: %s",
                            app.selected_model_path.empty()
                                ? app.selected_model_ref.str().c_str()
                                : app.selected_model_path.c_str()
                        );

                        ImGui::InputText("##objectname", app.objectname, sizeof(app.objectname));

                        if (ImGui::Button("Add Object From Selected Model", ImVec2(-1, 30))) {
                            core::LoadedModelInfo info;
                            info.model_ref = app.selected_model_ref;
                            info.model_path = app.selected_model_path;

                            // bool ok = app.client.add_object_from_loaded_model(info, app.objectname);
                            // if (ok) {
                            //     app.status_text = "Object added from selected model";
                            // }
                            // else {
                            //     app.status_text = "Failed to add object from selected model";
                            // }
                        }
                    }
                }
                
                ImGui::EndTabItem();
            }
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
