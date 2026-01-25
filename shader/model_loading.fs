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
uniform bool parallaxMappingEnabled;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_ao1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_height1;

uniform float heightScale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(texture_height1, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(texture_height1, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = ( texture(texture_height1, prevTexCoords).r) - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void main()
{    
    float distance = length(fs_in.FragPos - viewPos);
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    vec3 normal = vec3(0.0);

    vec3 ao = vec3(1.0);

    vec3 diff = vec3(0.0);

    vec3 spec = vec3(0.0);
    vec3 mirror = vec3(0.0);
    
    float facSpec = pow(max(dot(viewDir, mirror), 0.0), 500);

    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec2 texCoords = fs_in.TexCoords;

    if (parallaxMappingEnabled) {
        
        texCoords = ParallaxMapping(fs_in.TexCoords, viewDir);
        texCoords.x = mod(texCoords.x, 1.0);
        texCoords.y = mod(texCoords.y, 1.0);
        if (texCoords.x > 1.0 || texCoords.x < 0.0 || texCoords.y > 1.0 || texCoords.y < 0.0)
            discard;
    }
    //texCoords = texCoords * texScale;
    normal = texture(texture_normal1, texCoords).rgb * 2.0 - 1.0;
    //normal = normalize(transpose(fs_in.TBN) * normal);

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
    
    vec3 result = (diff + ao + spec);
    FragColor = pow(vec4(result, 1.0), (1/vec4(gamma)));
    //FragColor = pow(vec4(texture(texture_height1, fs_in.TexCoords).rrr, 1.0), (1/vec4(gamma)));
}