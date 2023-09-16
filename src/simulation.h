#ifndef SIMULATION_H_
#define SIMULATION_H_


#include <GLFW/glfw3.h>

class Simulation
{
public:
    Simulation();
    ~Simulation();

    virtual void OnAttach(GLFWwindow* window, int* windowDimensions);
    virtual void OnDetach();
    virtual void OnUpdate(double timeDelta);
};

#endif