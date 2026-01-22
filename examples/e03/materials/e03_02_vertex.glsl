#version 330 core

in vec3 vertex;
in vec2 uv0;
out vec2 fUV;

void main() {
	gl_Position = vec4(vertex, 1.0);
	fUV = uv0;
}
