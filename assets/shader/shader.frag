#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;
layout (location = 3) out int EntityColor;
// Note: gScattering is no longer here

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform int entity;

void main()
{    
    // Store fragment position in gbuffer
    gPosition = FragPos;
    
    // Store normals
    gNormal = normalize(Normal);
    
    // Diffuse color
    vec3 texColor = texture(texture_diffuse1, TexCoords).rgb;
    float texAlpha = texture(texture_diffuse1, TexCoords).a;
    //gColorSpec.rgb = mix(vec3(0.0f, 0.5f, 0.5f), texColor, texAlpha); // Use alpha as a blend factor
    gColorSpec.rgb = vec3(0.0f, 0.5f, 0.5f); // Use alpha as a blend factor

    // Specular intensity
    gColorSpec.a = texture(texture_specular1, TexCoords).r;

    // Entity ID
    EntityColor = entity;

    // gScattering is not output here
}
