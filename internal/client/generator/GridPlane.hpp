#pragma once
#include <tuple>
#include <rendering/Model.hpp>

namespace generator {

    rendering::GpuPrimitive lines(std::vector<float> points) {;
        auto pr = rendering::GpuPrimitive();
        glGenVertexArrays(1, &pr.vao);
        glBindVertexArray(pr.vao);

        glGenBuffers(1, &pr.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, pr.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        return pr;
    }

}
