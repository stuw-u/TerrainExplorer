#pragma once
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Shader.h"
#include "Display.h"
#include "IRenderable.h"

class Renderer {
private:
	Display display;
	std::vector<IRenderable*> m_renderables;

	SurfaceShader* testShader;
public:
	Renderer();
	~Renderer();

	void Init();

	void Render(GLFWwindow* window, Camera* camera);

	void SetDisplaySize(int width, int height);
	glm::ivec2 GetDisplaySize();

	void SetRenderables (std::vector<IRenderable*> renderables);
private:
	static void APIENTRY message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);

};