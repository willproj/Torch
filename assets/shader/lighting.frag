#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;
uniform sampler2D scatteringTexture; // Scattering texture from the atmosphere pass
uniform vec3 viewPos; // Camera position in world space

// Fixed light properties
const vec3 lightPos = vec3(3.0f, 3.0f, 3.0f); // Position of the light source
const vec3 lightColor = vec3(1.0f, 1.0f, 1.0f); // White light
const float ambientStrength = 0.2f; // Ambient light strength
const float specularStrength = 0.3f; // Specular light strength
const float shininess = 100.0f; // Shininess factor for specular highlight

void main()
{             
    // Retrieve data from GBuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 Diffuse = texture(gColorSpec, TexCoords).rgb;
    float Specular = texture(gColorSpec, TexCoords).a;

    // Retrieve atmospheric scattering color from the scattering buffer
    vec4 scattering = texture(scatteringTexture, TexCoords);

    // Ambient lighting
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // Final lighting color (ambient + diffuse + specular)
    vec3 lightingColor = (ambient + diffuse + specular) * Diffuse;

    // Mix scattering with the final lighting color
    vec3 finalColor = mix(lightingColor, scattering.rgb, scattering.a);

    // Output the final color
    FragColor = vec4(lightingColor, 1.0);
}
