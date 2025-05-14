#version 330 core
layout (location = 0) in vec2 aPos;    // x, y
layout (location = 1) in vec2 aTexCoord;// u, v
layout (location = 2) in vec4 aColor;   // r, g, b, a

uniform mat4 projection;

out vec2 TexCoord;
out vec4 Color;

void main() {
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
    Color = aColor;
}