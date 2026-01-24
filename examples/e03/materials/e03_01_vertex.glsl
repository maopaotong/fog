#version 330 core 
//uniform mat4 worldviewproj_matrix;
uniform mat4 projection;
in vec3 vertex;
in vec2 uv0;//
in vec2 uv1;//

out vec2 fElm;
out vec2 fPos;
void main() {
	
	vec4 gPos = projection * vec4(vertex, 1.0);
	gl_Position = gPos;
	float e = (vertex.z + 1.0) * 0.5;
	fElm = vec2(e, 0.5);
	fPos = vertex.xy * 0.001 + 1.0 / 64.0;	
	fPos.y = 1.0 - fPos.y;//flip y
}