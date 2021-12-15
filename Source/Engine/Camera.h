#pragma once

#include "../Math/Matrix.h"
#include "../External/glm/mat4x4.hpp"
#include "../External/glm/vec3.hpp"

struct CameraParams {
	float fov = 90.0f;
	float nearClipPlane = 0.1f;
	float farClipPlane = 2000.0f;
};

class Camera {
public:
	glm::dvec3 position;
	glm::vec3 rotation;
private:
	CameraParams m_params;
	glm::mat4x4 m_viewMatrix;
	glm::mat4x4 m_projectionMatrix;
	glm::mat4x4 m_projectionViewMatrix;
	// TODO: Add frustrum for culling
public:
	Camera(CameraParams params);

	void SetFOV (float fov);
	float GetFOV ();

	void SetParams (CameraParams params);
	CameraParams GetParams ();

	void Update (glm::dvec3 position, glm::vec3 rotation, glm::ivec2 displaySize);

	glm::mat4x4 GetProjectionViewMatrix();
};