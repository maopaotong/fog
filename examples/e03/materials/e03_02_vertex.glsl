#version 330 core

uniform mat4 worldviewproj_matrix;

in vec3 vertex;
in vec2 uv0;
out vec2 fUV;

void main() {
	gl_Position = worldviewproj_matrix * vec4(vertex, 1.0);
	fUV = uv0;
}
