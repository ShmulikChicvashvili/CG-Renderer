#pragma once

#include <string>

//static char* readShaderSource(const char* shaderFile);
static std::string readShaderSource(const char* shaderFile);
GLuint InitShader(const char* vShaderFile, const char* fShaderFile);
