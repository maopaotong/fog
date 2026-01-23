#version 330 core 
//uniform mat4 worldviewproj_matrix;
uniform mat4 projection;
in vec3 vertex;
in vec2 uv0;//
in vec2 uv1;//

out float elevation;
out vec2 fUv;
void main() {

	vec4 gPos = projection * vec4(vertex.xy, vertex.z * 10, 1.0);
	//gPos = vec4(vertex.x * 0.001 * 0.25, -vertex.y * 0.001 * 0.5, -0.1, 1);
	gl_Position = gPos;
	fUv = vertex.xy * 0.001;
	elevation = vertex.z;
}