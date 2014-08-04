#version 400

in vec2 uv;
in vec4 FragPos_View;
in vec3 FragNormal;

uniform vec4 LightPos_View;
uniform sampler2D noiseTexture;
uniform float t;

out vec4 FragColor;

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

void main()
{
	vec4 noise = texture(noiseTexture, uv);
	
	if(noise.r < t)
		discard;
	float distanceBehind = noise.r-t;
	if(distanceBehind < 0.05)
	{
		// Experimenting with a way to make the trail look like particles via discard
		if(distanceBehind < 0.045 && texture(noiseTexture, uv*80.0).r * distanceBehind < 0.5 * 0.03)
			discard;
		FragColor = mix(vec4(0.8, 0.3, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), smoothstep(0.0, 0.05, distanceBehind));
	}
	else
	{
		vec3 diffSpec = phongDS(
		FragPos_View.xyz, FragNormal.xyz, 
		LightPos_View.xyz, vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0),
		vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0), 20.0);

		FragColor = vec4(diffSpec, 1.0);
	}
}
