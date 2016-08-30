#version 330 core

layout(location = 0) in vec2 inPos;
out vec2 outTex;

void main(void)
{
	gl_Position = vec4(inPos, 0, 1);
	outTex = (inPos + 1.0) / 2.0;
}
