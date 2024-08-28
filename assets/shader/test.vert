#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Transform vertex position to world space
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz; 
    
    // Pass texture coordinates to fragment shader
    TexCoords = aTexCoords;
    
    // Transform the normal using the normal matrix
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * aNormal;

    // Calculate final position in clip space
    gl_Position = projection * view * worldPos;
}
