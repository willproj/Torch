#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;
uniform vec3 viewPos; // Make sure this is provided from the vertex shader

// Fixed light properties
const vec3 lightPos = vec3(3.0f, 3.0f, 3.0f); // Position of the light source
const vec3 lightColor = vec3(1.0f, 1.0f, 1.0f); // White light
const float ambientStrength = 0.2f; // Ambient light strength
const float specularStrength = 0.3f; // Specular light strength
const float shininess = 100.0f; // Shininess factor for specular highlight

void main()
{             
    // Retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 Diffuse = texture(gColorSpec, TexCoords).rgb;
    float Specular = texture(gColorSpec, TexCoords).a;

    // Ambient
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // Final color
    vec3 finalColor = (ambient + diffuse + specular) * Diffuse;
    FragColor = vec4(finalColor, 1.0);
}
