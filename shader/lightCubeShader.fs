#version 330 core 
out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
    mat3 TBN;
} fs_in;

uniform vec3 lightColor;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;


vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir) {
    float height = texture(texture_diffuse1, texCoords).r;
    float scale = 0.05; // Adjust this value to control the parallax effect
    vec2 p = viewDir.xy * (height * scale);
    return texCoords + p;
}

void main()
{
    vec3 tangentFragPos = fs_in.TBN * fs_in.FragPos;
    vec3 tangentViewPos = normalize(fs_in.TBN * viewPos - tangentFragPos);

    //vec2 texCoords = ParallaxMapping(fs_in.TexCoords, normalize(tangentViewPos));
    FragColor = vec4(lightColor * texture(texture_diffuse1, fs_in.TexCoords).rgb, 1.0);
}