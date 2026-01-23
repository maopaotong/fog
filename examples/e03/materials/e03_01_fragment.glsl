#version 330 core

uniform sampler2D tex_0;
in float elevation;
out vec4 color;
void main() {
    vec4 c0 = texture2D(tex_0, vec2(0.5, 0.5));
    float e = 0.5 * (1.0 + elevation);
    if(e >= 0.5) {
        color = vec4(0, e, 0, 1);
    } else {
        color = vec4(0, e, 0, 1);
    }
}