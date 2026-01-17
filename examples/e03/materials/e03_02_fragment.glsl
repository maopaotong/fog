#version 330 core

uniform sampler2D tex0;
in vec2 fUV;

void main() {

    
    //gl_FragColor = vec4(1,0,1,1);   
    gl_FragColor = texture2D(tex0, fUV);

}//end of main()
