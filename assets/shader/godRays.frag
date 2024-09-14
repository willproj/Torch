#version 450 core

in vec2 TexCoords; // Texture coordinates from the vertex shader
out vec4 FragColor; // Final output color

uniform sampler2D gPosition;  // G-buffer position texture (world positions)
uniform sampler2D gDepth;     // G-buffer depth texture
uniform vec3 sunPosition;     // World-space position of the sun
uniform mat4 view;            // View matrix of the camera
uniform mat4 projection;      // Projection matrix of the camera

// Parameters for god rays appearance
uniform float densityFactor;  // Density factor for god rays falloff
uniform float depthBias;      // Depth bias for occlusion checks
uniform vec3 sunColor;        // Color of the sun and god rays

uniform vec3 camPos;          // Camera position

void main() {
    // Fetch the world-space position of the current fragment from the G-buffer
    vec3 fragPosition = texture(gPosition, TexCoords).xyz;

    // Compute the direction of the ray from the fragment towards the sun
    vec3 rayDir = normalize(sunPosition - fragPosition);

    // Initialize ray marching parameters
    vec3 rayColor = vec3(0.0);
    const float maxDistance = 20.0;  // Maximum distance for ray marching
    float stepSize = 0.5;            // Step size for ray marching

    for (float t = 0.0; t < maxDistance; t += stepSize) {
        // Step forward along the ray
        vec3 currentPos = fragPosition + t * rayDir;

        // Convert the world-space position to screen-space position
        vec4 clipSpacePos = projection * view * vec4(currentPos, 1.0);
        vec3 ndcPos = clipSpacePos.xyz / clipSpacePos.w;
        vec2 screenPos = (ndcPos.xy + 1.0) * 0.5;  // Convert NDC to screen coordinates

        // Fetch the depth at the current position
        float depthAtCurrentPos = texture(gDepth, TexCoords).r;
//        FragColor = vec4(vec3(depthAtCurrentPos), 1.0);

        // Compute light scattering based on distance and occlusion
        float distance = length(currentPos - fragPosition);
        float attenuation = exp(-densityFactor * distance);
        float occlusion = smoothstep(0.0, 1.0, (depthAtCurrentPos - depthBias) / maxDistance);
//        if(t==6)
//        {
//            FragColor = vec4(vec3(occlusion), 1.0);
//            return;
//        }
//
        vec3 scatteredLight = sunColor * occlusion;

        // Accumulate the color
        rayColor += scatteredLight * stepSize;
    }

    // Output the final color
        FragColor = vec4(rayColor, 1.0);

}
