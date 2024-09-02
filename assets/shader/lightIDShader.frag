#version 430 core
layout (location = 0) out int EntityColor;

out vec4 FragColor;

uniform int entity;

void main()
{
    FragColor = vec4(1.0f,0.0f,0.0f, 1.0);
    EntityColor = entity;
}
