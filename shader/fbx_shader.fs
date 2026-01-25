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

void main()
{    
    float distance = length(fs_in.FragPos - viewPos);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    
    

    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    
    vec3 normal = texture(texture_diffuse1, fs_in.TexCoords).rgb * 2.0 - 1.0;
    normal = normalize(fs_in.TBN * normal);

    vec3 ao = vec3(1.0);
    ao = ao * light.ambient;
    ao = ao * texture(texture_diffuse1, fs_in.TexCoords).rgb;

    vec3 diff = texture(texture_diffuse1, fs_in.TexCoords).rgb * light.diffuse;
    diff = diff * light.diffuse;
    diff = max(dot(normal, lightDir), 0.0) * diff + diff * light.ambient;

    vec3 spec = texture(texture_diffuse1, fs_in.TexCoords).rrr;
    vec3 mirror = reflect(-lightDir, normal);

    float facSpec = pow(max(dot(viewDir, mirror), 0.0), 500);    

    spec = spec * facSpec * light.specular * 1/pow(max(distance, 0.001), 0.5);

    spec = spec * (1.0 - texture(texture_diffuse1, fs_in.TexCoords).r);
    
    
    vec3 result = (diff);
    FragColor =vec4(texture(texture_diffuse1, vec2(fs_in.TexCoords.x, 1-fs_in.TexCoords.y)).rgb, 1.0);// pow(vec4(texture(texture_diffuse1, fs_in.TexCoords).rgb, 1.0), (1/vec4(gamma)));
    //FragColor = pow(vec4(texture(texture_height1, fs_in.TexCoords).rrr, 1.0), (1/vec4(gamma)));
}