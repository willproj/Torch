#version 330 core

out vec4 FragColor;

in vec3 TexCoords;

// Constants
const float PI = 3.14159265359;
const float RAY_TMIN = 0.001;
const float INVALID_INTERSECTION = -1.0;

const float EARTH_RADIUS = 6371000.0;
const vec3 EARTH_CENTER = vec3(0, -EARTH_RADIUS, 0);

const float ATMOSPHERE_HEIGHT = 100000.0;
const float RAYLEIGH_HEIGHT = 8000.0;
const float MIE_HEIGHT = 1200.0;

const vec3 RAYLEIGH_SCATTERING_COEF = vec3(5.802, 13.558, 33.1) * 1e-6;
const vec3 MIE_SCATTERING_COEF = vec3(3.996) * 1e-6;
const vec3 OZONE_ABSORPTION_COEF = vec3(3.426, 8.298, 0.356) * 0.1 * 1e-5;

const vec3 SUN_COLOR = vec3(1.0);

// Functions
float intersectSphere(vec3 org, vec3 dir, vec3 center, float radius)
{
    float b = dot(org - center, dir);
    float c = dot(org - center, org - center) - radius * radius;
    float D = b * b - c;
    if(D < 0.0) return INVALID_INTERSECTION;
    
    float t0 = -b - sqrt(D);
    float t1 = -b + sqrt(D);
    float t = t0;
    if(t < RAY_TMIN)
    {
        t = t1;
        if(t < RAY_TMIN) return INVALID_INTERSECTION;
    }
    
    return t;
}

float intersectEarth(vec3 org, vec3 dir)
{
    return intersectSphere(org, dir, EARTH_CENTER, EARTH_RADIUS);
}

float intersectAtmosphere(vec3 org, vec3 dir)
{
    return intersectSphere(org, dir, EARTH_CENTER, EARTH_RADIUS + ATMOSPHERE_HEIGHT);
}

float phaseRayleigh(float c)
{
    return 3.0 * (1.0 + c * c) / (16.0 * PI);
}


float phaseHG(float c, float g)
{
    float denom = 1.0 + g * g - 2.0 * g * c;
    return (1.0 - g * g) / (4.0 * PI * pow(denom, 1.5));
}

float phaseDraine(float c, float alpha, float g)
{
    return 1.0 / (4.0 * PI) * (1.0 - g * g) / pow(1.0 + g * g - 2.0 * g * c, 3.0 / 2.0) * (1.0 + alpha * c * c) / (1.0 + alpha * (1.0 + 2.0 * g * g) / 3.0);
}

float phaseMie(float c)
{
#if 0
    return phaseHG(c, 0.85);
#else
    float d = 5.0;
    float gHG = exp(-0.0990567 / (d - 1.67154));
    float gD = exp(-2.20679 / (d + 3.91029) - 0.428934);
    float alpha = exp(3.62489 - 8.29288 / (d + 5.52825));
    float wD = exp(-0.599085 / (d - 0.641583) - 0.665888);
    return (1.0 - wD) * phaseHG(c, gHG) + wD * phaseDraine(c, alpha, gD);
#endif
}

float atmosphereHeight(vec3 p)
{
    return distance(p, EARTH_CENTER) - EARTH_RADIUS;
}

float densityRayleigh(float h)
{
    return exp(-max(0.0, h / RAYLEIGH_HEIGHT));
}

float densityMie(float h)
{
    return exp(-max(0.0, h / MIE_HEIGHT));
}

float densityOzone(float h)
{
    return max(1.0 - abs(h - 25000.0) / 15000.0, 0.0);
}

vec3 scatteringRayleigh(float h)
{
    return RAYLEIGH_SCATTERING_COEF * densityRayleigh(h);
}

vec3 scatteringMie(float h)
{
    return MIE_SCATTERING_COEF * densityMie(h);
}

vec3 absorptionOzone(float h)
{
    return OZONE_ABSORPTION_COEF * densityOzone(h);
}

vec3 extinctionAtmosphere(float h)
{
    return scatteringRayleigh(h) + 1.11 * scatteringMie(h) + absorptionOzone(h);
}

vec3 transmittance(vec3 org, vec3 dir)
{
    float t = intersectAtmosphere(org, dir);
    int sampleCount = 20;
    float stepSize = t / float(sampleCount);
    
    vec3 tr = vec3(1.0);
    vec3 p = org;
    for(int i = 0; i < sampleCount; ++i)
    {
        float s = stepSize;
        p += s * dir;
        float h = atmosphereHeight(p);
        vec3 mu_e = extinctionAtmosphere(h);
        tr *= exp(-s * mu_e);
    }
    
    return tr;
}

vec3 atmosphere(vec3 org, vec3 dir, vec3 sunDir, vec3 sunColor, float distToSurface)
{
    float c = dot(dir, sunDir);
    float phaseR = phaseRayleigh(c);
    float phaseM = phaseMie(c);

    int sampleCount = 100;
    float stepSize = distToSurface / float(sampleCount);
    
    vec3 radiance = vec3(0.0);
    vec3 tr = vec3(1.0);
    vec3 p = org;
    vec3 tr_light = transmittance(org, sunDir);

    for(int i = 0; i < sampleCount; ++i)
    {
        float s = stepSize;
        p += s * dir;
        
        float h = atmosphereHeight(p);
        vec3 mu_e = extinctionAtmosphere(h);
        tr *= exp(-s * mu_e);
        
        radiance += tr * tr_light * phaseR * scatteringRayleigh(h) * s * sunColor;
        radiance += tr * tr_light * phaseM * scatteringMie(h) * s * sunColor;
    }
    
    return radiance;
}

vec3 ACESFilm(vec3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return (x * (a * x + b)) / (x * (c * x + d) + e);
}

uniform vec3 u_SunDir;

const float GAMMA = 2.8;
float SUN_ANGLE = 2.0 * PI / 180.0;

void main()
{
    // Normalize the TexCoords to get the direction vector for the skybox
    vec3 dir = normalize(TexCoords);
   
    // Initialize color
    vec3 color = vec3(0);

    // Perform intersection with atmosphere
    float t = intersectAtmosphere(vec3(0), dir);
    if(t != INVALID_INTERSECTION)
    {
        // Compute the atmosphere color
        color = atmosphere(vec3(0), dir, u_SunDir, SUN_COLOR, t);

        // Calculate the sun's contribution
        float c = acos(dot(dir, u_SunDir));
        if(c < SUN_ANGLE)
        {
            float h = c / SUN_ANGLE;
            color += max(1.0 - h, 0.0) * SUN_COLOR * transmittance(vec3(0), dir);
        }
    }

    // Apply post-processing effects
    float exposure = 8.0;
    color *= exposure;
    color = ACESFilm(color);
    color = pow(color, vec3(1.0 / GAMMA));

    // Output to screen
    FragColor = vec4(color, 1.0);
}

