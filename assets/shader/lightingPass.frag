#version 430 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightnessMap;

layout(binding = 0, r32i) uniform readonly iimage2D gRedInt;

in vec2 TexCoords;

// G-buffer inputs
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;  // RGB = Albedo, A = Metallic
uniform sampler2D gRoughAO;     // R = Roughness, G = AO
uniform sampler2D gLightSpacePosition;
uniform sampler2D gDepth;

// Skybox
uniform sampler2D u_SkyboxMap;

// IBL
uniform samplerCube u_IrradianceMap;
uniform samplerCube u_PrefilterMap;
uniform sampler2D u_BrdfLUT;

// Light properties
uniform vec3 u_SunLightDir;
uniform vec3 u_SunLightColor;

// Shadow map 
uniform sampler2DArray u_ShadowMap;
uniform float u_FarPlane;
layout (std140) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};
uniform float u_CascadePlaneDistances[16];
uniform int u_CascadeCount;   // number of frusta - 1
uniform int u_TextureSize;

#define NUM_SAMPLES 25 //PCSS sample parameter in step 3
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES //PCSS sample parameter in step 1
#define EPS 1e-3

#define PI 3.141592653589793
#define PI2 6.283185307179586
#define NUM_RINGS 10

// SSAO
uniform sampler2D u_SSAO;

// Camera position
uniform vec3 u_CamPos;
uniform mat4 u_View;

// Debug parameters
uniform bool u_DebugBrightness = true;

/*******-------------------- PCSS functions --------------------******/

highp float rand_1to1(highp float x) {
	// -1 -1
	return fract(sin(x) * 10000.0);
}

highp float rand_2to1(vec2 uv) {
	// 0 - 1
	const highp float a = 12.9898, b = 78.233, c = 43758.5453;
	highp float dt = dot(uv.xy, vec2(a, b)), sn = mod(dt, PI);
	return fract(sin(sn) * c);
}


// poisson distribution
vec2 poissonDisk[NUM_SAMPLES];

void poissonDiskSamples(const in vec2 randomSeed) {

	float ANGLE_STEP = PI2 * float(NUM_RINGS) / float(NUM_SAMPLES);
	float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);

	float angle = rand_2to1(randomSeed) * PI2;
	float radius = INV_NUM_SAMPLES;
	float radiusStep = radius;

	for (int i = 0; i < NUM_SAMPLES; i++) {
		poissonDisk[i] = vec2(cos(angle), sin(angle)) * pow(radius, 0.75);
		radius += radiusStep;
		angle += ANGLE_STEP;
	}
}

void uniformDiskSamples(const in vec2 randomSeed) {

	float randNum = rand_2to1(randomSeed);
	float sampleX = rand_1to1(randNum);
	float sampleY = rand_1to1(sampleX);

	float angle = sampleX * PI2;
	float radius = sqrt(sampleY);

	for (int i = 0; i < NUM_SAMPLES; i++) {
		poissonDisk[i] = vec2(radius * cos(angle), radius * sin(angle));

		sampleX = rand_1to1(sampleY);
		sampleY = rand_1to1(sampleX);

		angle = sampleX * PI2;
		radius = sqrt(sampleY);
	}
}

// Shadow Map calculation
float ShadowCalculation(vec3 fragPosWorldSpace, vec3 N)
{
    // Select cascade layer
    vec4 fragPosViewSpace = u_View * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < u_CascadeCount; ++i)
    {
        if (depthValue < u_CascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = u_CascadeCount;
    }

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosWorldSpace, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    if (currentDepth > 1.0)
    {
        return 0.0;
    }

    vec3 normal = N;
    float bias = max(0.05 * (1.0 - dot(normal, u_SunLightDir)), 0.005);
    const float biasModifier = 0.5f;
    if (layer == u_CascadeCount)
    {
        bias *= 1 / (u_FarPlane * biasModifier);
    }
    else
    {
        bias *= 1 / (u_CascadePlaneDistances[layer] * biasModifier);
    }

    /***--------STEP 1: Blocker search: find the average blocker depth---------***/

	poissonDiskSamples(projCoords.xy); //sampled from poisson distribution
    const float u_LightSize = 200.0f;
	float sampleStride = u_LightSize/2.5; 
	float dBlocker = 0.0;
	float sampleSize = 1.0 / u_TextureSize * sampleStride;
	int blockerNumSample = BLOCKER_SEARCH_NUM_SAMPLES;

    float border = sampleStride / u_TextureSize;
	// just cut out the no padding area according to the sarched area size
    //if (projCoords.x <= border || projCoords.x >= 1.0f - border) {
	//	return 1.0;
	//}
	//if (projCoords.y <= border || projCoords.y >= 1.0f - border) {
	//	return 1.0;
	//}

	int count = 0;
	for (int i = 0; i < blockerNumSample; ++i) {
		vec2 sampleCoord = poissonDisk[i] * sampleSize + projCoords.xy;
		float closestDepth = texture(u_ShadowMap, vec3(sampleCoord, layer)).r;
		//Only compute average depth of blocker! not the average of the whole filter's area!
		if (closestDepth < currentDepth) {
			dBlocker += closestDepth;
			count++;
		}
	}
	
	dBlocker /= count;

	if (dBlocker < bias) {
		return 0.0;
	}
	if (dBlocker > 1.0) {
		return 1.0;
	}

    /***---------STEP 02: Penumbra estimation----------***/
	// estimation the filter size to control the softness

	poissonDiskSamples(projCoords.xy); // sampled from poisson distribution

	float lightWidth = u_LightSize/2.5;
	float wPenumbra = (currentDepth - dBlocker) * lightWidth / dBlocker;

	float filterStride = 5.0;
	float filterSize = 1.0 / u_TextureSize * filterStride * wPenumbra;

	/***--------STEP 03: Percentage Closer Filtering (PCF)---------***/

	float shadow = 0.0;
	int NumSample = NUM_SAMPLES;

	for (int i = 0; i < NumSample; ++i) {
		vec2 sampleCoord = poissonDisk[i] * filterSize + projCoords.xy;
		float pcfDepth = texture(u_ShadowMap, vec3(sampleCoord, layer)).r;
		shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
	}
	
	shadow /= NumSample;

	return shadow;
    // PCF with a larger kernel
    //float shadow = 0.0;
    //vec2 texelSize = 1.0 / vec2(textureSize(u_ShadowMap, 0).xy);
    //int kernelSize = 3; // Size of the kernel
    //for(int x = -kernelSize; x <= kernelSize; ++x)
    //{
    //    for(int y = -kernelSize; y <= kernelSize; ++y)
    //    {
    //        vec2 offset = vec2(x, y) * texelSize;
    //        float pcfDepth = texture(u_ShadowMap, vec3(projCoords.xy + offset, layer)).r;
    //        shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
    //    }
    //}
    //shadow /= float((2 * kernelSize + 1) * (2 * kernelSize + 1)); // Normalize
    //
    //return shadow;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 lightingCalculation(vec3 Lo, float shadow, vec3 lightDir, vec3 V, vec3 N, vec3 F0, float metallic, float roughness, vec3 albedo)
{
     // calculate per-light radiance
    vec3 L = normalize(lightDir);
    vec3 H = normalize(V + L);
    //float distance = length(lightPositions[i] - WorldPos);
    //float attenuation = 1.0 / (distance * distance);
    //vec3 radiance = lightColors[i] * attenuation;
    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
       
    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;
    
    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;	  
    
    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);
    
    // for directional light, no attenuation is needed
    vec3 radiance = u_SunLightColor; 
    
    
    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + specular)  * (1- shadow) * radiance * NdotL; 
    return Lo;
}

float sRGBToLuma(vec3 c)
{
    return dot(c, vec3(0.2126729, 0.7151522, 0.0721750));
}

void main()
{
    // Retrieve data from G-buffer
    vec3 WorldPos = texture(gPosition, TexCoords).rgb;
    vec3 N = normalize(texture(gNormal, TexCoords).rgb);
    vec4 gAlbedoSpecData = texture(gAlbedoSpec, TexCoords);
    vec3 albedo = gAlbedoSpecData.rgb;

    ivec2 texCoords = ivec2(gl_FragCoord.xy); 
    int value = imageLoad(gRedInt, texCoords).r;

    vec3 generalRenderersColor;
    vec3 skyboxColor;
    float isBackground;
    vec3 lightsColor;


    // Refactor: find the pure blocks to avoid the pbr lighting calculation entity 0 and 2 correctly is pure color blocks identitied as lights
    if(value == 200 || value == 120)
    {
        lightsColor = albedo;
        FragColor = vec4(lightsColor, 1.0); 
    }
    else
    {
        float metallic = gAlbedoSpecData.a;

        vec3 gRoughAOData = texture(gRoughAO, TexCoords).rgb;
        float roughness = gRoughAOData.r;
        float ao = gRoughAOData.g;

        // Retrieve SSAO value
        float ssao = texture(u_SSAO, TexCoords).r;

        // Calculate ambient occlusion factor
        float ambientOcclusion = ao * ssao;

        // View vector
        vec3 V = normalize(u_CamPos - WorldPos);
        vec3 R = reflect(-V, N);
        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, albedo, metallic);

        // Lighting calculations
        vec3 Lo = vec3(0.0);
        //vec4 fragPosLightSpace = texture(gLightSpacePosition, TexCoords);
        float shadow = ShadowCalculation(WorldPos, N); 
        Lo = lightingCalculation(Lo, shadow, u_SunLightDir, V, N, F0,metallic, roughness, albedo);

        // ambient lighting (we now use IBL as the ambient term)
        vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
        
        vec3 kS = F;
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 irradiance = texture(u_IrradianceMap, N).rgb;
        vec3 diffuse      = irradiance * albedo;

        // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureLod(u_PrefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
        vec2 brdf  = texture(u_BrdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

   
        
        vec3 ambient = (kD * diffuse + specular) * ambientOcclusion;
        generalRenderersColor = ambient + Lo;

        // HDR tonemapping
         //gamma correct
        generalRenderersColor = generalRenderersColor * (generalRenderersColor + vec3(1.0));
        generalRenderersColor = pow(generalRenderersColor, vec3(1.0/2.2)); 

        //FragColor = vec4(color, 1.0f);

         // Sample the 2D skybox texture for the background
        skyboxColor = texture(u_SkyboxMap, TexCoords).rgb;

        // Blend between skybox and lighting based on depth
        float depthValue = texture(gDepth, TexCoords).r;
        isBackground = step(0.9999, depthValue);  // 1.0 means far plane (background)
        // Interpolate between skybox and lighting
        vec3 finalColor = mix(generalRenderersColor, skyboxColor, isBackground);

        FragColor = vec4(finalColor, 1.0);
    }
    // for some reason that skybox color need to be enhanced to achieve the equalvelent intensity with the lightpass color
    vec3 brightnessColor = mix(generalRenderersColor + lightsColor, skyboxColor * 1.55, isBackground);
    //debug for brightness
    if(u_DebugBrightness)
    {
        if(sRGBToLuma(brightnessColor) > 1.5f)
        {
            //FragColor = vec4(0.0f,1.0f,1.0f, 1.0f);
            BrightnessMap = vec4(brightnessColor,1.0f);
        }
    }
}
