#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D lightScene;
uniform sampler2D bloomBlur;
const float exposure = 100.001f;
const float bloomStrength = 0.04f;

vec3 bloom_new()
{
    vec3 hdrColor = texture(lightScene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
//    return mix(hdrColor, bloomColor, 0); // linear interpolation
    return hdrColor * bloomColor;
}

void main()
{
    vec3 result = vec3(0.0);
    // tone mapping
    result = bloom_new();
    result = vec3(1.0) - exp(-result * exposure);
    FragColor = vec4(result, 1.0);
    FragColor = vec4(texture(bloomBlur, TexCoords).rgb, 1.0);
}
