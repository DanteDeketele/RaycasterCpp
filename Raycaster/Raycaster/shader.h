#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>

std::string readFile(const char* filepath);
GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath);

#endif
