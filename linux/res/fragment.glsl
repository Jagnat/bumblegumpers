#version 330 core

in vec4 outColor;
in vec2 outTexel;

out vec4 finalColor;

uniform sampler2D texture;

void main(void)
{
	finalColor = texture2D(texture, outTexel) * outColor;
}
