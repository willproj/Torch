#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec; // RGB = Albedo, A = Metallic
layout (location = 3) out vec3 gRoughAO; // R = Roughness, G = AO, B = unused or other data
layout (location = 4) out int EntityColor;

const float PI = 3.14159265359;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec3 WorldPos;

uniform int entity;

// Material texture maps
uniform sampler2D u_AlbedoMap;       // Albedo color texture
uniform sampler2D u_NormalMap;       // Normal map texture
uniform sampler2D u_MetallicMap;     // Metallic texture
uniform sampler2D u_RoughnessMap;    // Roughness texture
uniform sampler2D u_AoMap;           // Ambient occlusion texture

uniform float u_Metallic;
uniform float u_Roughness;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(u_NormalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{    
    // Store the fragment position in the G-buffer texture
    gPosition = WorldPos;

    // Fetch the normal from the normal map and store in G-buffer
    gNormal = getNormalFromMap();

    // Fetch the material properties from the textures
    vec3 albedo = pow(texture(u_AlbedoMap, TexCoords).rgb, vec3(3.2));
    float metallic = texture(u_MetallicMap, TexCoords).r;   // Metallic in red channel
    float roughness = texture(u_RoughnessMap, TexCoords).r; // Roughness in red channel
    float ao = texture(u_AoMap, TexCoords).r;               // AO in red channel

    // Store the albedo and metallic values in the gColorSpec buffer
    gAlbedoSpec.rgb = albedo;
    gAlbedoSpec.a = metallic;
    //gAlbedoSpec.a = u_Metallic;

    // Store roughness and AO in the gSpecRoughAO buffer
    gRoughAO.r = roughness;
    //gRoughAO.r = u_Roughness;
    gRoughAO.g = ao;

    //gRoughAO = vec3(1.0f,0.0f,1.0f);

    EntityColor = entity;
}
