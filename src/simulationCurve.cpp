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
#include "block.h"
#include "common/vertexBuffer.h"
#include <common/types.h>

#include <imgui/imgui.h>

class SimulationCurve : public Simulation {
    
public:
    

    void OnAttach(GLFWwindow* window, int* windowDimensions) override {
        programId = LoadShaders( "assets/shaders/curve.vs", "assets/shaders/curve.fs" );

        // Use our shader
        glUseProgram(programId);

        // enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        initializeBuffers();
        initializeCamera(windowDimensions);

        std::vector<Vertex> _vertices(10000);
        vertices = _vertices;
    }

    void OnUpdate(double timeDelta) override {
		static int numPoints = 200;
        static float xMin = -10;
        static float xMax = 10.0;
        static float phase = 0.0;
        static float frequency = 4.0;
        static float thickness = 0.05f;

        static int cameraX = -29;
        static int cameraY = 5;
        static int cameraZ = 34;

        static int cameraLookAtX = 0;
        static int cameraLookAtY = 0;
        static int cameraLookAtZ = 0;


        
        // render GUI
		ImGui::Begin("Tools");
		ImGui::Text("%.1f FPS\n\n", ImGui::GetIO().Framerate);
        ImGui::SliderInt("# of line segments", &numPoints, 1, 5000);
        ImGui::SliderFloat("Min x", &xMin, -10, 10);
		ImGui::SliderFloat("Max x", &xMax, -10, 10);
        ImGui::SliderFloat("Frequency", &frequency, 0.1, 1000);
        ImGui::SliderFloat("Phase", &phase, -2*3.14, 2*3.14);
        ImGui::SliderFloat("Thickness", &thickness, 0, 1);

        ImGui::Text("Camera\n");
        ImGui::Text("  Position\n");
        ImGui::SliderInt("pos X", &cameraX, -100, 100);
        ImGui::SliderInt("pos Y", &cameraY, -100, 100);
        ImGui::SliderInt("pos Z", &cameraZ, -100, 100);
        ImGui::Text("  Look at\n");
        ImGui::SliderInt("look at X", &cameraLookAtX, -100, 100);
        ImGui::SliderInt("look at Y", &cameraLookAtY, -100, 100);
        ImGui::SliderInt("look at Z", &cameraLookAtZ, -100, 100);

		ImGui::End();

        // camera
        glm::mat4 viewMatrix = glm::lookAt(
            glm::vec3(cameraX, cameraY, cameraZ),
            glm::vec3(cameraLookAtX, cameraLookAtY, cameraLookAtZ),
            glm::vec3(0,1,0)
        );
        GLuint viewUniformLocationId = glGetUniformLocation(programId, "view");
        glUniformMatrix4fv(viewUniformLocationId, 1, GL_FALSE, &viewMatrix[0][0]);

        // plot
        glm::vec3 points[numPoints];
        double xs[numPoints];

        for (int i=0; i<numPoints; i++) {
            double x = xMin + (double)i * (xMax - xMin) / (double)(numPoints);
            xs[i] = x;
        }

        for (int i=0; i<numPoints; i++) {
            const double x = xs[i];
            const double y = 2 + glm::cos(frequency*x + phase);
            points[i] = {
                glm::vec3(x, y, 0.0f)
            };
        }

        for (int i=0; i<numPoints-1; i++) {
            glm::vec4 color = {
                points[i].x / 3.0f,
                points[i].x / 5.0f,
                points[i].x / 6.0f,
                1.0f
            };
            std::vector<Vertex> lineSegment = generateVerticesForLineSegment(
                points[i],
                points[i+1],
                thickness,
                color
            );

            memcpy(vertices.data() + i*lineSegment.size(), lineSegment.data(), lineSegment.size() * sizeof(Vertex));
        }

        // 3d plane below plot
        Block xAxis = Block(xMax, xMin, 0.25f, 0.55f, 10.0f, -10.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        memcpy(vertices.data() + (2*numPoints-2), xAxis.getVertices().data(), xAxis.getVertices().size() * sizeof(Vertex));

        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        const int numSegments = numPoints - 1;
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()*sizeof(Vertex), vertices.data());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, numSegments);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLE_STRIP, numSegments-2, numSegments-1);

        // draw x axis
        glDrawArrays(GL_TRIANGLES, 2*numPoints-2, 6*6);
    }

    void OnDetach() override {
        // TODO: clean up VAOs, VBOs, etc etc
    }

private:
    GLuint programId;

    // struct Vertex {
    //     glm::vec3 position;
    //     glm::vec4 color;
    // };

    // pre-initialize array to hold each loop iteration's data to be buffered
    // std::vector<Vertex> vertices(4*numParticles);
    std::vector<Vertex> vertices;

    void initializeBuffers() {
        int maxVertices = 10000;
        int maxIndices = 10000;

        GLuint m_QuadVA, quadVB, quadIB;
        glGenVertexArrays(1, &m_QuadVA);
        glBindVertexArray(m_QuadVA);

        // VBO containing the vertex info
        VertexBuffer vertexBuffer = VertexBuffer(maxVertices);

        // glGenBuffers(1, &quadVB);
        // glBindBuffer(GL_ARRAY_BUFFER, quadVB);
        // glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(Vertex), NULL, GL_STREAM_DRAW);

        // shader position 0: vertex position in model space
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quadVB);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
        
        // shader position 1: vertex color
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

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

    void initializeCamera(int* windowDimensions) {
        // Projection matrix : 45° Field of View, window's aspect ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 projectionMatrix = glm::perspective(
            glm::radians(45.0f), 
            (float) windowDimensions[0] / (float)windowDimensions[1], 
            0.1f, 
            100.0f
        );
        GLuint projectionUniformLocationId = glGetUniformLocation(programId, "projection");
        glUniformMatrix4fv(projectionUniformLocationId, 1, GL_FALSE, &projectionMatrix[0][0]);

        // Camera matrix
        glm::mat4 viewMatrix = glm::lookAt(
            glm::vec3(20,3,3), // camera in World Space
            glm::vec3(0,0,0),  // look at the origin
            glm::vec3(0,1,0)   // head is up
        );
        GLuint viewUniformLocationId = glGetUniformLocation(programId, "view");
        glUniformMatrix4fv(viewUniformLocationId, 1, GL_FALSE, &viewMatrix[0][0]);

        // Model matrix : an identity matrix (model will be at the origin)
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        GLuint modelUniformLocationId = glGetUniformLocation(programId, "model");
        glUniformMatrix4fv(modelUniformLocationId, 1, GL_FALSE, &modelMatrix[0][0]);
    }

    static std::vector<Vertex> generateVerticesForLineSegment(glm::vec3 start, glm::vec3 end, float thickness, glm::vec4 color) {
        float cosTheta = (end.y - start.y) / glm::sqrt(glm::pow(end.x-start.x, 2) + glm::pow(end.y-start.y, 2));
        float sinTheta = (end.x - start.x) / glm::sqrt(glm::pow(end.x-start.x, 2) + glm::pow(end.y-start.y, 2));

        const float t = thickness; // to make math cleaner


        Vertex v0 = {
            .position = {start.x - t/2*cosTheta, start.y + t/2*sinTheta, 0.0f},
            .color = color,
        };
        Vertex v1 = {
            .position = {start.x + t/2*cosTheta, start.y - t/2*sinTheta, 0.0f},
            .color = color,
        };
        Vertex v2 = {
            .position = {end.x - t/2*cosTheta, end.y + t/2*sinTheta, 0.0f},
            .color = color,
        };
        Vertex v3 = {
            .position = {end.x + t/2*cosTheta, end.y - t/2*sinTheta, 0.0f},
            .color = color,
        };

        return { v2, v3};
    }
};
