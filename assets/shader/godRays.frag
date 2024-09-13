#version 450 core

in vec2 TexCoords; // Texture coordinates from the vertex shader
out vec4 FragColor; // Final output color

uniform sampler2D gPosition;  // G-buffer position texture (world positions)
uniform sampler2D gDepth;     // G-buffer depth texture
uniform vec3 sunPosition;     // World-space position of the sun
uniform mat4 viewMatrix;      // View matrix of the camera
uniform mat4 projectionMatrix;// Projection matrix of the camera

// Parameters for god rays appearance
uniform float densityFactor;  // Density factor for god rays falloff
uniform float depthBias;      // Depth bias for occlusion checks
uniform float maxDistance;    // Maximum distance for ray marching
uniform float stepSize;       // Step size for ray marching
uniform vec3 sunColor;        // Color of the sun and god rays

void main() {
    // Fetch the world-space position of the current fragment from the G-buffer
    vec3 fragPosition = texture(gPosition, TexCoords).xyz;

    // Compute the direction of the ray from the fragment towards the sun
    vec3 rayDir = normalize(sunPosition - fragPosition);

    float totalLight = 0.0;

    // Iterate along the ray in world space
    for (float t = 0.0; t < maxDistance; t += stepSize) {
        // Sample the position along the ray
        vec3 samplePosition = fragPosition + rayDir * t;

        // Project the sample position into screen space
        vec4 samplePosScreenSpace = projectionMatrix * viewMatrix * vec4(samplePosition, 1.0);
        vec2 sampleUV = samplePosScreenSpace.xy / samplePosScreenSpace.w; // Convert to NDC

        // Check if sampleUV is within valid screen bounds
        if (sampleUV.x < 0.0 || sampleUV.x > 1.0 || sampleUV.y < 0.0 || sampleUV.y > 1.0) {
            continue; // Out of bounds, keep marching
        }

        // Sample the depth buffer (G-buffer) at the sample's screen space position
        float sceneDepth = texture(gDepth, sampleUV).r; // Scene depth at this point
        float sampleDepth = samplePosScreenSpace.z / samplePosScreenSpace.w; // Depth of sample

        // Check if the sample is occluded by scene geometry
        if (sampleDepth > sceneDepth + depthBias) {
            continue; // Occluded, stop accumulating light
        }

        // Compute density based on distance
        float density = exp(-densityFactor * t);

        // Accumulate light based on density
        totalLight += density;
    }

    // Calculate the final color of the god rays
    vec3 godRaysColor = sunColor * totalLight;

    // Output the final color
    FragColor = vec4(rayDir, 1.0);
}
