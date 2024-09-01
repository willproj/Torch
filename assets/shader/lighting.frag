#version 330 core
#define RENDER_ALL 0
#define RENDER_GCOLOR 1
#define RENDER_GPOSITION 2
#define RENDER_GNORMAL 3
#define RENDER_GDEPTH 4

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;
uniform sampler2D gDepth;

uniform vec3 viewPos; 
uniform vec3 sunDir; 
uniform vec3 sunColor; 

const float ambientStrength = 0.2f;
const float specularStrength = 0.3f;
const float shininess = 100.0f;

uniform int u_RenderType;


void main()
{             
    // Retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 Diffuse = texture(gColorSpec, TexCoords).rgb;
    float Specular = texture(gColorSpec, TexCoords).a;

    // Ambient
    vec3 ambient = ambientStrength * sunColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-sunDir); 
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * sunColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * sunColor;

    // Final color calculation
    vec3 finalColor = (ambient + diffuse + specular) * Diffuse;
    
    // Output final color
    if(u_RenderType == RENDER_ALL)
    {
        FragColor = vec4(finalColor, 1.0);
    }
    else if(u_RenderType == RENDER_GCOLOR)
    {
        FragColor = vec4(Diffuse, 1.0);
    }
    else if(u_RenderType == RENDER_GPOSITION)
    {
        FragColor = vec4(FragPos, 1.0);
    }
    else if(u_RenderType == RENDER_GNORMAL)
    {
        FragColor = vec4(Normal, 1.0);
    }
    else if(u_RenderType == RENDER_GDEPTH)
    {
        FragColor = vec4(texture(gDepth, TexCoords).rgb, 1.0);
    }
}
