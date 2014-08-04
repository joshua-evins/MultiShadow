#version 400

in vec3 LightDir;
in vec2 FragUV;
in vec3 ViewDir;
in vec3 testing;

uniform sampler2D NormalMap;

uniform vec4 LightIntensity;

uniform vec4 MaterialColor;
uniform float MaterialShininess;

uniform vec4 Ambient;

uniform int UseDiffuse;
uniform int UseSpecular;


out vec4 FinalColor;

vec3 phongModel( vec3 norm )
{
	vec3 r = reflect( -LightDir, norm);
	vec3 ambient = Ambient.xyz;
	
	float sDotN = max(dot(LightDir, norm), 0.0);
	vec3 diffuse = (LightIntensity * MaterialColor * sDotN ).xyz;
	vec3 spec = vec3(0.0);
	
	if(sDotN > 0.0)
	{
		spec = (LightIntensity * MaterialColor * 
				pow( max( dot( r, ViewDir ), 0.0), MaterialShininess)).xyz;
	}
	
	return vec3(diffuse + spec);
	
	if(UseDiffuse == 0)
		diffuse = vec3(0);
	if(UseSpecular == 0)
		spec = vec3(0);
	
	return diffuse + spec + ambient;
}

void main() {
	vec3 normal = texture(NormalMap, FragUV).xyz *2.0 - 1.0;
	
	//FinalColor = vec4(normal.xyz, 1.0);
	
	FinalColor = vec4(phongModel(normal.xyz), 1.0);
}
