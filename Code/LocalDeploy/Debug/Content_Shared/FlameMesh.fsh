#version 400

in vec2 uv;

uniform sampler2D noiseTexture;
uniform float t;
uniform float alpha;
uniform float discardThreshold;

out vec4 FragColor;

void main()
{
	vec4 noise = texture(noiseTexture, (uv+vec2(t*0.166, t*0.24)));
	noise += texture(noiseTexture, (uv+vec2(t, t*0.42)));
	noise *= 0.5;
	if(noise.r < discardThreshold)
		discard;
	FragColor = vec4(mix(vec3(1.0, 0.0, 0.0), vec3(0.8, 0.3, 0.0), smoothstep(0.0, 1.0, uv.y)), clamp(alpha, 0.0, 1.0));
}
