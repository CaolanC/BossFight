#pragma once

#include <core/defines.hpp>
#include <rendering/Model.hpp>
#include <crossguid/guid.hpp>
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

        rendering::Model get_model(ModelHandle id) {
            return model_map.at(id);
        }

        bool check_ref(xg::Guid ref) {
            return model_map.contains(ref);
        }

    private:
        std::map<ModelHandle, rendering::Model> model_map;
        std::unordered_map<std::string, ModelHandle> path_to_model;
    };
}