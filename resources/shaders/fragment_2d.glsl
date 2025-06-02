#version 330 core
in vec2 fragUV;
in vec4 fragColor;

uniform sampler2D texture0;
uniform int useTexture;

out vec4 outColor;

void main() {
    if (useTexture == 1)
        outColor = texture(texture0, fragUV) * fragColor;
    else
        outColor = fragColor;
}