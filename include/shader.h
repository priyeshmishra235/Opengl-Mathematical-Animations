#pragma once
#include "../glad/glad.h"
#include <string>

class Shader {
public:
  GLuint ID;

  Shader(const char *vertexPath, const char *fragmentPath);

  void use();
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setMat4(const std::string &name, const float *mat) const;
  void setVec2(const std::string &name, GLfloat a, GLfloat b) const;
  void setVec3(const std::string &name, float, float, float) const;
  ~Shader();

private:
  static GLuint compileShader(GLenum type, const char *src);
  static GLuint createShaderProgram(const char *vCode, const char *fCode);
};
