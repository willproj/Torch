#version 450 core

out vec4 FragColor;

in vec3 TexCoords; // Texture coordinates for the skybox

const float R0 = 6360e3; // Earth surface radius
const float Ra = 6390e3; // Earth atmosphere top radius
const vec3 bR = vec3(58e-7, 135e-7, 331e-7); // Rayleigh scattering coefficient
const vec3 bMs = vec3(2e-5); // Mie scattering coefficients
const vec3 bMe = bMs * 1.1;
const float I = 20.; // Increased Sun intensity
const vec3 C = vec3(0., -R0, 0.); // Earth center point

uniform vec3 u_SunDirection; // Sun direction uniform

// Calculate densities
vec2 densitiesRM(vec3 p) {
    float h = max(0., length(p - C) - R0); // height from Earth surface
    return vec2(exp(-h/8e3), exp(-h/12e2));
}

// Find distance to where rays escape a sphere
float escape(vec3 p, vec3 d, float R) {
    vec3 v = p - C;
    float b = dot(v, d);
    float det = b * b - dot(v, v) + R * R;
    if (det < 0.) return -1.;
    det = sqrt(det);
    float t1 = -b - det, t2 = -b + det;
    return (t1 >= 0.) ? t1 : t2;
}

// Calculate density integral for optical depth
vec2 scatterDepthInt(vec3 o, vec3 d, float L, float steps) {
    vec2 depthRMs = vec2(0.);
    L /= steps; d *= L;
    for (float i = 0.; i < steps; ++i)
        depthRMs += densitiesRM(o + d * i);
    return depthRMs * L;
}

// Global variables
vec2 totalDepthRM;
vec3 I_R, I_M;

// Calculate in-scattering for a ray
void scatterIn(vec3 o, vec3 d, float L, float steps) {
    L /= steps; d *= L;
    for (float i = 0.; i < steps; ++i) {
        vec3 p = o + d * i;
        vec2 dRM = densitiesRM(p) * L;
        totalDepthRM += dRM;
        vec2 depthRMsum = totalDepthRM + scatterDepthInt(p, u_SunDirection, escape(p, u_SunDirection, Ra), 4.);
        vec3 A = exp(-bR * depthRMsum.x - bMe * depthRMsum.y);
        I_R += A * dRM.x;
        I_M += A * dRM.y;
    }
}

// Final scattering function
vec3 scatter(vec3 o, vec3 d, float L, vec3 Lo) {
    totalDepthRM = vec2(0.);
    I_R = I_M = vec3(0.);
    scatterIn(o, d, L, 16.);
    float mu = dot(d, u_SunDirection);
    return Lo * exp(-bR * totalDepthRM.x - bMe * totalDepthRM.y)
        + I * (1. + mu * mu) * (
            I_R * bR * .0597 + // Reduced emission light around the sun
            I_M * bMs * .0196 / pow(1.58 - 1.52 * mu, 1.5)); // Reduced emission light around the sun
}
const float gamma = 2.2;
void main() {
    vec3 O = vec3(0., 0., 0.); // Camera position at the center of the Earth
    vec3 D = normalize(TexCoords); // Direction from the camera

    vec3 col = vec3(0.);
    float L = escape(O, D, Ra);
    col = scatter(O, D, L, col);
    col = pow(col, vec3(1.0 / gamma));

    FragColor = vec4(sqrt(col), 1.0); // Output the final color
}
