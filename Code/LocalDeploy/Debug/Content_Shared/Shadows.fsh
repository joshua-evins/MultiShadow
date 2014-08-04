#version 400

uniform vec4 Ambient;
uniform vec4 Diffuse;
uniform vec4 Specular;
uniform float Shininess;

uniform sampler2DShadow shadowMap;

uniform vec4 Lightposition;
uniform vec4 LightdiffI;
uniform vec4 LightspecI;
uniform vec3 lightWorldPos;

in vec4 ShadowCoord;
in vec4 FragColor;
in vec3 FragPos;
in vec3 Normal;
in vec3 WorldPos;

out vec4 color;

vec3 phongDS()
{
	// frag to light
	vec3 s = normalize( Lightposition.xyz - FragPos);
	//frag to eye
	vec3 v = normalize(-FragPos.xyz);
	//light to frag reflected on the normal
	vec3 r = reflect(-s, Normal);
	
	float sDotN = max(dot(s, Normal), 0.0);
	vec3 diffuse = (LightdiffI * Diffuse * sDotN).xyz;
	vec3 spec = vec3(0.0);
	
	if(sDotN > 0.0)
	{
		spec = (LightspecI * Specular *
			pow(max(dot(r,v), 0.0), Shininess)).xyz;
	}
	
	return diffuse + spec;
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine (RenderPassType)
void shadeWithShadow()
{
	vec3 ambient = Ambient.xyz;
	
	vec3 diffAndSpec = phongDS();
	//sc.z = sc.z - 0.0035;
	float shadow = textureProj(shadowMap, ShadowCoord);
	color = vec4(diffAndSpec * shadow + ambient, 1.0);
}

subroutine (RenderPassType)
void recordDepth()
{
	// auto-records depth
}

void main()
{
	RenderPass();
}