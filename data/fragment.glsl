#define PI 3.14159265358979323846264

uniform sampler2D colorTexture;
uniform sampler2D normalAndShadowTexture;
uniform vec2 textureSize;
uniform float smoothDistance;

varying vec2 texCoord;

void main()
{
    vec2 linearPosition = texCoord * textureSize;
    vec2 nearestPosition = floor(linearPosition) + 0.5;
    vec2 centerOffset = linearPosition - nearestPosition;
    vec2 edgeDistance = 0.5 - abs(centerOffset);
    vec2 smoothOffset = 0.5 * sign(centerOffset) * (1.0 - clamp(edgeDistance / smoothDistance, 0.0, 1.0));
    vec2 smoothPosition = nearestPosition + smoothOffset;
    vec4 color = texture2D(colorTexture, smoothPosition / textureSize);

    vec4 normalAndShadow = texture2D(normalAndShadowTexture, texCoord - 0.25 / textureSize);
    vec3 normal = normalAndShadow.xyz;
    float shadow = normalAndShadow.w;

    gl_FragColor.rgb = color.rgb;
    gl_FragColor.a = color.a + shadow * (1.0 - color.a);
}
