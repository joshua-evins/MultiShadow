#version 400

layout (location=0) in vec2 vertUV; // being used as a 0-1, 0-1 position

uniform vec2 pos;
uniform vec2 size;

out vec2 uv;

void main() {
	uv = vertUV;
	vec2 position = vertUV;
	position.x = position.x * size.x;
	position.y = position.y * size.y;
	position -= vec2(1.0, 0.0);
	position += vec2(pos.x, -pos.y);
	gl_Position = vec4(position, 0.0, 1);
}