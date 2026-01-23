#version 330 core

uniform sampler2D tex_elevation;
uniform sampler2D tex_output;

in vec2 fUv;

void main() {
    vec3 c0 = texture2D(tex_elevation, fUv).xyz;
    vec3 c1 = texture2D(tex_output, fUv).xyz;
    gl_FragColor = vec4(c1, 1);

}//end of main()
