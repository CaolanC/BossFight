#pragma once

#include <core/defines.hpp>
#include <utils/Model.hpp>
#include <crossguid/guid.hpp>
namespace core {

    class ModelManager {
    public:
        ModelManager() {

        };

        xg::Guid add_model(utils::Model model) {
            auto g = xg::newGuid();
            model_map.insert({g, model});

            return g;
        }

        utils::Model get_model(ModelHandle id) {
            return model_map.at(id);
        }
    private:
        std::map<ModelHandle, utils::Model> model_map;
    };
}