#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;        // HDR scene texture
uniform sampler2D bloomBlur;    // Blurred bloom texture

uniform float exposure = 2.0f;  // Exposure for tone mapping
uniform float bloomStrength = 1.0f; // Strength of bloom blending

vec3 applyBloom()
{
    vec3 hdrColor = texture(scene, TexCoords).rgb;    // Fetch scene color (HDR)
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;  // Fetch bloom (blurred bright areas)

    // Tone mapping (optional, can be adjusted or removed)
    hdrColor = hdrColor * exposure; // Adjust for exposure

    // Add bloom effect
    return hdrColor + bloomColor * bloomStrength;    // Add bloom with strength
}

void main()
{
    vec3 result = applyBloom();    // Apply bloom to the HDR color

    // Apply tone mapping and gamma correction
    // Optional: You may want to implement a more complex tone mapping here.
    // This example uses Reinhard tone mapping.
    result = result / (result + vec3(1.0)); // Reinhard tone mapping

    // Gamma correction (adjust gamma value as needed)
    const float gamma = 2.2; // Use a standard gamma value
    result = pow(result, vec3(1.0 / gamma)); // Apply gamma correction

    FragColor = vec4(result, 1.0);   // Output the final color
//    FragColor = vec4(texture(scene, TexCoords).rgb, 1.0);   // Output the final color
}
