#pragma once
#include <iostream>
#include <glad/glad.h>

// OpenGL Error Management + Macros
#define ASSERT(x) if(!(x)) __debugbreak();
#define GLTry(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError() {
	while(glGetError() != GL_NO_ERROR);
}

static bool GLLogCall (const char* function, const char* file, int line) {
	while(GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] (" << error << ") " << function << " - " << file << " at line " << line << std::endl;
		return false;
	}
	return true;
}