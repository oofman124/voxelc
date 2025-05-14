#version 330 core
in vec2 TexCoord;
in vec4 Color;

uniform sampler2D uiTexture;
uniform bool hasTexture;

out vec4 FragColor;

void main() {
    if(hasTexture) {
        vec4 texColor = texture(uiTexture, TexCoord);
        FragColor = texColor * Color;
    } else {
        FragColor = Color;
    }
}