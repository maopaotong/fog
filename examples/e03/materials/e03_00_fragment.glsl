#version 330 core

in float elevation;
out vec4 color;
void main() {
    float e = 0.5 * (1.0 + elevation);
    if(e >= 0.5) {
        color = vec4(e, 0, 0, 1);
    } else {
        color = vec4(e, 0, 0, 1);
    }
}