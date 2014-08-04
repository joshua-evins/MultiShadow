#version 400

out vec4 FragColor;
in vec2 uv;
uniform sampler2D shadowMap;

void main() {
	float z = texture2D(shadowMap, uv).r;
	z = pow(z, 100);
	FragColor = vec4(z,z,z,1.0);
}
