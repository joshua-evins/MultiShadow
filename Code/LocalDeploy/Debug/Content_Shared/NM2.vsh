#version 400

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec2 VertexUV;
layout (location=2) in vec3 VertexNormal;
layout (location=3) in vec3 VertexTangent;

uniform mat4 MVP;
uniform mat4 ModelView;
uniform mat3 NormalMat;

uniform vec4 LightPosition_View;

out vec3 LightDir;
out vec2 FragUV;
out vec3 ViewDir;
out vec3 testing;

void main() {
	vec3 norm = normalize(NormalMat * VertexNormal);
	vec3 tang = normalize(NormalMat * VertexTangent);
	
	vec3 binormal = normalize(cross(norm, tang))/* times tang.w*/;
	
	mat3 toObjectLocal = mat3(
		binormal.x, tang.x, norm.x,
		binormal.y, tang.y, norm.y,
		binormal.z, tang.z, norm.z
		);
	
	vec3 pos = vec3(ModelView * vec4(VertexPosition, 1.0));
	LightDir = normalize(toObjectLocal * (LightPosition_View.xyz - pos));
	
	ViewDir = toObjectLocal * normalize(-pos);
	
	FragUV = VertexUV;
	
	testing = vec3(NormalMat * VertexNormal);
	
	gl_Position = MVP * vec4(VertexPosition, 1.0);
}