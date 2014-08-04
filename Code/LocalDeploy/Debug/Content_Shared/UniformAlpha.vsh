#version 400

layout (location=0) in vec3 position;
layout (location=1) in vec4 color;

uniform mat4 transform;
uniform float alpha;

out vec4 fragColor;

void main() {
	fragColor = vec4(color.x, color.y, color.z, alpha);
	gl_Position = transform * vec4(position, 1);
}