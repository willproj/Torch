#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;
uniform vec3 viewPos; // Provided from the vertex shader
uniform vec3 sunDir; // Sun direction (provided from the application)
uniform vec3 sunColor; // Sun color (optional, can be set dynamically if needed)

// Fixed properties
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
    vec3 ambient = ambientStrength * sunColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-sunDir); // Sun direction is the light direction
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * sunColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * sunColor;

    // Final color
    vec3 finalColor = (ambient + diffuse + specular) * Diffuse;
    FragColor = vec4(finalColor, 1.0);
}
