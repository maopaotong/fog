#version 330 core

uniform sampler2D tex_0;
uniform sampler2D tex_1;
uniform sampler2D tex_cm;

in vec2 fUv;

void main() {
    vec3 c0 = texture2D(tex_0, fUv).xyz;
    vec3 c1 = texture2D(tex_1, fUv).xyz;
    vec3 cm = texture2D(tex_cm, fUv).xyz;
    gl_FragColor = vec4(c1, 1);

}//end of main()
