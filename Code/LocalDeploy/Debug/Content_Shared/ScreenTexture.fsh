#version 400

out vec4 FragColor;
in vec2 uv;
uniform sampler2D texture;
uniform int colorChannel;

void main() {
	
	vec3 color = vec3(1.0);
	if(colorChannel == 0)						// I'm not sure how well shaders handle this kind of code, but it's more or less debug, anyway
		color = texture2D(texture, uv).rgb;
	else if(colorChannel == 1)
		color = texture2D(texture, uv).rrr;
	else if(colorChannel == 2)
		color = texture2D(texture, uv).ggg;
	else if(colorChannel == 3)
		color = texture2D(texture, uv).bbb;
	else if(colorChannel == 4)
		color = texture2D(texture, uv).aaa;
		
	FragColor = vec4(color, 1.0);
}
