#version 330 core 
//uniform mat4 worldviewproj_matrix;
uniform mat4 projection;
in vec3 aPos;
in vec2 uv0;
in vec2 uv1;

out vec2 fUv0;
void main() {
	//gl_Position = worldviewproj_matrix * vec4(aPos, 1.0);
	//vec3 tPos = aPos;
	vec3 tPos = aPos;
	tPos = vec3(uv0.x, uv0.y, 0);
	mat4 pro = projection;
//	pro = worldviewproj_matrix;
	pro = projection;
	//tPos = vec3(uv1.x, 0, uv1.y);
	float scale = 0.01;
	gl_Position = vec4(tPos.x * scale, tPos.y * scale, 0, 1.0 );

	
	gl_Position = pro * vec4(tPos, 1.0);

	fUv0 = uv1;
}
