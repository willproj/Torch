#version 330 core

out vec4 FragColor;

uniform vec3 outlineColor; // Color of the outline

void main() {
    FragColor = vec4(outlineColor, 1.0);
}
