#version 330 core

uniform sampler2D tex_elevation;
uniform sampler2D tex_colormap;

in vec2 fUv;
in float elevation;
out vec4 color;
void main() {
    vec2 pos = fUv; 
    float m = 0;
    float z = texture2D(tex_elevation, pos).r;
    vec4 c1 = texture2D(tex_colormap, vec2(z, m));
    color = c1;
    // if(e >= 0.5) {
    //     color = vec4(0, e, 0, 1);
    // } else {
    //     color = vec4(0, e, 0, 1);
    // }

    
}