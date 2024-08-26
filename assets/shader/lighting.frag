#version 330 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform isampler2D gEntityID;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;
uniform vec3 viewPos;

uniform int entity;

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gColorSpec, TexCoords).rgb;
    float Specular = texture(gColorSpec, TexCoords).a;
    
    FragColor = vec4(Normal, 1.0);

    int entityID = texture(gEntityID, TexCoords).r;
}