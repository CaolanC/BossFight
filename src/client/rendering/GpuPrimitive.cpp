#include <rendering/GpuPrimitive.hpp>
#include <vector>
#include <par_shapes.h>
#include <iostream>

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

    GpuPrimitive par(par_shapes_mesh_s& m) {
        if (!m.normals) {
            par_shapes_compute_normals(&m);
        }
        GpuPrimitive pr{};
        pr.mode = GL_TRIANGLES;

        using index_t = std::remove_pointer_t<decltype(m.triangles)>; // PAR_SHAPES_T

        if constexpr (sizeof(index_t) == 2) {
            pr.indexType = GL_UNSIGNED_SHORT;
        } else if constexpr (sizeof(index_t) == 4) {
            pr.indexType = GL_UNSIGNED_INT;
        } else {
            static_assert(sizeof(index_t) == 2 || sizeof(index_t) == 4,
                          "Unsupported PAR_SHAPES_T size");
        }

        glGenVertexArrays(1, &pr.vao);
        glBindVertexArray(pr.vao);
        std::vector<float> vertices;
        for (int i = 0; i < m.npoints; ++i) {
            int p = 3 * i;
            vertices.push_back(m.points[p]);
            vertices.push_back(m.points[p+1]);
            vertices.push_back(m.points[p+2]);
            vertices.push_back(m.normals[p]);
            vertices.push_back(m.normals[p+1]);
            vertices.push_back(m.normals[p+2]);
        }

        glGenBuffers(1, &pr.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, pr.vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(float),
                     vertices.data(),
                     GL_STATIC_DRAW);

        const GLsizei stride = 6 * sizeof(float);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

        pr.indexCount = static_cast<GLsizei>(m.ntriangles) * 3;

        glGenBuffers(1, &pr.ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pr.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(m.ntriangles) * 3 * sizeof(index_t),
                     m.triangles,
                     GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        pr.vertexCount = static_cast<GLsizei>(m.npoints);
        return pr;
    }

}
