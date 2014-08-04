#version 400

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec2 VertexUV;
layout (location=2) in vec3 VertexNormal;

uniform mat3 NormalMat;
uniform mat4 MVP;
uniform float t;

out vec2 uv;

void main() {
	uv = VertexUV;
	vec3 Normal = NormalMat * VertexNormal;
	gl_Position = MVP * vec4(VertexPosition, 1.0);
}