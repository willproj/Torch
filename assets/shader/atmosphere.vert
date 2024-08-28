#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal; // Optional, if needed for additional calculations
layout(location = 2) in vec2 texCoord; // Optional, if using texture coordinates

out vec3 fsPosition;
out vec3 fsNormal;  // Optional, if normals are used
out vec2 fsTexCoord; // Optional, if texture coordinates are used

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Calculate world position (although it won't affect the final position of the skybox)
    vec4 worldPosition = model * vec4(position, 1.0);
    fsPosition = worldPosition.xyz;

    // Remove translation from the view matrix to keep the skybox centered around the camera
    mat4 viewNoTranslation = mat4(mat3(view)); // Remove the translation component

    // Compute the final position in clip space
    gl_Position = projection * viewNoTranslation * vec4(position, 1.0);

    // Compute normals if needed (for additional effects)
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    fsNormal = normalize(normalMatrix * normal);

    // Pass texture coordinates if needed
    fsTexCoord = texCoord;
}
