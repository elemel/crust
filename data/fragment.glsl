varying vec2 texCoord;
uniform sampler2D colorTexture;

void main()
{
    // Sampling The Texture And Passing It To The Frame Buffer
    gl_FragColor = texture2D(colorTexture, texCoord);
}
