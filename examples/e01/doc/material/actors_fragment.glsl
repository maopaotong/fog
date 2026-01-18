#version 150 core

in vec3 fPosition;
in vec3 fNormal;
//
out vec4 outColor;  //

uniform int debug;
//

void main() {

    vec4 color;
    vec3 normal = fNormal;

    color = vec4(0, 1, 1, 1);

    outColor = color;

}//end of main()
