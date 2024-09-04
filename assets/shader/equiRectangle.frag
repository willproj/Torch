#version 430 core

in vec3 TexCoords;
out vec4 color;

uniform samplerCube cubemap;

void main()
{
    // Convert from cubemap to equirectangular coordinates
    vec3 direction = normalize(TexCoords);
    float theta = atan(direction.z, direction.x);
    float phi = acos(direction.y);
    vec2 uv = vec2(theta / (2.0 * 3.14159265359) + 0.5, phi / 3.14159265359);
    color = texture(cubemap, direction);
}
