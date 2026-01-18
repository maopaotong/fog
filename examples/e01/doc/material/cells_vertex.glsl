#version 150 core

in vec3 vertex;
in vec2 uv0;
in vec3 normal;

uniform mat4 world_matrix;
uniform mat4 worldviewproj_matrix;
uniform int cellsCols;
uniform int cellsRows;
// uniform int meshQuality;
// uniform int unitCols;
// uniform int unitRows;
uniform float unitWidth;
uniform float unitHeight;

out vec2 fUV;
out vec2 fUV1;
out vec2 fUV2;
out vec3 fPosition;//world position.
out vec2 fPos2D;//position in the cells rad=1 space.

//out vec3 fPosRad1Adj;// adjust for the edge , move position to the centre of the rect for edge operation.

out vec3 fNormal;

const float sqrt3 = sqrt(3.0);
const float sqrt3D2 = sqrt3 / 2.0;

//
const int LT_POINTY_TOP = 0;
const int LT_FLAT_TOP = 1;
//value in normal

// float rectUnitWidth = unitWidth / (unitCols * meshQuality); //rad == 1: if terrQuality = 2, rect width = 1, if terrQuality = 4, rect width = 0.5;
// float rectUnitHeight = unitHeight / (unitRows * meshQuality);
//arguments.
const float rad = 30.0;
//const int cellsCols = 129;
//int cellsRows = 129;
// 
const float radB1 = 1.0 / rad;
//world width , height
// float rectWidth = rectUnitWidth * rad;
// float rectHeight = rectUnitHeight * rad;

float wWidth = unitWidth * rad * cellsCols;
float wHeight = unitHeight * rad * cellsRows;
// float rectWidthInUV = rectWidth / wWidth;
// float rectHeightInUV = rectHeight / wHeight;

//

float adjX = 0.0;
float adjY = 0.0;//+ rectWidthInUV / 2.0f;
// vec2 uv1Offset = vec2(rectWidthInUV / 2.0 + adjX, rectHeightInUV / 2.0 + adjY);

void main() {

	gl_Position = worldviewproj_matrix * vec4(vertex, 1.0);
	//shader out var.
	vec4 vPosition = world_matrix * vec4(vertex, 1.0);
	fPosition = vPosition.xyz;
	fUV = uv0;
	fNormal = normal;

	fUV1 = vec2(fPosition.x / (rad * 2), -fPosition.z / (rad * 2));// + uv1Offset;//
	fUV2 = vec2(fPosition.x / wWidth , -fPosition.z / wHeight);
//vec2 pIn2D = vec2(fPosition.x, -fPosition.z) * (1.0 / 30.0) + vec2(-1.0, sqrt3 / 2.0);//offset and normal rad to 1.

	fPos2D = vec2(fPosition.x * radB1, -fPosition.z * radB1);// + adjOffset;//offset and normal rad to 1.	
	//fPosRad1Adj = fPosRad1 + rad1AdjOffset;

}
