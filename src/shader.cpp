#include "../include/shader.h"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
  std::ifstream vertexFile(vertexPath), fragFile(fragmentPath);
  if (!vertexFile || !fragFile) {
    std::cerr << "Failed to open shader files: " << vertexPath << " or "
              << fragmentPath << std::endl;
    ID = 0;
    return;
  }
  std::stringstream vStream, fStream;
  vStream << vertexFile.rdbuf();
  fStream << fragFile.rdbuf();

  std::string vCode = vStream.str();
  std::string fCode = fStream.str();

  const char *vSrc = vCode.c_str();
  const char *fSrc = fCode.c_str();

  ID = createShaderProgram(vSrc, fSrc);
}

GLuint Shader::compileShader(GLenum type, const char *src) {
  GLuint id = glCreateShader(type);
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  GLint success;
  char infoLog[512];
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(id, 512, nullptr, infoLog);
    std::cerr << "Shader compile error: " << infoLog << std::endl;
    return 0;
  }

  return id;
}

GLuint Shader::createShaderProgram(const char *vCode, const char *fCode) {
  GLuint vs = compileShader(GL_VERTEX_SHADER, vCode);
  GLuint fs = compileShader(GL_FRAGMENT_SHADER, fCode);

  GLuint program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);

  GLint success;
  char infoLog[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (!success) {
    glGetProgramInfoLog(program, 512, nullptr, infoLog);
    std::cerr << "Program link error: " << infoLog << std::endl;
    glDeleteShader(vs);
    glDeleteShader(fs);
    return 0;
  }

  glDeleteShader(vs);
  glDeleteShader(fs);

  return program;
}

void Shader::use() { glUseProgram(ID); }

void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, const float *mat) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat);
}
void Shader::setVec3(const std::string &name, GLfloat a, GLfloat b,
                     GLfloat c) const {
  glUniform3f(glGetUniformLocation(ID, name.c_str()), a, b, c);
}
void Shader::setVec2(const std::string &name, GLfloat a, GLfloat b) const {
  glUniform2f(glGetUniformLocation(ID, name.c_str()), a, b);
}
Shader::~Shader() {
  if (ID) {
    glDeleteProgram(ID);
  }
}
