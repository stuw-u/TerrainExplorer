#pragma once
#include <vector>
#include "Engine/Renderer.h"
#include "Engine/IRenderable.h"
#include "Engine/Camera.h"
#include "World/WorldManager.h"
#include "External/glm/gtc/quaternion.hpp"
#include "External/glm/gtx/quaternion.hpp"
#include "External/glm/vec3.hpp"

class Application {
private:
	GLFWwindow* m_window;
	Renderer m_renderer;
	std::vector<IRenderable*> m_renderables;
	WorldManager m_worldManager;

	double deltaTime = 0.1;
	double fixedDeltaTime = 0.1;

	glm::dvec3 camPos;
	glm::dvec3 camRot;
	bool isCursorDisabled = true;
	double lastCurX = 0.0;
	double lastCurY = 0.0;

	bool wasEscapePressed = false;

	
public:
	Camera* camera;

	Application();

	~Application();

	void RunGameLoop ();

	void ProcessInputs (GLFWwindow* window);

	static void FramebufferSizeCallback (GLFWwindow* window, int width, int height);

	Camera& GetCamera();
};