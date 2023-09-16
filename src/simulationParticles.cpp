#include <stdio.h>
#include <stdlib.h>

#include <array>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/shader.hpp>
#include "simulation.h"

#include <imgui/imgui.h>

class SimulationParticles : public Simulation {
    
public:
    int numParticles = 200000;

    void OnAttach(GLFWwindow* window, int* windowDimensions) override {
        programId = LoadShaders( "assets/shaders/particle.vs", "assets/shaders/particle.fs" );

        // Use our shader
        glUseProgram(programId);

        glfwSwapInterval(0);

        initializeBuffers(numParticles);

        float modelDimensions[2] = {20.0, 20.0};
        /* Our model universe has the origin at the lower left corner, and 
        extends modelDimensions[0] meters in x, and modelDimensions[1] meters in y.
        
                        y
                        ^
                        |
                        |
                        |
                        |
                        |
                        |
                        --------------------> x
                    (0, 0)

            We will need to transform these coordinates into world coordinates that look like
                        
                        y
                        ^  (0, 1)
                        |
            (-1, 0)		|		(1, 0)
                --------|--------> x
                        |
                        |
                        |
                    (0, -1)

            This transformation matrix will be constant for the entire mesh, so we'll send it
            to the GPU as a uniform below.
        */

       // initialize uniforms
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(-1, -1, 0.0));
        transform = glm::scale(transform, glm::vec3(2.0/modelDimensions[0], 2.0/modelDimensions[1], 1.0));

        GLuint transformMatrixId = glGetUniformLocation(programId, "MVP");
        glUniformMatrix4fv(transformMatrixId, 1, GL_FALSE, &transform[0][0]);
        
        GLuint aspectRatioId = glGetUniformLocation(programId, "aspectRatio");
        glUniform2iv(aspectRatioId, 1, windowDimensions);

        // initialize our particle pool
        for(int i=0; i<numParticles; i++){
            particlePool[i].lifeRemaining = -1.0f;
            particlePool[i].position[2] = 1.0f;
        }

        std::vector<Vertex> _vertices(4*numParticles);
        vertices = _vertices;

    }

    void OnUpdate(double timeDelta) override {
        static float height = 18.0;
	    static int newParticles = 10;

		// render GUI
		ImGui::Begin("Tools");
		ImGui::Text("%.1f FPS\n\n", ImGui::GetIO().Framerate);
		
		ImGui::SliderFloat("height", &height, 1, 19);
		ImGui::SliderInt("# particles", &newParticles, 1, 1000);
		ImGui::End();

		
		for (int i=0; i<newParticles; i++) {
			int particleIdx = FindUnusedParticle();
			if (particleIdx == -1) {
				// no new particles available
				break;
			}
			particlePool[particleIdx].lifeRemaining = 5.0f;
			
			particlePool[particleIdx].position = glm::vec3(0.3, height, 0.0);
			glm::vec3 mainDir = glm::vec3(2.5f, 3.0f, 0.0f);
			
			float spread = 2.5f;
			glm::vec3 randomdir = glm::vec3(
				(rand()%2000 - 1000.0f)/1000.0f,
				(rand()%2000 - 1000.0f)/1000.0f,
				(rand()%2000 - 1000.0f)/1000.0f
			);
			particlePool[particleIdx].speed = mainDir + randomdir*spread;

			if (i % 2 == 0) {
				particlePool[particleIdx].position = glm::vec3(19.3, 18.0, 0.0);
				glm::vec3 mainDir = glm::vec3(-2.5f, 3.0f, 0.0f);	
				particlePool[particleIdx].speed = mainDir - randomdir*spread;
			} 
			
			particlePool[particleIdx].color = glm::vec4(
				(rand()%2000 - 1000.0f)/1000.0f,
				(rand()%2000 - 1000.0f)/1000.0f,
				(rand()%2000 - 1000.0f)/1000.0f,
				1.0f
			);
			particlePool[particleIdx].size = 0.1f;
		}

		// physics
		int aliveParticles = 0;
		for (int i=0; i<numParticles; i++) {
			Particle& p = particlePool[i];

			// decrease life
			p.lifeRemaining -= timeDelta;

			if (p.lifeRemaining > 0.0) {
				p.speed += glm::vec3(0.0, -9.81, 0.0) * (float)timeDelta * 0.5f;
				p.position += p.speed * (float)timeDelta;

				// floor
				if (p.position[1] <= 1) {
					p.position[1] = 1.0;
					p.speed[1] = 0.0;

					// friction
					p.speed[0] *= 0.995;
				}

				// right wall
				if (p.position[0] > 19.8) {
					p.speed[0] *= -1;
				}

				// left wall
				if (p.position[0] < 0.2) {
					p.speed[0] *= -1;
				}

				// define a quad, copy it into a vector we can send to a buffer
				std::array<Vertex, 4> q0 = generatePartcleVertices(p.position.x, p.position.y, p.size, p.color);
				memcpy(vertices.data() + aliveParticles*q0.size(), q0.data(), q0.size() * sizeof(Vertex));

				aliveParticles++;	
			}
		}

		// for (int i=0; i<6*4; i++) {
		// 	fprintf(stdout, "%i\n%f %f %f\n%f %f %f %f\n%f %f %f\n%f\n\n",
		// 			i,
		// 			vertices[i].position[0],
		// 			vertices[i].position[1],
		// 			vertices[i].position[2],
		// 			vertices[i].color[0],
		// 			vertices[i].color[1],
		// 			vertices[i].color[2],
		// 			vertices[i].color[3],
		// 			vertices[i].quadCenter[0],
		// 			vertices[i].quadCenter[1],
		// 			vertices[i].quadCenter[2],
		// 			vertices[i].quadSize);
		// }

		// fprintf(stdout, "%lu %lu %i\n", vertices.size(), 4*aliveParticles*sizeof(Vertex), aliveParticles);

		glBufferSubData(GL_ARRAY_BUFFER, 0, 4*aliveParticles*sizeof(Vertex), vertices.data());
		glDrawElements(GL_TRIANGLES, 6*aliveParticles, GL_UNSIGNED_INT, nullptr);
    }

    void OnDetach() override {
        // TODO: clean up VAOs, VBOs, etc etc
    }

private:
    GLuint programId;

    struct Particle {
        glm::vec3 position;
        glm::vec3 speed;
        float size;
        float mass;
        glm::vec4 color;
        float lifeRemaining;
    };

    Particle *particlePool = new Particle[numParticles];
    int LastUsedParticle = 0;


    // for now, every Vertex is part of a Quad
    struct Vertex {
        float position[3];
        float quadCenter[3];
        float quadSize;
        float color[4];
    };

    // pre-initialize array to hold each loop iteration's data to be buffered
    // std::vector<Vertex> vertices(4*numParticles);
    std::vector<Vertex> vertices;

    // Finds a Particle in particlePool which isn't used yet.
    // (i.e. life < 0);
    int FindUnusedParticle(){

        for(int i=LastUsedParticle; i<numParticles; i++){
            if (particlePool[i].lifeRemaining <= 0){
                LastUsedParticle = i;
                return i;
            }
        }

        for(int i=0; i<LastUsedParticle; i++){
            if (particlePool[i].lifeRemaining <= 0){
                LastUsedParticle = i;
                return i;
            }
        }

        return -1; // All particles are taken
    }

    void initializeBuffers(int maxQuads) {
        int maxIndices = 6 * maxQuads;
        int maxVertices = 4 * maxQuads;

        GLuint m_QuadVA, quadVB, quadIB;
        glGenVertexArrays(1, &m_QuadVA);
        glBindVertexArray(m_QuadVA);

        // VBO containing the vertex info (position + color)
        glGenBuffers(1, &quadVB);
        glBindBuffer(GL_ARRAY_BUFFER, quadVB);
        glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(Vertex), NULL, GL_STREAM_DRAW);

        // shader position 0: vertex position in model space
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quadVB);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

        // shader position 1: Quad center
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, quadCenter));

        // shader position 2: Quad size
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, quadSize));
        
        // shader position 3: vertex color
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

        // put quad indices into an element buffer so we can batch render everything together
        std::vector<GLuint> indices(maxIndices);
        int offset = 0;
        for (int i=0; i<maxIndices; i += 6) {
            indices[i + 0] = 0 + offset;
            indices[i + 1] = 1 + offset;
            indices[i + 2] = 2 + offset;
            indices[i + 3] = 2 + offset;
            indices[i + 4] = 3 + offset;
            indices[i + 5] = 0 + offset;

            offset += 4;
        }
        
        glGenBuffers(1, &quadIB);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    }

    static std::array<Vertex, 4> generatePartcleVertices(float x, float y, float size, glm::vec4 color) {
        // x, y: center of quad

        Vertex v0 = {
            .position = {x - size/2, y - size/2, 0.0f},
            .quadCenter = {x, y, 0.0f},
            .quadSize = size,
            .color = {color[0], color[1], color[2], color[3]},
        };
        Vertex v1 = {
            .position = {x + size/2, y - size/2, 0.0f},
            .quadCenter = {x, y, 0.0f},
            .quadSize = size,
            .color = {color[0], color[1], color[2], color[3]},
        };
        Vertex v2 = {
            .position = {x + size/2, y + size/2, 0.0f},
            .quadCenter = {x, y, 0.0f},
            .quadSize = size,
            .color = {color[0], color[1], color[2], color[3]},
        };
        Vertex v3 = {
            .position = {x - size/2, y + size/2, 0.0f},
            .quadCenter = {x, y, 0.0f},
            .quadSize = size,
            .color = {color[0], color[1], color[2], color[3]},
        };

        return {v0, v1, v2, v3};
    }
};
