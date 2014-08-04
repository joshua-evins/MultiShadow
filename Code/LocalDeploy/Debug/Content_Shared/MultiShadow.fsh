#version 400

/* material
uniform vec4 Ambient;
uniform vec4 Diffuse;
uniform vec4 Specular;
uniform float Shininess;
*/

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

in OneVertexProcessedLight VertexProcessedLights[4];
*/

in vec4 FragColor;
in vec3 FragPos_View;
in vec3 Normal;
in vec3 FragWorld;
out vec4 color;

uniform sampler2DArrayShadow shadowMaps;

uniform int useShadows;

uniform mat4 ViewMatrix;
uniform mat4 ModelMat;

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

	return diffuse + spec;
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine (RenderPassType)
void shadeWithShadow()
{
	//vec3 ambient = Ambient.xyz;
	vec3 ambient = vec3(0.1);
	vec3 lightingSum = vec3(0.0);
	for(int i=0; i<4; i++)
	{
		vec3 lightPosView = (ViewMatrix * _lights.lights[i].positionWorld).xyz;
		
		
		vec3 diffAndSpec = phongDS(
			FragPos_View, Normal,
			lightPosView, _lights.lights[i].diffuseColor, _lights.lights[i].specularColor,
			vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 20.0
		);
		
		float shadow = 1.0;
		if(useShadows == 1)
		{
			vec4 shadowCoord = _lights.lights[i].shadowMatrix * vec4(FragWorld,1.0);
			vec4 newShadowCoord = shadowCoord;
			newShadowCoord /= newShadowCoord.w;
			newShadowCoord.xyw = newShadowCoord.xyz;
			newShadowCoord.z = _lights.lights[i].shadowMap;
			shadow = texture(shadowMaps, newShadowCoord);
		}
		
		lightingSum += shadow * diffAndSpec;
	}
	
	color = vec4(ambient + lightingSum, 1.0);
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