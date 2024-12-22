#version 430 core

struct ShadowMap {
  mat4 MVP;
  sampler2D depthMap;
};

in vec3 FragPos;
out vec4 FragColor;

uniform vec3 viewPos;
uniform ShadowMap shadowMap;

void main() {
    // Checker color
    float scale = 1.0; // Size of checker
    float check = mod(floor(FragPos.x / scale) + floor(FragPos.z / scale), 2.0);
    vec3 checkerColor = mix(vec3(0.8), vec3(0.2), check);

    // Fog
    float distance = length(FragPos);
    float fogDensity = 0.05;
    float fogFactor = 1.0 - exp(-fogDensity * fogDensity * distance * distance);

    // Shadow
    vec4 posLightSpace = shadowMap.MVP * vec4(FragPos, 1.0);
    posLightSpace /= posLightSpace.w;
    posLightSpace = (posLightSpace + 1.0) * 0.5;

    // Compare depth + smooth lightning
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap.depthMap, 0);
    const int radius = 2;
    for (int x = -radius; x <= radius; ++x) {
        for (int y = -radius; y <= radius; ++y) {
            float pcfDepth = texture(shadowMap.depthMap, posLightSpace.xy + vec2(x, y) * texelSize).r;
            if (posLightSpace.z < pcfDepth) {
                shadow += 1.0;
            }
        }
    }
    shadow /= float((2*radius + 1) * (2*radius + 1));

    if(posLightSpace.z > 1.0)
        shadow = 1.0;

    vec3 litColor = (0.2 + shadow * 0.8) * checkerColor;
    vec3 finalColor = mix(litColor, vec3(0.2f, 0.3f, 0.3f), fogFactor);

    FragColor = vec4(finalColor, 1.0);
}
