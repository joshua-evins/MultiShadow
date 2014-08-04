#version 400

in vec2 FragUV;
in vec4 FragPosition_View;

uniform sampler2D NormalMap;
uniform mat3 NormalMat;

uniform vec4 LightPosition_View;
uniform vec4 LightDiffuse;
uniform vec4 LightSpecular;

uniform vec4 MaterialDiffuse;
uniform vec4 MaterialSpecular;
uniform float MaterialShininess;

uniform vec4 Ambient;

uniform int UseDiffuse;
uniform int UseSpecular;


out vec4 FinalColor;

vec3 phongDS(
	vec3 f_viewPosition, vec3 f_normal, 
	vec3 l_viewPosition, vec4 l_diffuse, vec4 l_specular,
	vec4 m_diffuse, vec4 m_specular, float m_shininess
	)
{
	vec3 s = normalize( l_viewPosition - f_viewPosition );
	vec3 v = normalize( -f_viewPosition );
	vec3 r = reflect( -s, f_normal);
	
	float sDotN = max(dot(s, f_normal), 0.0);
	vec3 diffuse = (l_diffuse * m_diffuse * sDotN ).xyz;
	vec3 spec = vec3(0.0);
	
	if(sDotN > 0.0)
	{
		spec = (l_specular * m_specular * 
				pow( max( dot( r, v ), 0.0), m_shininess)).xyz;
	}
	if(UseDiffuse == 0)
		diffuse = vec3(0);
	if(UseSpecular == 0)
		spec = vec3(0);
	return diffuse + spec;
}

void main() {
	vec3 rawNormal = texture2D(NormalMap, FragUV).xzy;
	//vec3 rawNormal = texture2D(NormalMap, FragUV).xyz;
	rawNormal += vec3(-.5, -.5, -.5);
	rawNormal *= 2.0;
	vec3 Normal = normalize(NormalMat * rawNormal);
	vec3 diffAndSpec = phongDS(
						FragPosition_View.xyz, Normal,
						LightPosition_View.xyz, LightDiffuse, LightSpecular,
						MaterialDiffuse, MaterialSpecular, MaterialShininess
						);
	FinalColor = vec4(diffAndSpec + Ambient.xyz, 1.0);
	//FinalColor = vec4(1.0);
}
