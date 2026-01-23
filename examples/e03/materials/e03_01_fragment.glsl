#version 330 core

uniform sampler2D tex_0;
uniform sampler2D tex_cm;

in float elevation;
out vec4 color;
void main() {
    vec4 c0 = texture2D(tex_0, vec2(0.5, 0.5));
    float m = 0.5;
    vec4 c1 = texture2D(tex_cm, vec2(elevation, m));
    color = c1;
    // if(e >= 0.5) {
    //     color = vec4(0, e, 0, 1);
    // } else {
    //     color = vec4(0, e, 0, 1);
    // }

    
}