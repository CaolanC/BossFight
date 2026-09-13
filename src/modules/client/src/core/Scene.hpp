#pragma once

#include <core/Object.hpp>
#include <core/cameras/CameraBase.h>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <entt/entt.hpp>
#include <component/Core.hpp>
#include <spawn/Spawn.hpp>
#include <systems/Render.hpp>
#include <core/MeshManager.hpp>
#include <core/sh_src.hpp>
#include <SceneSnapshot.hpp>
#include <SerializedObject.hpp>
#include <SceneSerializer.hpp>
#include <LoadedModelInfo.hpp>

#include <core/ShaderProgramManager.hpp>

#include <systems/GatherUserInput.hpp>
#include <systems/UserControl.hpp>
#include <systems/Transform.hpp>
#include <tuple>
#include <generator/GridPlane.hpp>

#include <systems/Init.hpp>
#include <core/ModelManager.hpp>
#include <systems/MeshLoading.hpp>
#include <rendering/Model.hpp>

#include <systems/Debug.hpp>

namespace core
{

class Scene
{
// public:

//      Scene(core::MeshManager const& manager, core::ModelManager const& model_manager) :
//             mesh_manager(manager),
//             model_manager(model_manager)
//      {
//     }

//     // Bootstrap functions - spawn global EnTT registry context
//     // These store things such as model manager and keyboard/mouse state
//     // If bootstrapping from file, also initialize file into scene
//     // Also, spawn the base platform in scene for both
//     void bootstrap() {
//          int no_keys;
//          const bool* k_state = SDL_GetKeyboardState(&no_keys);
//          registry.ctx().emplace<component::keyboard_state>();
//          registry.ctx().emplace<component::mouse_state>(0.0f, 0.0f);
//          registry.ctx().emplace<component::current_camera>(spawn_default_camera());
//          registry.ctx().emplace<component::mesh_manager>(mesh_manager);
//          registry.ctx().emplace<component::material_manager>(core::ShaderProgramManager());
//          registry.ctx().emplace<component::model_manager>(model_manager);
//          systems::Init_spawn_platform(registry);
//      }

//     bool bootstrap_from_file(const std::string& file_path) {
//          int no_keys;
//          const bool* k_state = SDL_GetKeyboardState(&no_keys);
//          registry.ctx().emplace<component::keyboard_state>();
//          registry.ctx().emplace<component::mouse_state>(0.0f, 0.0f);
//          registry.ctx().emplace<component::current_camera>(spawn_default_camera());
//          registry.ctx().emplace<component::mesh_manager>(mesh_manager);
//          registry.ctx().emplace<component::material_manager>(core::ShaderProgramManager());
//          registry.ctx().emplace<component::model_manager>(model_manager);
//          systems::Init_spawn_platform(registry);
//          SceneSerializer sceneserializer = SceneSerializer();

//          bool initialized = systems::Init_from_file(registry, utils::assets::get_filepath(file_path), sceneserializer, initial_snapshot, object_lookup);
//          if (initialized) {
//              initial_snapshot.debug_print();
//              return true;
//          }
//          else {
//              return false;
//          }
//      }

//     entt::entity spawn_default_camera() {
//          return spawn(spawn::freecam);
//     }

//     entt::entity spawn(std::function<entt::entity(entt::registry& registry)>const& spawn_function) {
//          return spawn_function(std::ref(registry));
//     }

//     void set_camera_position(glm::vec3 position) {
//          auto &pos = registry.get<shared::component::position>(registry.ctx().get<component::current_camera>().e);
//          pos.value = position;
//     }

//     // Snapshot builder (mainly for writing to file use)
//     const SceneSnapshot build_snapshot() {
//          SceneSnapshot snapshot;

//          auto view = registry.view<
//              component::object_id,
//              component::model_path,
//              component::name,
//              component::model_ref,
//              shared::component::position,
//              shared::component::rotation,
//              component::scale
//          >();

//          for (auto e : view) {
//              const auto& id   = view.get<component::object_id>(e);
//              const auto& ref = view.get<component::model_ref>(e);
//              const auto& objname = view.get<component::name>(e);
//              const auto& path = view.get<component::model_path>(e);
//              const auto& pos  = view.get<shared::component::position>(e);
//              const auto& rot  = view.get<shared::component::rotation>(e);
//              const auto& scl  = view.get<component::scale>(e);

//              core::SerializedObject obj;
//              obj.objectID = id.value;
//              obj.model_ref = ref.id;
//              obj.name = objname.value;
//              obj.model_path = path.value;
//              obj.position = pos.value;
//              obj.rotation = rot;
//              obj.scale = scl.s;

//              snapshot.insert(obj.objectID, obj);

//          }

//          return snapshot;
//      }

//     const SceneSnapshot get_initial_snapshot() {
//          return initial_snapshot;
//      }

//     // Functions called from client's netclient msg handler (a.k.a helpers for websockets)

//     // Initialize guest client (clients that join)
//     // We do not need local checks for guest initialization as any assets not locally found are put
//     // into the scene's deferred update list
//     void guest_init(SceneSnapshot& snapshot) {
//          systems::Init_from_snapshot(registry, snapshot, object_lookup);
//     }

//     // Add object to scene
//     bool add_obj(SerializedObject& obj) {
//          auto& model_m = registry.ctx().get<component::model_manager>().manager;
//          std::vector<core::ShaderSource> shader_sources = {
//              core::sh_src::v3D(),
//              core::sh_src::fSolid()
//          };
//          ShaderProgramHandle program = registry.ctx().get<component::material_manager>().manager.from_source_vec(shader_sources);

//          if (!(model_m.check_ref(obj.model_ref))) {
//              if (model_m.has_model_path(obj.model_path)) {
//                  obj.model_ref = model_m.get_model_ref_from_path(obj.model_path);
//              }
//              else {
//                  // Put loading models in a try -> catch error in an attempt to catch failed model loads
//                  // This is unrealistic to happen and is more of a just in case measure
//                  // Even more unrealistic in add_obj because it usually suggests that the model was added
//                  // successfully on another client - but this is just to prevent crashes.
//                  try {
//                      rendering::Model m = systems::LoadModel(registry, obj.model_path);
//                      model_m.add_model(m, obj.model_path, obj.model_ref);
//                  } catch (const std::exception& e) {
//                      std::cout << "Failed to load model: " << e.what() << "\n";
//                      return false;
//                  }
//              }
//          }

//          auto model_e = spawn::model(registry, obj.model_ref, program, obj.model_path, obj.position, obj.scale, obj.rotation, obj.objectID, obj.name);
//          object_lookup[obj.objectID] = model_e;

//          return true;
//      }

//     // Edit object in scene registry
//     bool edit_obj(SerializedObject& obj) {
//          auto e = registry_lookup(obj.objectID);
//          auto& pos = registry.get<shared::component::position>(e);
//          auto& rot = registry.get<shared::component::rotation>(e);
//          auto& scale = registry.get<component::scale>(e);
//          auto& name = registry.get<component::name>(e);
//          pos.value = obj.position;
//          rot = obj.rotation;
//          scale.s = obj.scale;
//          name.value = obj.name;

//          return true;
//     }

//     // Delete object from scene registry (also delete it frok the registry lookup)
//     bool delete_obj(SerializedObject& obj) {
//          auto e = registry_lookup(obj.objectID);
//          object_lookup.erase(obj.objectID);
//          registry.destroy(e);

//          return true;
//     }

//     // Local, used to load models from filepath
//     bool load_model_from_gui(const std::string& file_path) {
//          auto& model_m = registry.ctx().get<component::model_manager>().manager;

//          if (model_m.has_model_path(file_path)) {
//              return true;
//          }

//          // Put loading models in a try -> catch error in an attempt to catch failed model loads
//          // This is unrealistic to happen and is more of a just in case measure
//          try {
//              rendering::Model m = systems::LoadModel(registry, file_path);
//              model_m.add_model(m, file_path);
//              return true;
//          } catch (const std::exception& e) {
//              std::cout << "Failed to load model: " << e.what() << "\n";
//              return false;
//          }
//      }

//     // ENTT registry lookup methods

//     entt::registry& getRegistry() {
//          return registry;
//      }

//     entt::entity registry_lookup(const std::string& enttid) {
//          return object_lookup.at(enttid);
//     }

//     bool check_registry(const std::string& enttid) {
//          return object_lookup.find(enttid) != object_lookup.end();
//      }

//     // Looks up registry for one specific SerializedObject
//     bool registry_lookup_to_obj(const std::string& enttid, core::SerializedObject& out) const {
//          auto it = object_lookup.find(enttid);
//          if (it == object_lookup.end()) {
//              return false;
//          }

//          entt::entity e = it->second;
//          const auto& id  = registry.get<component::object_id>(e);
//          const auto& ref = registry.get<component::model_ref>(e);
//          const auto& objname = registry.get<component::name>(e);
//          const auto& path = registry.get<component::model_path>(e);
//          const auto& pos = registry.get<shared::component::position>(e);
//          const auto& rot = registry.get<shared::component::rotation>(e);
//          const auto& scale = registry.get<component::scale>(e);

//          out.objectID  = id.value;
//          out.model_ref = ref.id;
//          out.name = objname.value;
//          out.model_path = path.value;
//          out.position = pos.value;
//          out.rotation = rot;
//          out.scale = scale.s;

//          return true;
//      }

//     // Makes a vector of SerializedObjects from the EnTT registry for GUI to display
//     std::vector<core::SerializedObject> get_object_info() const {
//          std::vector<core::SerializedObject> out;
//          out.reserve(object_lookup.size());

//          auto view = registry.view<
//             component::object_id,
//             component::model_ref,
//             component::name,
//             component::model_path,
//             shared::component::position,
//             shared::component::rotation,
//             component::scale
//         >();

//          for (auto e : view) {
//              const auto& id = view.get<component::object_id>(e);
//              const auto& ref = view.get<component::model_ref>(e);
//              const auto& objname = view.get<component::name>(e);
//              const auto& path = view.get<component::model_path>(e);
//              const auto& pos = view.get<shared::component::position>(e);
//              const auto& rot = view.get<shared::component::rotation>(e);
//              const auto& scale = view.get<component::scale>(e);

//              core::SerializedObject obj;
//              obj.objectID  = id.value;
//              obj.model_ref = ref.id;
//              obj.name = objname.value;
//              obj.model_path = path.value;
//              obj.position = pos.value;
//              obj.rotation = rot;
//              obj.scale = scale.s;

//              out.push_back(obj);
//          }

//          return out;
//      }

//     // Gets a list of loaded models from model manager
//     std::vector<core::LoadedModelInfo> get_loaded_models() const {
//          const auto& model_manager = registry.ctx().get<component::model_manager>().manager;
//          return model_manager.get_loaded_models();
//      }

// private:
//     entt::registry registry;
//     core::MeshManager const& mesh_manager;
//     core::ModelManager const& model_manager;
//     unsigned int FBO;
//     SceneSnapshot initial_snapshot;
//     std::unordered_map<std::string, entt::entity> object_lookup;
// };

};
}