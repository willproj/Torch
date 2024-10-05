#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;        // Already tone-mapped and gamma-corrected scene texture
uniform sampler2D bloomBlur;    // Blurred bloom texture

uniform float exposure = 6.0f;  // Exposure for tone mapping
uniform float bloomStrength = 2.0f; // Strength of bloom blending

// Function to perform inverse gamma correction
vec3 gammaCorrectInverse(vec3 color, float gamma) {
    return pow(color, vec3(gamma)); // Apply inverse gamma correction
}

vec3 applyBloom()
{
    // Fetch the scene color (already tone-mapped and gamma-corrected)
    vec3 toneMappedColor = texture(scene, TexCoords).rgb;  
    // Fetch bloom (blurred bright areas)
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;  

    // Apply inverse gamma correction to the tone-mapped color to get back to linear space
    vec3 linearColor = gammaCorrectInverse(toneMappedColor, 2.2); // Assuming gamma = 2.2

    // Add bloom effect in linear space
    // Bloom should be combined in linear space
    return linearColor * exposure + (bloomColor * bloomStrength);    
}

void main()
{
    vec3 result = applyBloom(); // Apply bloom to the linear color

    // Tone mapping applied to the final output (assuming it's already tone-mapped)
    // You can adjust this part based on your tone mapping method
    result = result / (result + vec3(1.0)); // Reinhard tone mapping

    // Gamma correction for final output
    const float gamma = 2.2; // Standard gamma value
    result = pow(result, vec3(gamma)); // Apply gamma correction

    FragColor = vec4(result, 1.0); // Output the final color
}
