#version 430 core

layout (location = 0) in vec3 aPos;            // Vertex position
layout (location = 1) in vec3 aNormal;         // Vertex normal
layout (location = 2) in vec2 aTexCoords;      // Texture coordinates

out vec3 FragPos;                               // Position in world space
out vec2 TexCoords;                             // Texture coordinates to pass to fragment shader
out vec3 Normal;                                // Normal vector in world space
out vec3 DisplacedPos;                          // Displaced position for G-buffer output

uniform mat4 model;                             // Model matrix
uniform mat4 view;                              // View matrix
uniform mat4 projection;                        // Projection matrix

uniform sampler2D u_DisplacementMap;           // Displacement map texture
uniform bool u_UseDisplacementMap;              // Flag to use displacement map
uniform float displacementScale = 0.1f;         // Scale for displacement amount (typically small)

void main()
{
    // Calculate world position from vertex position and model matrix
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = (view * worldPos).xyz;             // Transform to view space for lighting calculations
    TexCoords = aTexCoords;                      // Pass texture coordinates to the fragment shader

    // Calculate the normal in world space
    Normal = normalize(transpose(inverse(mat3(model))) * aNormal);  

    // Initialize variables for parallax mapping
    vec3 viewDir = normalize(-FragPos); // Direction from fragment to camera
    float height = 0.0;

    // Sample displacement if enabled
    if (u_UseDisplacementMap)
    {
        height = texture(u_DisplacementMap, aTexCoords).r * displacementScale; // Sample displacement
        // Offset position along the normal by the displacement height
        worldPos.xyz += normalize(Normal) * height;
    }

    // Output displaced position
    DisplacedPos = worldPos.xyz; // Store the displaced position for the G-buffer

    // Set the final vertex position for rendering
    gl_Position = projection * view * worldPos; // Keep the original position for G-buffer
}
