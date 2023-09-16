#ifndef VERTEX_BUFFER_H_
#define VERTEX_BUFFER_H_

#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <common/types.h>

class VertexBuffer
{
public:
    VertexBuffer(int maxVertices);
    ~VertexBuffer();

    void addVertices(std::vector<Vertex> vertices);
    void resetIndex() {
        m_curIndex = 0;
    }

protected:
    GLuint m_vb;
    int m_curIndex = 0;
};

#endif  /* VERTEX_BUFFER_H_ */