#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec2 inverseScreenSize;

void main()
{
    vec3 rgbNW = texture(screenTexture, TexCoords + vec2(-1.0, -1.0) * inverseScreenSize).rgb;
    vec3 rgbNE = texture(screenTexture, TexCoords + vec2(1.0, -1.0) * inverseScreenSize).rgb;
    vec3 rgbSW = texture(screenTexture, TexCoords + vec2(-1.0, 1.0) * inverseScreenSize).rgb;
    vec3 rgbSE = texture(screenTexture, TexCoords + vec2(1.0, 1.0) * inverseScreenSize).rgb;
    vec3 rgbM  = texture(screenTexture, TexCoords).rgb;

    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * (1.0 / 8.0)), 1.0 / 128.0);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(vec2(8.0, 8.0), max(vec2(-8.0, -8.0), dir * rcpDirMin)) * inverseScreenSize;

    vec3 rgbA = 0.5 * (
        texture(screenTexture, TexCoords + dir * (1.0 / 3.0 - 0.5)).rgb +
        texture(screenTexture, TexCoords + dir * (2.0 / 3.0 - 0.5)).rgb);
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture(screenTexture, TexCoords + dir * -0.5).rgb +
        texture(screenTexture, TexCoords + dir * 0.5).rgb);

    float lumaB = dot(rgbB, luma);
    if ((lumaB < lumaMin) || (lumaB > lumaMax))
        FragColor = vec4(rgbA, 1.0);
    else
        FragColor = vec4(rgbB, 1.0);
}