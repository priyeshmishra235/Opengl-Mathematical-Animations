// The fucking code is running on cpu not gpu.
// Use shader to run it on GPU
#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "../include/callback.h"
#include "../include/camera.h"
#include "../include/makeCircleAndSphere.h"
#include "../include/shader.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

const float mainScale = 1.0f;

enum AnimationType {
  ANIM_NONE = 0,
  ANIM_RIPPLE,
  ANIM_NOISE,
  ANIM_WIND,
  ANIM_GALAXY,
  ANIM_CIRCLE,
  ANIM_SPHERE
};

int main() {
  if (!glfwInit()) {
    std::cerr << "glfwInit failed" << std::endl;
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800 * mainScale, 600 * mainScale,
                                        "GL Rf", nullptr, nullptr);
  if (!window) {
    std::cerr << "window::failed" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetCursorPosCallback(window, mouseCallback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  glEnable(GL_DEPTH_TEST);

  unsigned int vao, vbo;
  unsigned int lightVao;

  float vertices[] = {
      // positions          // normals

      // Front face (z = +0.5)
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

      // Back face (z = -0.5)
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 0.0f,
      -1.0f, 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.5f, 0.5f, -0.5f, 0.0f,
      0.0f, -1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, -0.5f, -0.5f, -0.5f,
      0.0f, 0.0f, -1.0f,

      // Left face (x = -0.5)
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -0.5f, -0.5f, 0.5f, -1.0f, 0.0f,
      0.0f, -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, -1.0f,
      0.0f, 0.0f, -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f,
      -1.0f, 0.0f, 0.0f,

      // Right face (x = +0.5)
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

      // Top face (y = +0.5)
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

      // Bottom face (y = -0.5)
      -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.5f, -0.5f, -0.5f, 0.0f, -1.0f,
      0.0f, 0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.5f, -0.5f, 0.5f, 0.0f,
      -1.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, -0.5f, -0.5f, -0.5f,
      0.0f, -1.0f, 0.0f};

  float planeVertices[] = {-0.5f, -0.5f, 0.0f, 0.5f,  -0.5f, 0.0f,
                           0.5f,  0.5f,  0.0f, 0.5f,  0.5f,  0.0f,
                           -0.5f, 0.5f,  0.0f, -0.5f, -0.5f, 0.0f};

  // plane
  GLuint planeVao, planeVbo;
  glGenVertexArrays(1, &planeVao);
  glGenBuffers(1, &planeVbo);
  glBindVertexArray(planeVao);
  glBindBuffer(GL_ARRAY_BUFFER, planeVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  // object cube
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // light cube VAO
  glGenVertexArrays(1, &lightVao);
  glBindVertexArray(lightVao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  std::vector<float> circleVertices = makeCircle(0.0f, 0.0f, 0.5f, 32);
  unsigned int circleVbo, circleVao;
  glGenVertexArrays(1, &circleVao);
  glGenBuffers(1, &circleVbo);
  glBindVertexArray(circleVao);
  glBindBuffer(GL_ARRAY_BUFFER, circleVbo);
  glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float),
               circleVertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  std::vector<Vertex> sphereVertices = makeSphere(1.0f, 36, 18);
  std::vector<unsigned int> indices = makeSphereIndices(36, 18);
  unsigned int sphereVBO, sphereVao, sphereEBO;
  glGenVertexArrays(1, &sphereVao);
  glGenBuffers(1, &sphereVBO);
  glGenBuffers(1, &sphereEBO);
  glBindVertexArray(sphereVao);
  glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
  glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(Vertex),
               sphereVertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  Shader shader("../shaders/vert.glsl", "../shaders/frag.glsl");
  Shader light("../shaders/lightVert.glsl", "../shaders/lightFrag.glsl");
  Shader axes("../shaders/axesVert.glsl", "../shaders/axesFrag.glsl");

  shader.use();
  shader.setVec3("material.ambient", 0.3f, 0.25f, 0.1f);
  shader.setVec3("material.diffuse", 0.8f, 0.7f, 0.2f);
  shader.setVec3("material.specular", 0.9f, 0.9f, 0.8f);
  shader.setFloat("material.shininess", 64.0f);

  shader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
  shader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
  shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
  shader.setVec3("light.position", 2.0f, 4.0f, 2.0f);

  AnimationType currentAnim = ANIM_NOISE;

  // shared params
  bool animate = true;
  float globalScale = 1.0f;

  // ripple params
  float waveNum = 7.0f;
  float rippleAmplitude = 10.0f;
  float rippleSpacing = 0.5f;
  float rippleDamping = 0.9f;

  // noise params
  float noiseAmplitude = 1.0f;
  float noiseSpacing = 0.5f;
  float noisePhase1 = 0.2f;
  float noisePhase2 = 0.8f;
  float noisePhase3 = 1.5f;

  // wind/shear params
  float windAmpY = 1.0f;
  float windAmpX = 0.5f;
  float windWaveSpeed = 7.0f;
  float windSpacing = 0.5f;

  // galaxy params
  float galaxySpacing = 0.6f;
  float galaxyScale = 5.0f;
  float galaxySpin = 2.0f;
  float galaxySpeed = 1.0f;

  // misc
  float light_intensity = 1.0f;
  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Animation Controls");
    {
      ImGui::Text("Choose effect:");
      if (ImGui::RadioButton("Ripple", currentAnim == ANIM_RIPPLE))
        currentAnim = ANIM_RIPPLE;
      if (ImGui::RadioButton("Noise", currentAnim == ANIM_NOISE))
        currentAnim = ANIM_NOISE;
      if (ImGui::RadioButton("Wind/Shear", currentAnim == ANIM_WIND))
        currentAnim = ANIM_WIND;
      if (ImGui::RadioButton("Galaxy", currentAnim == ANIM_GALAXY))
        currentAnim = ANIM_GALAXY;
      if (ImGui::RadioButton("Circle", currentAnim == ANIM_CIRCLE))
        currentAnim = ANIM_CIRCLE;
      if (ImGui::RadioButton("Sphere", currentAnim == ANIM_SPHERE))
        currentAnim = ANIM_SPHERE;

      ImGui::Separator();
      ImGui::Checkbox("Animate", &animate);
      ImGui::SliderFloat("Global Scale", &globalScale, 0.1f, 5.0f);

      ImGui::Separator();
      // ripple controls
      ImGui::Text("Ripple parameters");
      ImGui::SliderFloat("WaveNum", &waveNum, 0.1f, 100.0f);
      ImGui::SliderFloat("Amplitude", &rippleAmplitude, 0.1f, 20.0f);
      ImGui::SliderFloat("Spacing", &rippleSpacing, 0.1f, 2.0f);
      ImGui::SliderFloat("Damping", &rippleDamping, 0.0f, 2.0f);

      ImGui::Separator();
      // noise controls
      ImGui::Text("Noise parameters");
      ImGui::SliderFloat("Noise amplitude", &noiseAmplitude, 0.0f, 5.0f);
      ImGui::SliderFloat("Noise spacing", &noiseSpacing, 0.1f, 2.0f);

      ImGui::Separator();
      // wind controls
      ImGui::Text("Wind params");
      ImGui::SliderFloat("Wind amp Y", &windAmpY, 0.0f, 5.0f);
      ImGui::SliderFloat("Wind amp X", &windAmpX, 0.0f, 5.0f);
      ImGui::SliderFloat("Wind speed", &windWaveSpeed, 0.0f, 20.0f);
      ImGui::SliderFloat("Wind spacing", &windSpacing, 0.1f, 2.0f);

      ImGui::Separator();
      // galaxy controls
      ImGui::Text("Galaxy params");
      ImGui::SliderFloat("Galaxy spacing", &galaxySpacing, 0.1f, 2.0f);
      ImGui::SliderFloat("Galaxy scale", &galaxyScale, 0.1f, 20.0f);
      ImGui::SliderFloat("Galaxy spin", &galaxySpin, 0.0f, 10.0f);
      ImGui::SliderFloat("Galaxy speed", &galaxySpeed, 0.0f, 10.0f);

      ImGui::Separator();
      if (ImGui::Button("Reset to Defaults")) {
        waveNum = 7.0f;
        rippleAmplitude = 5.0f;
        rippleSpacing = 0.5f;
        rippleDamping = 0.5f;
        noiseAmplitude = 1.0f;
        noiseSpacing = 0.5f;
        windAmpY = 1.0f;
        windAmpX = 0.5f;
        windWaveSpeed = 7.0f;
        galaxySpacing = 0.6f;
        galaxyScale = 5.0f;
        galaxySpin = 2.0f;
        galaxySpeed = 1.0f;
      }
    }
    ImGui::End();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);

    axes.use();
    glm::mat4 axesModel = glm::mat4(1.0f);
    axes.setMat4("view", glm::value_ptr(view));
    axes.setMat4("projection", glm::value_ptr(projection));
    glBindVertexArray(lightVao);

    axesModel = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f, 0.1f, 0.1f));
    axes.setMat4("model", glm::value_ptr(axesModel));
    axes.setVec3("axesColor", 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    axesModel = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 50.0f, 0.1f));
    axes.setMat4("model", glm::value_ptr(axesModel));
    axes.setVec3("axesColor", 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    axesModel = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 50.0f));
    axes.setMat4("model", glm::value_ptr(axesModel));
    axes.setVec3("axesColor", 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    light.use();
    light.setMat4("view", glm::value_ptr(view));
    light.setMat4("projection", glm::value_ptr(projection));
    glBindVertexArray(lightVao);

    float t = (animate ? (float)glfwGetTime() : 0.0f);

    if (currentAnim == ANIM_RIPPLE) {
      light.use();
      for (int i = -50; i < 50; ++i) {
        for (int k = -50; k < 50; ++k) {
          float x_coord = (float)i * rippleSpacing;
          float z_coord = (float)k * rippleSpacing;
          float r = std::sqrt(x_coord * x_coord + z_coord * z_coord);
          float arg = t * waveNum - r * rippleDamping;
          float simpleWave = sin(arg);
          float amplitude_scaling = rippleAmplitude / (r + 1.0f);
          float waveY = simpleWave * amplitude_scaling;
          glm::mat4 lightModel = glm::mat4(1.0f);
          lightModel = glm::translate(
              lightModel, glm::vec3(x_coord, 0.0f + waveY, z_coord));
          lightModel = glm::scale(lightModel, glm::vec3(rippleSpacing * 0.4f));
          light.setMat4("model", glm::value_ptr(lightModel));
          float r_color = (x_coord / rippleAmplitude * 0.5f) + 0.8f;
          float g_color = (waveY / rippleAmplitude * 0.8f) + 0.5f;
          float b_color = std::abs(sin(arg * 0.3f));
          light.setVec3("lightColor", r_color, g_color, b_color);
          glDrawArrays(GL_TRIANGLES, 0, 36);
        }
      }
    }
    if (currentAnim == ANIM_NOISE) {
      light.use();
      for (int i = -50; i < 50; ++i) {
        for (int k = -50; k < 50; ++k) {
          float x_coord = (float)i * noiseSpacing;
          float z_coord = (float)k * noiseSpacing;
          float noise_sum = 0.0f;
          float phase1 = x_coord * noisePhase1 + z_coord * 0.1f;
          noise_sum += 1.0f * sin(t * 0.8f + phase1);
          float phase2 = x_coord * noisePhase2 + z_coord * 0.4f;
          noise_sum += 0.5f * sin(t * 2.5f + phase2);
          float phase3 = x_coord * noisePhase3 - z_coord * 1.5f;
          noise_sum += 0.25f * sin(t * 5.0f + phase3);
          const float normalization = 1.0f + 0.5f + 0.25f;
          float waveY = (noise_sum / normalization) * noiseAmplitude;
          float r_color = std::abs(sin(x_coord * 0.1f + t * 0.1f));
          float g_color = (waveY / noiseAmplitude * 0.5f) + 0.5f;
          float b_color = 0.9f;
          glm::mat4 lightModel = glm::mat4(1.0f);
          lightModel = glm::translate(
              lightModel, glm::vec3(x_coord, 0.0f + waveY, z_coord));
          lightModel = glm::scale(lightModel,
                                  glm::vec3(noiseSpacing * 0.4f * globalScale));
          light.setMat4("model", glm::value_ptr(lightModel));
          light.setVec3("lightColor", r_color * light_intensity,
                        g_color * light_intensity, b_color * light_intensity);
          glDrawArrays(GL_TRIANGLES, 0, 36);
        }
      }
    }
    if (currentAnim == ANIM_WIND) {
      light.use();
      for (int i = -50; i < 50; ++i) {
        for (int k = -50; k < 50; ++k) {
          float x_coord = (float)i * windSpacing;
          float z_coord = (float)k * windSpacing;
          float phase_shift = x_coord * 0.5f;
          float arg = t * windWaveSpeed + phase_shift;
          float waveY = sin(arg) * windAmpY;
          float waveX = cos(arg) * windAmpX;
          float r_color = (waveX / (windAmpX + 1e-6f) * 0.5f) + 0.5f;
          float g_color = (waveY / (windAmpY + 1e-6f) * 0.5f) + 0.5f;
          float b_color = std::abs(sin(t * 0.3f));
          glm::mat4 lightModel = glm::mat4(1.0f);
          lightModel = glm::translate(
              lightModel, glm::vec3(x_coord + waveX, 0.0f + waveY, z_coord));
          lightModel = glm::scale(lightModel,
                                  glm::vec3(windSpacing * 0.4f * globalScale));
          light.setMat4("model", glm::value_ptr(lightModel));
          light.setVec3("lightColor", r_color * light_intensity,
                        g_color * light_intensity, b_color * light_intensity);
          glDrawArrays(GL_TRIANGLES, 0, 36);
        }
      }
    }
    if (currentAnim == ANIM_GALAXY) {
      light.use();
      int rings = 150;
      for (int r = 1; r < rings; ++r) {
        for (int a = 0; a < 360; a += 8) {
          float theta = glm::radians((float)a);
          float radius = (float)r * galaxySpacing;
          float spinAngle = galaxySpin * (float)r * 0.02f + galaxySpeed * t;
          float x = radius * cos(theta + spinAngle);
          float z = radius * sin(theta + spinAngle);
          float y = 0.5f * sin(t + r * 0.1f) * (1.0f / (r + 1)) * galaxyScale;
          glm::mat4 lightModel = glm::mat4(1.0f);
          lightModel = glm::translate(lightModel, glm::vec3(x, y, z));
          float s = (1.2f / (r + 1)) * globalScale;
          lightModel = glm::scale(lightModel, glm::vec3(s));
          light.setMat4("model", glm::value_ptr(lightModel));
          float brightness = 100.0f / (r + 0.1f);
          float r_color = std::abs(sin(theta + t));
          float g_color = std::abs(cos(theta * 0.5f + t * 0.5f));
          float b_color = std::abs(sin(t * 0.1f + r * 0.03f));
          light.setVec3("lightColor", r_color * light_intensity,
                        g_color * light_intensity, b_color * light_intensity);
          glDrawArrays(GL_TRIANGLES, 0, 36);
        }
      }
    }
    if (currentAnim == ANIM_CIRCLE) {
      shader.use();
      glBindVertexArray(circleVao);
      glm::mat4 model = glm::mat4(1.0f);
      model =
          glm::scale(model, glm::vec3(globalScale, globalScale, globalScale));
      shader.setMat4("model", glm::value_ptr(model));
      shader.setMat4("view", glm::value_ptr(view));
      shader.setMat4("projection", glm::value_ptr(projection));
      shader.setVec3("objColor", 1.0f, 0.5f, 0.6f);
      glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)(circleVertices.size() / 2));
    }
    if (currentAnim == ANIM_SPHERE) {
      shader.use();
      glBindVertexArray(sphereVao);
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(-2.0f, -2.0f, 0.0f));
      model = glm::scale(model, glm::vec3(globalScale));
      shader.setMat4("model", glm::value_ptr(model));
      shader.setMat4("view", glm::value_ptr(view));
      shader.setMat4("projection", glm::value_ptr(projection));
      glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glDeleteVertexArrays(1, &vao);
  glDeleteVertexArrays(1, &lightVao);
  glDeleteBuffers(1, &vbo);
  glfwTerminate();
  return 0;
}
