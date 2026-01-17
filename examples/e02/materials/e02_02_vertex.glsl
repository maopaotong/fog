#version 120

uniform mat4 worldviewproj_matrix;

attribute vec3 vertex;
attribute vec2 uv0;
varying vec2 fUV;

void main() {
	gl_Position = worldviewproj_matrix * vec4(vertex, 1.0);
	fUV = uv0;
}
