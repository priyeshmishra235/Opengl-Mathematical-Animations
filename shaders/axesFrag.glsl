#version 330 core

out vec4 FragColor;
uniform vec3 axesColor;

void main() {
    FragColor = vec4(axesColor, 1.0f);
}
