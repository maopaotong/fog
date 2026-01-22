#version 330 core

uniform sampler2D tex_0;
uniform sampler2D tex_1;

in vec2 fUV;
 
void main() {

    
    //gl_FragColor = vec4(1,0,1,1);   
    vec4 c0 = texture2D(tex_0, fUV);
    vec4 c1 = texture2D(tex_1, fUV);
    
    gl_FragColor = vec4(c1.x, c1.y, 0,1);

}//end of main()
