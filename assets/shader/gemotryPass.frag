#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec; // RGB = Albedo, A = Metallic
layout (location = 3) out vec3 gRoughAO; // R = Roughness, G = AO, B = unused or other data
layout (location = 4) out int EntityColor;
layout (location = 5) out vec4 gLightSpacePosition;
layout (location = 6) out vec3 gViewPosition;

const float PI = 3.14159265359;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec3 WorldPos;
//in vec4 FragPosLightSpace;

uniform int entity;

// Material texture maps
uniform sampler2D u_AlbedoMap;       // Albedo color texture
uniform sampler2D u_NormalMap;       // Normal map texture
uniform sampler2D u_MetallicMap;     // Metallic texture
uniform sampler2D u_RoughnessMap;    // Roughness texture
uniform sampler2D u_AoMap;           // Ambient occlusion texture

// Material usage
uniform bool u_UseAlbedoMap;
uniform bool u_UseNormalMap;
uniform bool u_UseMetallicMap;
uniform bool u_UseRoughnessMap;
uniform bool u_UseAoMap;

uniform float u_Metallic;
uniform float u_Roughness;


vec3 getNormalFromMap()
{
    // Check if a normal map is being used
    if(u_UseNormalMap)
    {
        vec3 tangentNormal = texture(u_NormalMap, TexCoords).xyz * 2.0 - 1.0;  

        vec3 Q1  = dFdx(WorldPos);
        vec3 Q2  = dFdy(WorldPos);
        vec2 st1 = dFdx(TexCoords);
        vec2 st2 = dFdy(TexCoords);
    
        vec3 N   = normalize(Normal);
        vec3 T   = normalize(Q1 * st2.t - Q2 * st1.t);
        vec3 B   = -normalize(cross(N, T));
        mat3 TBN = mat3(T, B, N);
    
        return normalize(TBN * tangentNormal);
    }
    else
    {
        return Normal;
    }
}


void main()
{    
    //gLightSpacePosition = FragPosLightSpace;
    gViewPosition = FragPos;
    // Store the fragment position in the G-buffer texture
    gPosition = WorldPos;
    
    // Fetch the normal from the normal map and store in G-buffer
    gNormal = getNormalFromMap();
    
    // Set defaults for material properties if no textures are bound
    vec3 albedo = u_UseAlbedoMap ? pow(texture(u_AlbedoMap, TexCoords).rgb, vec3(2.0)) : vec3(1.0f,1.0,1.0f);
    float metallic = u_UseMetallicMap ? texture(u_MetallicMap, TexCoords).r * u_Metallic: u_Metallic;
    float roughness = u_UseRoughnessMap ? texture(u_RoughnessMap, TexCoords).r * u_Roughness : u_Roughness;
    float ao = u_UseAoMap ? texture(u_AoMap, TexCoords).r : 1.0f;
    // Store the albedo and metallic values in the gAlbedoSpec buffer
    gAlbedoSpec.rgb = albedo;
    gAlbedoSpec.a = metallic;

    // Store roughness and AO in the gRoughAO buffer
    gRoughAO.r = roughness;
    gRoughAO.g = ao;

    // Pass the entity ID for identification in the G-buffer
    EntityColor = entity;

}
