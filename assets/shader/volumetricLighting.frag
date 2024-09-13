#version 330 core

// Uniforms
uniform sampler2D gPosition; // G-buffer texture containing world-space positions
uniform sampler3D lightVolumeTexture; // 3D texture for volumetric lighting
uniform vec3 viewPosition; // Camera position

// Inputs from the vertex shader
in vec2 TexCoords; // Texture coordinates from the vertex shader
out vec4 FragColor;

void main()
{
    // Retrieve the world-space position from the G-buffer
    vec3 position = texture(gPosition, TexCoords).rgb;

    // Compute texture coordinates for sampling the light volume
    // Adjust normalization based on your specific scale and range
    vec3 texCoord = (position - viewPosition) / 100.0; // Example normalization, adjust as needed

    // Sample the light volume texture
    vec4 lightSample = texture(lightVolumeTexture, texCoord);

    // Output the sampled color
    FragColor = lightSample;
}
