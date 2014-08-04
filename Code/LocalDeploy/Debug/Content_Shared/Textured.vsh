#version 400

layout (location=1) in vec2 vertUV;

out vec2 uv;
uniform mat4 rotateUVs;

void main() {
	uv = vertUV;
	gl_Position = rotateUVs * vec4(vec2(0.6, 0.6) + vertUV/2.5, 0.0, 1);
}