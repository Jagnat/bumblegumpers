#version 330 core

uniform sampler2D fboTexture;

in vec2 outTex;

out vec4 outColor;

void main(void)
{
	outColor = texture2D(fboTexture, outTex);
}