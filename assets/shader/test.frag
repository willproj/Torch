#version 330 core
in vec3 WorldPos;
out vec4 FragColor;

uniform samplerCube irradianceMap;

void main()
{
    // Sample the irradiance cubemap using the world position of the cube's face
    vec3 color = texture(irradianceMap, WorldPos).rgb;
    FragColor = vec4(color, 1.0);
}
