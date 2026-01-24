#version 330 core

uniform sampler2D tex_elevation;
uniform sampler2D tex_colormap;

in float fE;
in vec2 fPos;
out vec4 color;
void main() {
    float m = 0;
    float z = texture2D(tex_elevation, fPos).r;
    vec2 uvC = vec2(fE, m);
    vec4 c1 = texture2D(tex_colormap, uvC);
    color = c1;       
}