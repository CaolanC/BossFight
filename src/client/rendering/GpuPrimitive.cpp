#include <rendering/GpuPrimitive.hpp>
#include <vector>

namespace rendering {
    GpuPrimitive pr_lines(std::vector<float> const& vertices) {
        GpuPrimitive pr;
        glGenVertexArrays(1, &pr.vao);
        glBindVertexArray(pr.vao);

        glGenBuffers(1, &pr.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, pr.vbo);

        pr.mode = GL_LINES;
        pr.vertexCount = 3 * sizeof(float);

        glBufferData(GL_ARRAY_BUFFER,
             vertices.size() * sizeof(float),
             vertices.data(),
             GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        return pr;
    }
}
