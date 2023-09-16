#include "block.h"
#include <vector>
#include <glm/glm.hpp>


Block::Block(float posX,
             float negX,
             float posY,
             float negY,
             float posZ,
             float negZ,
             glm::vec4 color) {
    
    m_vertices = {
        // front face
        {glm::vec3(negX, negY, posZ), color},
        {glm::vec3(posX, negY, posZ), color},
        {glm::vec3(posX, posY, posZ), color},
        
        {glm::vec3(posX, posY, posZ), color},
        {glm::vec3(negX, posY, posZ), color},
        {glm::vec3(negX, negY, posZ), color},
        // back face
        {glm::vec3(negX, negY, negZ), color},
        {glm::vec3(posX, negY, negZ), color},
        {glm::vec3(posX, posY, negZ), color},
        
        {glm::vec3(posX, posY, negZ), color},
        {glm::vec3(negX, posY, negZ), color},
        {glm::vec3(negX, negY, negZ), color},

        // top face
        {glm::vec3(negX, posY, posZ), color},
        {glm::vec3(posX, posY, posZ), color},
        {glm::vec3(posX, posY, -10.0f), color},
        
        {glm::vec3(posX, posY, -10.0f), color},
        {glm::vec3(negX, posY, -10.0f), color},
        {glm::vec3(negX, posY, posZ), color},

        // bottom face
        {glm::vec3(negX, negY, posZ), color},
        {glm::vec3(posX, negY, posZ), color},
        {glm::vec3(posX, negY, negZ), color},
        
        {glm::vec3(posX, negY, negZ), color},
        {glm::vec3(negX, negY, negZ), color},
        {glm::vec3(negX, negY, posZ), color},

        // left face
        {glm::vec3(negX, negY, posZ), color},
        {glm::vec3(negX, negY, negZ), color},
        {glm::vec3(negX, posY, negZ), color},
        
        {glm::vec3(negX, posY, negZ), color},
        {glm::vec3(negX, posY, posZ), color},
        {glm::vec3(negX, negY, posZ), color},

        // right face
        {glm::vec3(posX, 0.25f, posZ), color},
        {glm::vec3(posX, 0.25f, negZ), color},
        {glm::vec3(posX, 0.55f, negZ), color},
        
        {glm::vec3(posX, 0.55f, negZ), color},
        {glm::vec3(posX, 0.55f, posZ), color},
        {glm::vec3(posX, 0.25f, posZ), color}
    };
    
}
Block::~Block() {

}

void Block::Render() {

}

