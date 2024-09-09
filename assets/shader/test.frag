#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D tex;  // SSAO texture

void main()
{
    // Retrieve the SSAO factor from the SSAO texture
    vec3 ssao = texture(tex, TexCoords).rgb;

    // Display the SSAO as a grayscale value
    FragColor = vec4(ssao, 1.0);
}
