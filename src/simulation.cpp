#include <stdio.h>

#include "simulation.h"
#include <GLFW/glfw3.h>


Simulation::Simulation() {
    fprintf(stdout, "hello world from Simulation.cpp!\n");
}

Simulation::~Simulation() {

}

void Simulation::OnAttach(GLFWwindow* window, int* windowDimensions) {
    // handle initialization here
}

void Simulation::OnDetach() {
    // handle destruction here
}

void Simulation::OnUpdate(double timeDelta) {
    // handle render loop here
}