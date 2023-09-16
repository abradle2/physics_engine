# CMake entry point
cmake_minimum_required( VERSION 3.1 )
project( physics_engine )

find_package(OpenGL REQUIRED)

# brew install glew
find_package(GLEW REQUIRED)

# Compile external dependencies
set( GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL "GLFW lib only" )

add_subdirectory(external)

include_directories(
    external/glfw-3.3.8/include/
    external/glad/include/
    .
)

set(ALL_LIBS
        ${OPENGL_LIBRARY}
        glfw
        glad
        GLEW::glew
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
    -std=c++11
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_executable(application
    src/application.cpp
    src/simulation.h
    src/simulation.cpp
    src/block.cpp
    src/block.h
    
    common/vertexBuffer.cpp
    common/vertexBuffer.h
    common/controls.cpp
    common/controls.hpp
    common/shader.cpp
    common/shader.hpp
    common/types.h

    imgui/imconfig.h
    imgui/imgui.cpp
    imgui/imgui.h
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_glfw.h
    imgui/imgui_impl_opengl3.cpp
    imgui/imgui_impl_opengl3.h
    imgui/imgui_impl_opengl3_loader.h
    imgui/imgui_internal.h
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h

    src/simulationParticles.cpp
    src/simulationCurve.cpp
)

target_link_libraries(application
    ${ALL_LIBS}
)

