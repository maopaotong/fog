#version 330 core

uniform mat4 projection;

in vec3 vertex;
in vec2 uv0;
out vec2 fUv;

void main() {
	vec4 gPos = projection * vec4(vertex, 1.0);
	gl_Position = gPos;
	fUv = vertex.xy * 0.001;	
	fUv.y = 1.0 - fUv.y;//flip y
	fUv += 1.0 / 64;//offset to avoid edge artifacts
}
