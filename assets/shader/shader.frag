#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;
layout (location = 3) out int EntityColor;


in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform int entity;


void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // and the diffuse per-fragment color
    vec3 texColor = texture(texture_diffuse1, TexCoords).rgb;
    float texAlpha = texture(texture_diffuse1, TexCoords).a;

    //gColorSpec.rgb = mix(vec3(1.0f,1.0f, 0.0f), texColor, texAlpha); //texture(texture_diffuse1, TexCoords).rgb;
    gColorSpec = vec4(1.0f,1.0f, 0.0f, 1.0f); //texture(texture_diffuse1, TexCoords).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    //gColorSpec.a = texture(texture_specular1, TexCoords).r;

    EntityColor = entity;
}