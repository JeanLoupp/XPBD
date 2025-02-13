#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightDir;    // Direction de la lumière (doit être normalisée
uniform vec3 viewPos;     // Position de la caméra
uniform vec3 lightColor;  // Couleur de la lumière
uniform vec3 objectColor; // Couleur de l'objet


struct ShadowMap {
    bool use;
    mat4 MVP;
    sampler2D depthMap;
};
uniform ShadowMap shadowMap;

float getShadow(){
    if (!shadowMap.use) return 1.0f;

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
            if (posLightSpace.z < pcfDepth + 0.003) {
                shadow += 1.0;
            }
        }
    }
    shadow /= float((2*radius + 1) * (2*radius + 1));

    if(posLightSpace.z > 1.0)
        shadow = 1.0;

    return shadow;
}

void main() {
    vec3 norm = normalize(Normal);
    
    // Composante ambiante
    vec3 ambient = 0.2 * lightColor;
    
    // Composante diffuse
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * lightColor * 0.8;
    
    // Composante spéculaire
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightDir, norm);
    float specularStrength = 0.3;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
    vec3 specular = specularStrength * spec * lightColor;

    // Shadow
    float shadow = getShadow();

    // Résultat final
    vec3 result = ambient * objectColor 
                + shadow * (diffuse * objectColor + specular);
    FragColor = vec4(result, 1.0);
}
