#version 400


in vec2 uv;

uniform sampler2D noiseTexture;
uniform vec4 DarkWoodColor = vec4(0.8, 0.5, 0.1, 1.0);
uniform vec4 LightWoodColor = vec4(1.0, 0.75, 0.25, 1.0);
uniform mat4 Slice;

out vec4 FragColor;

void main()
{
	vec4 cyl = Slice * vec4(uv, 0.0, 1.0);
	
	float dist = length(cyl.xz);
	
	vec4 noise = texture(noiseTexture, uv);
	dist += noise.b;
	
	float t = 1.0 - abs( fract(dist) * 2.0 - 1.0);
	
	vec4 color = mix(DarkWoodColor, LightWoodColor, t);
	
	FragColor = vec4(color.rgb, 1.0);
}
