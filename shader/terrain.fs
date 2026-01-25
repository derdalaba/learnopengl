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
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 TangentLightPos;

} fs_in;

uniform Light light;
uniform float gamma;
uniform vec3 viewPos;

uniform sampler2D texture_height1;


void main()
{    
    float distance = length(fs_in.FragPos - viewPos);
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    vec3 normal = vec3(0.0);

    vec3 ao = vec3(1.0);

    vec3 diff = vec3(1.0);

    vec3 spec = vec3(0.1);
    vec3 mirror = vec3(0.0);
    
    float facSpec = pow(max(dot(viewDir, mirror), 0.0), 500);

    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec2 texCoords = fs_in.TexCoords;

    normal = vec3(0.0, 1.0, 0.1);
    //normal = normalize(transpose(fs_in.TBN) * normal);

    ao = vec3(1.0);
    ao = ao * light.ambient;
    //ao = ao * texture(texture_diffuse1, texCoords).rgb;

    diff = texture(texture_height1, texCoords).rrr * light.diffuse;
    diff = diff * light.diffuse;
    diff = max(dot(normal, lightDir), 0.0) * diff + diff * light.ambient;

    
    mirror = reflect(-lightDir, normal);
    spec = spec * facSpec * light.specular * 1/pow(max(distance, 0.001), 0.5);
    
    vec3 result = (diff + ao + spec);
    FragColor = pow(vec4(result, 1.0), (1/vec4(gamma)));
    //FragColor = pow(vec4(texture(texture_height1, fs_in.TexCoords).rrr, 1.0), (1/vec4(gamma)));
}