#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; // For basic lighting

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal; // For basic lighting
out vec3 FragmentPos; // For basic lighting

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragmentPos = vec3(model * vec4(aPos, 1.0)); // For basic lighting
    Normal = aNormal; // For basic lighting
}