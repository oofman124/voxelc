#version 330 core
out vec4 FragColor;
 
in vec2 TexCoord;
in float Distance;

uniform sampler2D texture0;
uniform sampler2D normalMap;
uniform float normalStrength = 0.2;
uniform vec3 SkyColor = vec3(186.0f / 255.0f, 214.0f / 255.0f, 254.0f / 255.0f);
uniform float fogMax = 2000.0f;

void main()
{
    // FragColor = mix(texture(texture0, TexCoord), texture(normalMap, TexCoord), normalStrength);
    vec4 texColor = texture(texture0, TexCoord);
    if(texColor.a < 0.1)
        discard;


    FragColor = vec4(mix(texColor.xyz, SkyColor, min(1.0f, Distance / fogMax)), texColor.w);
}