#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

void main()
{
    // Set output color (pure red for now)
    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
