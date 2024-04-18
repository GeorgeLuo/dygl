#version 330 core
uniform vec4 ourColor; // Uniform input
out vec4 FragColor;

void main() {
    FragColor = ourColor; // Use the uniform color
}