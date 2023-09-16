#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 inColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec4 color;

void main()
{
	mat4 MVP = projection * view * model;
	gl_Position = MVP *vec4(position, 1.0f);
	color = inColor;
}