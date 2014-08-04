#version 400

uniform vec4 LightColor;

out vec4 color;

void main()
{
	color = LightColor;
}