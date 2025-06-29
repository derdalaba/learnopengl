#version 330 core 
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;

uniform vec3 lightColor;

uniform sampler2D texture_diffuse1;

void main()
{
    FragColor = vec4(lightColor * texture(texture_diffuse1, TexCoords).rgb, 1.0);
}