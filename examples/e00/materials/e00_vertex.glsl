#version 150 core

uniform mat4 worldviewproj_matrix;

in vec3 vertex;

void main() {
	gl_Position = worldviewproj_matrix * vec4(vertex, 1.0);
}
