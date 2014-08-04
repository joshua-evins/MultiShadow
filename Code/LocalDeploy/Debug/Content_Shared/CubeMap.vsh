#version 400

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexNormal;

uniform vec3 CameraPosition_World;
uniform mat4 MVP;
uniform mat4 ModelMat;
uniform int drawSkybox;

out vec3 ReflectDir;

void main() {
	if(drawSkybox == 1)
	{
		ReflectDir = VertexPosition;
	}
	else
	{
		vec3 worldPos = vec3(vec4(VertexPosition, 1.0));
		vec3 worldNorm = vec3(vec4(VertexNormal, 0.0));
		vec3 worldView = normalize(CameraPosition_World - worldPos);
		ReflectDir = reflect(-worldView, worldNorm);
	}
	gl_Position = MVP * vec4(VertexPosition, 1.0);
}