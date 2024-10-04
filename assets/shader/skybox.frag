#version 430 core
layout(std430, binding = 0) buffer SSBO {
    vec3 finalColor;
};

out vec4 FragColor;

in vec3 TexCoords;

// Constants
const float PI = 3.14159265359;
const float INVALID_INTERSECTION = -1.0;


// uniforms
uniform float u_RayTMin;
uniform float u_EarthRadius;
uniform float u_AtmosphereHeight;
uniform float u_RayleighHeight;
uniform float u_MieHeight;
uniform vec3 u_RayleighScatteringCoef;
uniform vec3 u_MieScatteringCoef;
uniform vec3 u_OzoneAbsorptionCoef;
uniform vec3 u_SunPosition;
uniform float u_SunAngle;
uniform vec3 u_SunColor;
uniform float u_SunIntensity;

const float GAMMA = 2.8;
vec3 EARTH_CENTER = vec3(0, -u_EarthRadius, 0);

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
    if(t < u_RayTMin)
    {
        t = t1;
        if(t < u_RayTMin) return INVALID_INTERSECTION;
    }
    
    return t;
}

float intersectEarth(vec3 org, vec3 dir)
{
    return intersectSphere(org, dir, EARTH_CENTER, u_EarthRadius);
}

float intersectAtmosphere(vec3 org, vec3 dir)
{
    return intersectSphere(org, dir, EARTH_CENTER, u_EarthRadius + u_AtmosphereHeight);
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
    return distance(p, EARTH_CENTER) - u_EarthRadius;
}

float densityRayleigh(float h)
{
    return exp(-max(0.0, h / u_RayleighHeight));
}

float densityMie(float h)
{
    return exp(-max(0.0, h / u_MieHeight));
}

float densityOzone(float h)
{
    return max(1.0 - abs(h - 25000.0) / 15000.0, 0.0);
}

vec3 scatteringRayleigh(float h)
{
    return u_RayleighScatteringCoef * densityRayleigh(h);
}

vec3 scatteringMie(float h)
{
    return u_MieScatteringCoef * densityMie(h);
}

vec3 absorptionOzone(float h)
{
    return u_OzoneAbsorptionCoef * densityOzone(h);
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
        color = atmosphere(vec3(0), dir, u_SunPosition, u_SunColor * u_SunIntensity, t);

        // Calculate the sun's contribution
        float c = acos(dot(dir, u_SunPosition));
        if(c < u_SunAngle)
        {
            float h = c / u_SunAngle;
            color += max(1.0 - h, 0.0) * u_SunColor * u_SunIntensity * transmittance(vec3(0), dir);
        }

    }

    // Apply post-processing effects
    float exposure = 8.0;
    color *= exposure;
    color = ACESFilm(color);

    color = pow(color, vec3(1.0 / 2.8));

    // Output to screen
    FragColor = vec4(color, 1.0);


    //color blended sun color
    // Step 1: Compute sun color at the position
    vec3 sunColorAtPosition = vec3(0.0);
    float t_sun = intersectAtmosphere(vec3(0), normalize(u_SunPosition));
    if (t_sun != INVALID_INTERSECTION) {
        sunColorAtPosition = atmosphere(vec3(0), normalize(u_SunPosition), u_SunPosition, u_SunColor, t_sun);
    }
    
    // Normalize the sun color
    sunColorAtPosition = pow(sunColorAtPosition, vec3(1.0 / GAMMA));
    float totalSum = sunColorAtPosition.r + sunColorAtPosition.g + sunColorAtPosition.b;
    vec3 normalizedSunColor = sunColorAtPosition / totalSum;
    
    float sunElevation = dot(normalize(u_SunPosition), vec3(0.0, 1.0, 0.0)); // Dot product with the vertical direction
    sunElevation = clamp(sunElevation, -1.0, 1.0);
    float intensityMultiplier = mix(0.5, 1.0, (sunElevation + 1.0) * 0.5); // Linear interpolation between 0.5 and 1.0
    vec3 scaledSunColor = u_SunColor * intensityMultiplier;
    float totalIntensity = scaledSunColor.r + scaledSunColor.g + scaledSunColor.b;
    
    vec3 finalSunColor = vec3(
        totalIntensity * normalizedSunColor.r,
        totalIntensity * normalizedSunColor.g,
        totalIntensity * normalizedSunColor.b
    );


    finalColor = finalSunColor;
}

