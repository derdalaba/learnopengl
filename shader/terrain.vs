#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

out VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 TangentLightPos;

} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform Light light;
uniform vec3 viewPos;

uniform float heightScale;

uniform sampler2D texture_height1;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vec3 T = normalize(mat3(model) * aTangent);
    vec3 B = normalize(mat3(model) * aBiTangent);
    vec3 N = normalize(mat3(model) * aNormal);
    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentLightPos = TBN * light.position;

    vs_out.TexCoords = aTexCoords;   
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y + texture(texture_height1, aTexCoords).g * heightScale * 10.0f, aPos.z, 1.0);
}