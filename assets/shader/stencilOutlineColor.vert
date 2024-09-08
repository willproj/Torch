#version 430 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;     // The model matrix (with the larger scale for outline)
uniform mat4 view;      // The view matrix
uniform mat4 projection; // The projection matrix

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0); // Transform the vertex
}
