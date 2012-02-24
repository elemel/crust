uniform sampler2D colorTexture;
uniform sampler2D shadowTexture;
uniform vec2 offset;

varying vec2 texCoord;

void main()
{
    vec4 color1 = texture2D(colorTexture, texCoord + vec2(-offset.x, -offset.y));
    vec4 color2 = texture2D(colorTexture, texCoord + vec2(offset.x, -offset.y));
    vec4 color3 = texture2D(colorTexture, texCoord + vec2(offset.x, offset.y));
    vec4 color4 = texture2D(colorTexture, texCoord + vec2(-offset.x, offset.y));
    vec4 shadowColor = texture2D(shadowTexture, texCoord);
    float totalAlpha = color1.a + color2.a + color3.a + color4.a;
    if (totalAlpha < 0.001) {
        gl_FragColor = shadowColor;
    } else {
        gl_FragColor.rgb = (color1.rgb * color1.a + color2.rgb * color2.a + color3.rgb * color3.a + color4.rgb * color4.a) / totalAlpha;
        gl_FragColor.a = 0.25 * totalAlpha;
        gl_FragColor.rgb = gl_FragColor.rgb * gl_FragColor.a + shadowColor.rgb * shadowColor.a * (1.0 - gl_FragColor.a);
        gl_FragColor.a = gl_FragColor.a + shadowColor.a * (1.0 - gl_FragColor.a);
    }
}
