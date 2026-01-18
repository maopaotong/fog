#version 150 core

in vec3 vertex;
in vec2 uv0;
in vec3 normal;

uniform mat4 world_matrix;
uniform mat4 worldviewproj_matrix;

out vec3 fPosition;//world position.
out vec3 fNormal;

void main() {

	gl_Position = worldviewproj_matrix * vec4(vertex, 1.0);
	//shader out var.
	vec4 vPosition = world_matrix * vec4(vertex, 1.0);
	fPosition = vPosition.xyz;	
	fNormal = normal;
}
