#version 330 core

uniform sampler2D tex_show;

in vec2 fUv;

void main() {
    vec2 tUv = fUv;
    vec4 c0 = texture2D(tex_show, tUv);
    gl_FragColor = c0;

}//end of main()
