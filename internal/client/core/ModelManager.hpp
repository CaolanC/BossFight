#pragma once

#include <core/defines.hpp>
#include <rendering/Model.hpp>
#include <crossguid/guid.hpp>
namespace core {

    class ModelManager {
    public:
        ModelManager() {

        };

        xg::Guid add_model(rendering::Model model) {
            auto g = xg::newGuid();
            model_map.insert({g, model});

            return g;
        }

        rendering::Model get_model(ModelHandle id) {
            return model_map.at(id);
        }
    private:
        std::map<ModelHandle, rendering::Model> model_map;
    };
}