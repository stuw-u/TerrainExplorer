#include "Camera.h"
#include "../Math/Matrix.h"

Camera::Camera(CameraParams params) :
	m_params(params), position(glm::vec3()), rotation(glm::vec3()) {
}

void Camera::SetFOV(float fov) {
	m_params.fov = fov;
}

float Camera::GetFOV() {
	return m_params.fov;
}

void Camera::SetParams(CameraParams params) {
	m_params = params;
	
}

CameraParams Camera::GetParams() {
	return m_params;
}


void Camera::Update (glm::dvec3 position, glm::vec3 rotation, glm::ivec2 displaySize) {
	this->position = position;
	this->rotation = rotation;
	m_projectionMatrix = CreateProjMatrix(m_params, displaySize);
	m_viewMatrix = CreateViewMatrix(*this);
	m_projectionViewMatrix = m_projectionMatrix * m_viewMatrix;
	m_frustrum.SetMatrix(m_projectionViewMatrix);
}

glm::mat4x4 Camera::GetProjectionViewMatrix() {
	return m_projectionViewMatrix;
}

const Frustum& Camera::GetFrustrum() const {
	return m_frustrum;
}