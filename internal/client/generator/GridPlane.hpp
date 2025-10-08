#pragma once
#include <tuple>
#include <rendering/Model.hpp>

namespace generator {

    std::vector<float> grid_plane(unsigned int cell_width, unsigned int no_cells) {;
        std::vector<float> vertices = {};
        std::vector<float> curr = {};
        for (int x = 0; x < no_cells+1; x++) {
            vertices.insert(vertices.end(), {static_cast<float>(x), 0.0f, static_cast<float>((cell_width+1)*x)});
            std::cout << vertices[x] << vertices[x+1] << vertices[x+2] << '\n';
        }

        return vertices;
    }

}
