#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 quadCenter;
layout(location = 2) in float quadSize;
layout(location = 3) in vec4 inColor;

uniform mat4 MVP;
uniform ivec2 aspectRatio;

out vec4 color;
out vec3 pos;
out vec3 center;
out float size;

void main()
{
	vec3 worldPosition = vec3(position.x * aspectRatio.y/aspectRatio.x, position.y, position.z);

	gl_Position = MVP * vec4(worldPosition, 1);
	color = inColor;
	pos = position;
	center = quadCenter;
	size = quadSize;
}