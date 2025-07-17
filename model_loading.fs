#version 330 core
out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
    mat3 TBN;
} fs_in;

uniform Light light;
uniform float gamma;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_ao1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_height1;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir) {
    float height = texture(texture_height1, texCoords).r;
    float scale = 0.05; // Adjust this value to control the parallax effect
    vec2 p = viewDir.xy * (height * scale);
    return texCoords + p;
}

void main()
{    
    float distance = length(fs_in.FragPos - viewPos);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec3 tangentFragPos = fs_in.TBN * fs_in.FragPos;
    vec3 tangentViewPos = normalize(fs_in.TBN * viewPos - tangentFragPos);

    vec2 texCoords = ParallaxMapping(fs_in.TexCoords, tangentViewPos);

    vec3 normal = texture(texture_normal1, texCoords).rgb * 2.0 - 1.0;
    normal = normalize(fs_in.TBN * normal);

    vec3 ao = vec3(1.0) - texture(texture_ao1, texCoords).rrr;
    ao = ao * light.ambient;
    ao = ao * texture(texture_diffuse1, texCoords).rgb;

    vec3 diff = texture(texture_diffuse1, texCoords).rgb * light.diffuse;
    diff = diff * light.diffuse;
    diff = max(dot(normal, lightDir), 0.0) * diff + diff * light.ambient;

    vec3 spec = texture(texture_specular1, texCoords).rrr;
    vec3 mirror = reflect(-lightDir, normal);
    float facSpec = pow(max(dot(viewDir, mirror), 0.0), 500);
    spec = spec * facSpec * light.specular * 1/pow(max(distance, 0.001), 0.5);

    spec = spec * (1.0 - texture(texture_roughness1, texCoords).r);
    
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    vec3 result = (diff + ao + spec);
    FragColor = pow(vec4(result, 1.0), (1/vec4(gamma)));
    //FragColor = pow(vec4(texture(texture_height1, fs_in.TexCoords).rrr, 1.0), (1/vec4(gamma)));
}