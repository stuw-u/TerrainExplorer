#include "Renderer.h"
#include <glad/glad.h>
#include <iostream>
#include "Camera.h"

#define DEBUG_SHADER_FILEPATH "../../TerrainExplorer/Ressources/Shaders/Default_Surface.glsl"

Renderer::Renderer() {
	m_renderables = std::vector<IRenderable*>();
	testShader = nullptr;
}

void Renderer::message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
	if(severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

	auto const src_str = [source]() {
		switch(source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		}
	}();

	auto const type_str = [type]() {
		switch(type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		}
	}();

	auto const severity_str = [severity]() {
		switch(severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		}
	}();
	std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}

void Renderer::Init() {
	gladLoadGL();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(message_callback, nullptr);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	testShader = new SurfaceShader(DEBUG_SHADER_FILEPATH);
}

Renderer::~Renderer() {
	delete testShader;
}

void Renderer::Render (GLFWwindow* window, Camera* camera) {

	// Clear buffer with color
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.1f, 0.1f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	testShader->Bind();
	glm::mat4x4 viewProjection = camera->GetProjectionViewMatrix();

	for(auto element : m_renderables) {
		element->Render(*testShader, viewProjection);
	}

	testShader->Unbind();
	glDisable(GL_DEPTH_TEST);

	//Swap back and front buffer
	glfwSwapBuffers(window);
}

void Renderer::SetDisplaySize(int width, int height) {
	display.resolution = glm::ivec2(width, height);
}

glm::ivec2 Renderer::GetDisplaySize() {
	return display.resolution;
}

void Renderer::SetRenderables (std::vector<IRenderable*> renderables) {
	m_renderables = renderables;
}
