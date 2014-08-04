#version 400

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec2 VertexUV;
layout (location=2) in vec3 VertexNormal;

uniform mat3 NormalMat;
uniform mat4 ModelView;
uniform mat4 MVP;

out vec2 uv;
out vec3 FragNormal;
out vec4 FragPos_View;

void main() {
	uv = VertexUV;
	FragNormal = NormalMat * VertexNormal;
	FragPos_View = ModelView * vec4(VertexPosition, 1.0);
	gl_Position = MVP * vec4(VertexPosition, 1.0);
}