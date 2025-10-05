#pragma once
#include <tuple>

namespace generator {

    std::tuple<std::vector<float>, std::vector<unsigned int>> GridPlane() {
        std::vector<float> v = {};
        std::vector<unsigned int> i = {};

        const float fences = 3;


        std::tuple<std::vector<float>, std::vector<unsigned int>> g = std::make_tuple(v, i);
        return g;
    }

}
