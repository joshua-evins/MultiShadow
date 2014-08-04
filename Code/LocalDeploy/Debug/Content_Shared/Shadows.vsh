#version 400

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexNormal;
layout (location=2) in vec4 VertexColor;

out vec3 FragPos;
out vec3 WorldPos;
out vec3 Normal;
out vec4 ShadowCoord;
out vec4 FragColor;

uniform mat3 NormalMat;
uniform mat4 ShadowMat;
uniform mat4 ModelViewMat;
uniform mat4 transform;
uniform mat4 Model;

void main() {
	FragPos = (ModelViewMat * vec4(VertexPosition, 1.0)).xyz;
	WorldPos = (Model*vec4(VertexPosition,1.0)).xyz;
	Normal = normalize(NormalMat * VertexNormal);
	ShadowCoord = ShadowMat * vec4(VertexPosition, 1.0);
	FragColor = VertexColor;
	
	gl_Position = transform * vec4(VertexPosition, 1.0);
}