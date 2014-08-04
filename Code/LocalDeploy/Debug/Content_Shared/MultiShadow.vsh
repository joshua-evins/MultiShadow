#version 400

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexNormal;
layout (location=2) in vec4 VertexColor;

uniform mat4 ModelMat;
uniform mat4 MVP;
uniform mat3 NormalMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelView;

struct PointLight
{
	mat4 shadowMatrix;
	vec4 positionWorld;
	vec4 diffuseColor;
	vec4 specularColor;
	int shadowMap;
	int active;
	int padding1;
	int padding2;
};

layout (std140) uniform PointLights {
	PointLight lights[4];
}_lights;

/*
struct OneVertexProcessedLight {
	vec3 lightPositionView;
	vec4 shadowCoord;
};

out OneVertexProcessedLight VertexProcessedLights[4];
*/

out vec4 FragColor;
out vec3 FragPos_View;
out vec3 Normal;
out vec3 FragWorld;

void main()
{
	FragPos_View = (ModelView * vec4(VertexPosition, 1.0)).xyz;
	Normal = normalize(NormalMatrix * VertexNormal);
	
	//for(int i=0; i<4; i++)
	//{
	//	VertexProcessedLights[i].lightPositionView = (ViewMatrix * _lights.lights[i].positionWorld).xyz;
	//	VertexProcessedLights[i].shadowCoord = _lights.lights[i].shadowMatrix * ModelMat * vec4(VertexPosition,1.0);
	//}
	
	FragWorld = (ModelMat * vec4(VertexPosition, 1.0)).xyz;

	FragColor = VertexColor;
	
	gl_Position = MVP * vec4(VertexPosition, 1.0);
}