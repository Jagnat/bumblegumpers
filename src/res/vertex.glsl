#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexel;

out vec4 outColor;
out vec2 outTexel;

uniform mat4 transform = mat4(1.0);

void main(void)
{
	vec4 pos = transform * vec4(inPosition, 1.0);
	gl_Position = pos;
	outColor = inColor;
	outTexel = inTexel;
}
