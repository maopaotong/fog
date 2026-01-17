#version 120 
uniform mat4 worldviewproj_matrix;
attribute  vec3 vertex;
void main() {
	gl_Position = worldviewproj_matrix * vec4(vertex, 1.0);
}
