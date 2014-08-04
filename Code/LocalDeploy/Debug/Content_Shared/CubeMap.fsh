#version 400

in vec3 ReflectDir;

uniform samplerCube cubeMap;


uniform int drawSkybox;

//uniform float ReflectFactor;
//uniform vec4 MaterialColor;

out vec4 FinalColor;

void main() {
	vec4 cubeColor = texture(cubeMap, ReflectDir);
	
	FinalColor = cubeColor;
	
	
}
