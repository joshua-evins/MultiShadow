#version 400

layout (location=0) in vec3 VertexPosition;

uniform mat4 VP;

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

uniform int index;

out vec4 FragColor;

void main()
{
	FragColor = VertexColor;
	
	vec3 positionWorld = _lights.lights[index].positionWorld.xyz;
	
	mat4 translation = mat4(
		1.0, 0.0, 0.0, positionWorld.x,
		0.0, 1.0, 0.0, positionWorld.y,
		0.0, 0.0, 1.0, positionWorld.z,
		0.0, 0.0, 0.0, 1.0
	);
	
	gl_Position = VP * translation * vec4(VertexPosition, 1.0);
}