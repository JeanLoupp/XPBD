#version 430 core

layout(location=0) in vec3 aPos;

uniform mat4 lightMVP;

void main() {
  gl_Position = lightMVP*vec4(aPos, 1.0);
}
