#version 330 core
out vec4 FragColor; // Output color of the pixel

in vec2 TexCoord; // Received from vertex shader

uniform sampler2D textTexture0; // Texture uniform
uniform vec4 textColor; // Color to tint text

void main() {
    vec4 sampled = texture(textTexture0, TexCoord);
    FragColor = textColor * sampled; // Apply tint color and blend with texture
}
