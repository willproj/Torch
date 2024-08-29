#version 450 core

out vec4 FragColor;

in vec3 TexCoords; // Normalized texture coordinates for the skybox

uniform vec3 SUN_WORLD_POSITION; // Dynamic sun position
uniform mat4 view;               // View matrix
uniform mat4 projection;         // Projection matrix

#define CAMERA_HEIGHT 1.000001
#define INV_WAVE_LENGTH vec3(5.60204474633241, 9.4732844379203038, 19.643802610477206)
#define INNER_RADIUS 1.0
#define OUTER_RADIUS 1.025
#define ESUN 10.0
#define KR 0.0025
#define KM 0.0015
#define SCALE_DEPTH 0.25
#define SAMPLES 2
#define G -0.99
#define GROUND_COLOR vec3(0.15, 0.20, 0.30) // Darker ground color
#define MORNING_COLOR vec3(1.0, 0.5, 0.2) // Brighter morning color
#define SUNSET_COLOR vec3(1.0, 0.6, 0.3) // Warm sunset color
#define GAMMA 1.0 / 2.2
#define TRANSITION_HEIGHT 0.005 // Adjust for gradual transition
#define TRANSITION_WIDTH 0.02   // Adjust for the width of the transition

#define PI 3.14159265

// Helper functions
float scale(float fCos) {
    float x = 1.0 - fCos;
    return SCALE_DEPTH * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

vec3 uvToRayDir(vec2 uv) {
    vec2 v = PI * (vec2(1.5, 1.0) - vec2(2.0, 1.0) * uv);
    return vec3(
        sin(v.y) * cos(v.x),
        cos(v.y),
        sin(v.y) * sin(v.x)
    );
}

vec2 getIntersections(vec3 pos, vec3 dir, float dist2, float rad2) {
    float B = 2.0 * dot(pos, dir);
    float C = dist2 - rad2;
    float det = max(0.0, B * B - 4.0 * C);
    return 0.5 * vec2(
        (-B - sqrt(det)),
        (-B + sqrt(det))
    );
}

float getMiePhase(float fCos, float fCos2, float g, float g2) {
    return 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos2) / pow(1.0 + g2 - 2.0 * g * fCos, 1.5);
}

float getRayleighPhase(float fCos2) {
    return 0.75 * (2.0 + 0.5 * fCos2);
}

// Main function
void main() {
    vec3 rayDir = normalize(TexCoords); // Direction of the ray for the skybox

    // Rayleigh and Mie scattering calculations
    float fInnerRadius2 = INNER_RADIUS * INNER_RADIUS;
    float fOuterRadius2 = OUTER_RADIUS * OUTER_RADIUS;
    float fKrESun = KR * ESUN;
    float fKmESun = KM * ESUN;
    float fKr4PI = KR * 4.0 * PI;
    float fKm4PI = KM * 4.0 * PI;
    float fScale = 1.0 / (OUTER_RADIUS - INNER_RADIUS);
    float fScaleOverScaleDepth = fScale / SCALE_DEPTH;
    float fG2 = G * G;

    vec3 lightDir = normalize(SUN_WORLD_POSITION);

    // Initialize ray parameters
    vec3 rayOrigin = vec3(0.0, CAMERA_HEIGHT, 0.0);
    vec3 rayDirNormalized = normalize(rayDir);
    float fCameraHeight = length(rayOrigin);
    float fCameraHeight2 = fCameraHeight * fCameraHeight;

    // Get intersections with the atmosphere
    vec2 innerIntersections = getIntersections(rayOrigin, rayDirNormalized, fCameraHeight2, fInnerRadius2);
    vec2 outerIntersections = getIntersections(rayOrigin, rayDirNormalized, fCameraHeight2, fOuterRadius2);
    bool isGround = 0.0 < innerIntersections.x;

    if (outerIntersections.x == outerIntersections.y) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // No intersection with atmosphere
        return;
    }

    float near = max(0.0, outerIntersections.x);
    float far = isGround ? innerIntersections.x : outerIntersections.y;
    vec3 farPos = rayOrigin + rayDirNormalized * far;
    vec3 farPosNorm = normalize(farPos);

    vec3 startPos = rayOrigin + rayDirNormalized * near;
    float startPosHeight = length(startPos);
    vec3 startPosNorm = startPos / startPosHeight;
    float startAngle = dot(rayDirNormalized, startPosNorm);
    float startDepth = exp(fScaleOverScaleDepth * (INNER_RADIUS - startPosHeight));
    float startOffset = startDepth * scale(startAngle);

    float cameraAngle = dot(-rayDirNormalized, farPosNorm);
    float cameraScale = scale(cameraAngle);
    float cameraOffset = exp((INNER_RADIUS - fCameraHeight) / SCALE_DEPTH) * cameraScale;

    float temp = scale(dot(farPosNorm, lightDir)) + scale(dot(farPosNorm, -rayDirNormalized));

    float sampleLength = (far - near) / float(SAMPLES);
    float scaledLength = sampleLength * fScale;
    vec3 sampleDir = rayDirNormalized * sampleLength;
    vec3 samplePoint = startPos + sampleDir * 0.5;

    vec3 frontColor = vec3(0.0);
    vec3 attenuate;
    for (int i = 0; i < SAMPLES; i++) {
        float height = length(samplePoint);
        float depth = exp(fScaleOverScaleDepth * (INNER_RADIUS - height));
        float lightAngle = dot(lightDir, samplePoint) / height;
        float cameraAngle = dot(rayDirNormalized, samplePoint) / height;
        float scatter = isGround
            ? depth * temp - cameraOffset
            : (startOffset + depth * (scale(lightAngle) - scale(cameraAngle)));
        attenuate = exp(-scatter * (INV_WAVE_LENGTH * fKr4PI + fKm4PI));
        frontColor += attenuate * (depth * scaledLength);
        samplePoint += sampleDir;
    }

    frontColor = clamp(frontColor, 0.0, 3.0);
    vec3 c0 = frontColor * (INV_WAVE_LENGTH * fKrESun);
    vec3 c1 = frontColor * fKmESun;

    // Smooth blending between ground and sky
    float heightFactor = (fCameraHeight - INNER_RADIUS) / (OUTER_RADIUS - INNER_RADIUS);
    float blendFactor = smoothstep(0.0, TRANSITION_HEIGHT, heightFactor);
    blendFactor = smoothstep(0.0, 1.0, blendFactor); // Apply a smooth step to the blend factor
    
    if (isGround) {
        vec3 rayleighColor = c0 + c1;
        vec3 mieColor = clamp(attenuate, 0.0, 3.0);
        vec3 finalColor = 1.0 - exp(-(rayleighColor + GROUND_COLOR * mieColor));
        // Blend between sky color and ground color
        vec3 sunPositionNormalized = normalize(SUN_WORLD_POSITION);
        float sunIntensity = max(0.0, dot(sunPositionNormalized, vec3(0.0, 1.0, 0.0)));
        vec3 skyColor = mix(MORNING_COLOR, SUNSET_COLOR, sunIntensity);
        FragColor = vec4(mix(finalColor, skyColor, blendFactor), 1.0);
    } else {
        float cosTheta = dot(-lightDir, rayDirNormalized);
        float cosTheta2 = cosTheta * cosTheta;
        FragColor = vec4(
            getRayleighPhase(cosTheta2) * c0 + getMiePhase(cosTheta, cosTheta2, G, fG2) * c1,
            1.0
        );
    }
    FragColor.xyz = pow(FragColor.xyz, vec3(GAMMA));
}
