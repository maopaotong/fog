#version 330 core

in float elevation;
void main() {
    float e = elevation;
    if(e < 0){
        e = -e;
    }
    gl_FragColor = vec4(e, 0, 0, 1);
}