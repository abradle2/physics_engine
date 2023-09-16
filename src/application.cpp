#include <iostream>
#include <stdio.h>
#include <typeinfo>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "simulationParticles.cpp"
#include "simulationCurve.cpp"

// defined below
GLFWwindow* initialize(int*);


int main() {
    // set up the application
    int windowDimensions[2] = {1024, 768};  // size in px of our openGL window
    GLFWwindow* window = initialize(windowDimensions);
    if (window == NULL) {
		fprintf( stderr, "Initialization failed\n" );
		getchar();
		return -1;
	}

    // determine which simulation we want to run and attach it
    SimulationCurve sim = SimulationCurve();
	// SimulationParticles sim = SimulationParticles();
	
    sim.OnAttach(window, windowDimensions);

    // main loop
    double lastTime = glfwGetTime();
    do
    {
        double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;

        // Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// feed inputs to ImGui, start new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

        sim.OnUpdate(delta);

        // ImGui render to screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

    sim.OnDetach();

    // top-level cleanup
    glfwTerminate();
    
    
    return 0;
}


GLFWwindow* initialize(int* windowDimensions) {
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return nullptr;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	GLFWwindow* window = glfwCreateWindow( windowDimensions[0], windowDimensions[1], "Particle Simulator", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window\n" );
		getchar();
		glfwTerminate();
		return window;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return window;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwPollEvents();

	// Set the "clear color" to Black
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test - accept fragment if closer to the camera than the former one
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true); 
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();

	return window;
}