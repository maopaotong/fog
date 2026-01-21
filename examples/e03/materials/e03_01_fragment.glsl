#version 330 core

in float elevation;
void main() {
    gl_FragColor = vec4(elevation, 0, 0, 1);
}