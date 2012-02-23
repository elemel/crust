#version 120

varying vec2 pos;

void main()
{
    // Transforming The Vertex
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
 
    // Passing The Texture Coordinate Of Texture Unit 0 To The Fragment Shader
    pos = vec2(gl_MultiTexCoord0);
}
