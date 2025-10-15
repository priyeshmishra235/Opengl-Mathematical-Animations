#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 Normal;

out vec3 normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    fragPos = vec3(model * vec4(aPos, 1.0)); // World-space position
    normal = mat3(transpose(inverse(model))) * Normal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
