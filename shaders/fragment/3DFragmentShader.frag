#version 330 core
out vec4 FragColor;

in vec3 Normal; // For basic lighting
in vec3 FragmentPos; // For basic lighting

uniform vec4 ourColor; // Updated to vec4
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    // Ambient lighting
    float ambientStrength = 0.75;
    vec3 ambient = ambientStrength * lightColor;
  
    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragmentPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor; 
    
    vec3 result = (ambient + diffuse) * ourColor.rgb; // Use .rgb to multiply with vec3 light components
    FragColor = vec4(result, ourColor.a); // Use ourColor's alpha for the final color
}
