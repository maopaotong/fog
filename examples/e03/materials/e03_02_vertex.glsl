#version 120

uniform mat4 worldviewproj_matrix;

attribute vec3 aPos;
attribute vec2 uv0;
varying vec2 fUV;

void main() {

	gl_Position = worldviewproj_matrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	fUV = uv0;
}
