#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdio>
#include <chrono>
#include "Application.h"
#include "Engine/Display.h"
typedef std::chrono::high_resolution_clock Clock;


// Simulation settings
const unsigned int FIXED_UPDATE_PER_SECOND = 60; // Physics simm fps

// Window settings
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;



Application::Application () {

	// Initial the window/input library called glfw.
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creates a window.
	m_window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Terrain Explorer", NULL, NULL);
	if(m_window == NULL) {
		throw "Failed to create GLFW window";
		return;
	}

	// Hide console if there's no errors, send context to window
	glfwMakeContextCurrent(m_window);
	glfwSetWindowUserPointer(m_window, this);
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if(glfwRawMouseMotionSupported())
		glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	// Set frame buffer size callback
	glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);

	// Create a renderer (AFTER THE CONTEXT HAS BEEN CREATED!)
	m_renderables.push_back(&m_worldManager);
	m_renderer.Init();
	m_renderer.SetRenderables(m_renderables);
	m_renderer.SetDisplaySize(SCREEN_WIDTH, SCREEN_HEIGHT);
	CameraParams camParams = CameraParams();
	camParams.fov = 100;
	camParams.farClipPlane = 10000.0f;
	camParams.nearClipPlane = 0.01f;
	camera = new Camera(camParams);

	m_worldManager.Init();
}


Application::~Application () {

	// Cleans up glfw/window and renderer
	glfwDestroyWindow(m_window);
	glfwTerminate();
	delete camera;
}


void Application::RunGameLoop () {

	const double fixedSecondPerFrame = ((double)1 / (double)FIXED_UPDATE_PER_SECOND);

	auto previousFixedTime = Clock::now();
	auto previousTime = Clock::now();

	// Main program
	while(!glfwWindowShouldClose(m_window)) {
		auto currentTime = Clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - previousTime).count() / 1000000000.0;
		fixedDeltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - previousFixedTime).count() / 1000000000.0;
		previousTime = currentTime;
		
		if(fixedDeltaTime >= fixedSecondPerFrame) {
			previousFixedTime = currentTime;

			// Run simulation
			//std::cout << "Fixed delta time: " << fixedDeltaTime << std::endl;
		}

		// Gets windows size and gives it to the display
		int window_width;
		int window_height;
		glfwGetWindowSize(m_window, &window_width, &window_height);
		m_renderer.SetDisplaySize(window_width, window_height);

		// Renders the current scene
		camera->Update(camPos, camRot, m_renderer.GetDisplaySize());
		m_renderer.Render(m_window, camera);

		// Process out-of-game events
		ProcessInputs(m_window);

		// Update glfw events (window resize, cursor, keybord)
		glfwPollEvents();

	}
}

void Application::FramebufferSizeCallback (GLFWwindow* window, int width, int height) {

	// Make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);

	// Re-render the scene because the current frame was drawn for the old resolution
	void* const user_ptr = glfwGetWindowUserPointer(window);
	Application& app = *static_cast<Application*>(user_ptr);
	app.camera->Update(app.camPos, app.camRot, app.m_renderer.GetDisplaySize());
	app.m_renderer.Render(window, app.camera);
}

void Application::ProcessInputs (GLFWwindow* window) {

	// Close window when pressing ESC
	bool isEscapePressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
	if(!wasEscapePressed && isEscapePressed) {
		if(isCursorDisabled) {
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		} else {
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		isCursorDisabled = !isCursorDisabled;
	}
	wasEscapePressed = isEscapePressed;

	glm::dvec3 localDelta = glm::dvec3(0.0, 0.0, 0.0);
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		localDelta.z -= 8.0 * deltaTime;

	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		localDelta.z += 8.0 * deltaTime;

	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		localDelta.x -= 8.0 * deltaTime;

	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		localDelta.x += 8.0 * deltaTime;

	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		localDelta *= 4.0;

	double curX, curY;
	glfwGetCursorPos(window, &curX, &curY);
	double curDeltaX = curX - lastCurX;
	double curDeltaY = curY - lastCurY;
	lastCurX = curX;
	lastCurY = curY;
	if(isCursorDisabled) {
		camRot.x += curDeltaY * 0.4;
		camRot.y += curDeltaX * 0.4;
		camRot.x = glm::clamp(camRot.x, -90.0, 90.0);
		camRot.y = glm::mod(camRot.y, 360.0);
	}

	camPos += (glm::quat(glm::dvec3(-glm::radians(camRot.x), -glm::radians(camRot.y), 0)) * (glm::vec3)localDelta);

	//std::cout << cam_x << ", " << cam_y << ", " << cam_z << std::endl;
}

Camera& Application::GetCamera () {
	return *camera;
}