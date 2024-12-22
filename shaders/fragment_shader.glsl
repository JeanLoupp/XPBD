#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightDir;    // Direction de la lumière (doit être normalisée
uniform vec3 viewPos;     // Position de la caméra
uniform vec3 lightColor;  // Couleur de la lumière
uniform vec3 objectColor; // Couleur de l'objet

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

    // Résultat final
    vec3 result = ambient * objectColor + diffuse * objectColor + specular;
    FragColor = vec4(result, 1.0);
}
