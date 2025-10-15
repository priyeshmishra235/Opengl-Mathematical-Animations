#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;

uniform Material material;
uniform Light light;
uniform vec3 cameraPos;
void main() {
    // ╭─────────╮
    // │ Ambient │
    // ╰─────────╯
    vec3 ambient = light.ambient * material.ambient;

    // ╭─────────╮
    // │ Diffuse │
    // ╰─────────╯
    vec3 L = normalize(light.position - fragPos);
    vec3 N = normalize(normal);
    float diff = max(dot(N, L), 0.0f);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // ╭──────────╮
    // │ Specular │
    // ╰──────────╯
    vec3 V = normalize(cameraPos - fragPos);
    // phong model
    // vec3 R = 2 * (dot(N, L) * N) - L;
    // float spec = pow(max(dot(R, V), 0.0), shininessFactor);
    // this make it blinn phong (more stable)
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    //shininess color will be object color
    // vec3 result = (diffuse + ambient + specularRes) * objColor;

    //shininess color will be light color
    vec3 result = ambient + diffuse + specular;

    fragColor = vec4(result, 1.0f);
}
