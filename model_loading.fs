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
uniform bool parallaxMappingEnabled;

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

    vec3 normal = vec3(0.0);

    vec3 ao = vec3(1.0);

    vec3 diff = vec3(0.0);

    vec3 spec = vec3(0.0);
    vec3 mirror = vec3(0.0);
    
    float facSpec = pow(max(dot(viewDir, mirror), 0.0), 500);

    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    if (parallaxMappingEnabled) {
        vec3 tangentFragPos = fs_in.TBN * fs_in.FragPos;
        vec3 tangentViewPos = normalize(fs_in.TBN * viewPos - tangentFragPos);
        vec2 texCoords = ParallaxMapping(fs_in.TexCoords, tangentViewPos);
        normal = texture(texture_normal1, texCoords).rgb * 2.0 - 1.0;
        normal = normalize(fs_in.TBN * normal);

        ao = vec3(1.0) - texture(texture_ao1, texCoords).rrr;
        ao = ao * light.ambient;
        ao = ao * texture(texture_diffuse1, texCoords).rgb;

        diff = texture(texture_diffuse1, texCoords).rgb * light.diffuse;
        diff = diff * light.diffuse;
        diff = max(dot(normal, lightDir), 0.0) * diff + diff * light.ambient;

        spec = texture(texture_specular1, texCoords).rrr;
        mirror = reflect(-lightDir, normal);
        spec = spec * facSpec * light.specular * 1/pow(max(distance, 0.001), 0.5);

        spec = spec * (1.0 - texture(texture_roughness1, texCoords).r);
    

    } else {
        normal = texture(texture_normal1, fs_in.TexCoords).rgb * 2.0 - 1.0;
        normal = normalize(fs_in.TBN * normal);

        ao = vec3(1.0) - texture(texture_ao1, fs_in.TexCoords).rrr;
        ao = ao * light.ambient;
        ao = ao * texture(texture_diffuse1, fs_in.TexCoords).rgb;

        diff = texture(texture_diffuse1, fs_in.TexCoords).rgb * light.diffuse;
        diff = diff * light.diffuse;
        diff = max(dot(normal, lightDir), 0.0) * diff + diff * light.ambient;

        spec = texture(texture_specular1, fs_in.TexCoords).rrr;
        mirror = reflect(-lightDir, normal);
        
        spec = spec * facSpec * light.specular * 1/pow(max(distance, 0.001), 0.5);

        spec = spec * (1.0 - texture(texture_roughness1, fs_in.TexCoords).r);
    
    }
    
    vec3 result = (diff + ao + spec);
    FragColor = pow(vec4(result, 1.0), (1/vec4(gamma)));
    //FragColor = pow(vec4(texture(texture_height1, fs_in.TexCoords).rrr, 1.0), (1/vec4(gamma)));
}