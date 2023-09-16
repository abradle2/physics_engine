#version 330 core

in vec4 color;
in vec3 pos;
in vec3 center;
in float size;

out vec4 FragColor;

void main(){
	float r = length(pos.xy - center.xy);
	if (r > 0.5*size) 
		discard;
	else
		FragColor = color;
}