#version 400

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec2 VertexUV;

uniform mat4 MVP;
uniform mat4 ModelView;

out vec2 FragUV;
out vec4 FragPosition_View;

void main() {
	FragPosition_View = ModelView * vec4(VertexPosition, 1.0);
	FragUV = VertexUV;
	gl_Position = MVP * vec4(VertexPosition, 1.0);
}