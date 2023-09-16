#ifndef BLOCK_H_
#define BLOCK_H_

#include <glm/glm.hpp>
#include <vector>

#include <common/types.h>


class Block
{
public:
    Block(float posX,
          float negX,
          float posY,
          float negY,
          float posZ,
          float negZ,
          glm::vec4 color);
    
    ~Block();

    void Render();

    std::vector<Vertex> getVertices() {
        return m_vertices;
    }


protected:
    std::vector<Vertex> m_vertices;
};

#endif  /* BLOCK_H_ */