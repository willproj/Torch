#version 430 core
layout (location = 0) out int EntityColor;
layout (location = 1) out vec4 ColorAttchment;

out vec4 FragColor;

uniform int entity;

void main()
{
    FragColor = vec4(1.0f,10.0f,10.0f, 1.0);
    EntityColor = entity;
    ColorAttchment = FragColor;
}
