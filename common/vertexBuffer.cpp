#include <GL/glew.h>
#include "vertexBuffer.h"
#include <GLFW/glfw3.h>


VertexBuffer::VertexBuffer(int maxVertices) {
    glGenBuffers(1, &m_vb);
    glBindBuffer(GL_ARRAY_BUFFER, m_vb);
    glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(Vertex), NULL, GL_STREAM_DRAW);
}

VertexBuffer::~VertexBuffer() {

}

void VertexBuffer::addVertices(std::vector<Vertex> vertices) {
    const float size = vertices.size() * sizeof(Vertex);
    glBufferSubData(GL_ARRAY_BUFFER, m_curIndex, size, vertices.data());
    m_curIndex += vertices.size();
}


