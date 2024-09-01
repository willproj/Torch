#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 FragColor;

void main()
{
    vec4 pos = model * vec4(aPos, 1.0);
    vec3 norm = normalize(mat3(transpose(inverse(model))) * aPos);
    vec4 expandedPos = pos + vec4(2.0 * norm, 0.0); // Larger expansion for testing
    gl_Position = projection * view * expandedPos;
}
