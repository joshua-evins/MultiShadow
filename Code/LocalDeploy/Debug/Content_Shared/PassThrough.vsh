#version 400

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec4 VertexColor;

uniform mat4 MVP;
out vec4 fragColor;

void main() {
	fragColor = VertexColor;
	gl_Position = MVP * vec4(VertexPosition, 1.0);
}