#pragma once

#include "../External/glm/mat4x4.hpp"
#include "../Engine/Camera.h"

struct CameraParams;
class Camera;

glm::mat4x4 CreateViewMatrix (const Camera& camera);
glm::mat4x4 CreateProjMatrix (CameraParams params, glm::ivec2 displaySize);