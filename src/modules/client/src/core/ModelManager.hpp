#pragma once

#include <core/defines.hpp>
#include <rendering/Model.hpp>
#include <crossguid/guid.hpp>
#include <vector>
#include <unordered_map>
#include <map>
#include <LoadedModelInfo.hpp>

namespace core {

    class ModelManager {
    public:
        ModelManager() {

        };

        xg::Guid add_model(rendering::Model model, xg::Guid g = {}) {
            if (!g.isValid()) {
                g = xg::newGuid();
            }
            model_map.insert({g, model});

            return g;
        }

        xg::Guid add_model(rendering::Model model, const std::string& path, xg::Guid g = {}) {
            if (!g.isValid()) {
                g = xg::newGuid();
            }
            model_map.insert({g, model});
            path_to_model[path] = g;
            model_to_path[g] = path;
            return g;
        }

        rendering::Model get_model(ModelHandle id) {
            return model_map.at(id);
        }

        bool check_ref(xg::Guid ref) {
            return model_map.contains(ref);
        }

        bool has_model_path(const std::string& path) const {
            return path_to_model.contains(path);
        }

        xg::Guid get_model_ref_from_path(const std::string& path) const {
            return path_to_model.at(path);
        }

        // Get a vector of all loaded models for GUI

        std::vector<LoadedModelInfo> get_loaded_models() const {
            std::vector<LoadedModelInfo> out;
            out.reserve(model_map.size());

            for (const auto& [ref, model] : model_map) {
                LoadedModelInfo info;
                info.model_ref = ref;

                auto it = model_to_path.find(ref);
                if (it != model_to_path.end()) {
                    info.model_path = it->second;
                    out.push_back(info);
                }

            }

            return out;
        }

    private:
        std::map<ModelHandle, rendering::Model> model_map;
        std::unordered_map<std::string, ModelHandle> path_to_model;
        std::unordered_map<ModelHandle, std::string> model_to_path;
    };
}