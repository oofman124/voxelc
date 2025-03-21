#version 330 core
out vec4 FragColor;
 
in vec2 TexCoord;

uniform sampler2D texture0;
uniform sampler2D normalMap;
uniform float normalStrength = 0.2;

void main()
{
    // FragColor = mix(texture(texture0, TexCoord), texture(normalMap, TexCoord), normalStrength);
    FragColor = texture(texture0, TexCoord);
}