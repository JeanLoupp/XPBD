#version 430 core

in vec3 FragPos;
out vec4 FragColor;

uniform vec3 viewPos;

void main() {
    // Calculate the checker pattern
    float scale = 1.0; // Size of each checker square
    float check = mod(floor(FragPos.x / scale) + floor(FragPos.z / scale), 2.0);
    vec3 checkerColor = mix(vec3(0.8), vec3(0.2), check);

    // Calculate distance from the camera for fog
    float distance = length(FragPos - viewPos);
    float fogDensity = 0.05;
    float fogFactor = 1.0 - exp(-fogDensity * fogDensity * distance * distance);

    // Mix the checker color with the fog color
    vec3 finalColor = mix(checkerColor, vec3(0.2f, 0.3f, 0.3f), fogFactor);

    FragColor = vec4(finalColor, 1.0);
}