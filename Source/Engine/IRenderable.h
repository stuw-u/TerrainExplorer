#pragma once
#include "Shader.h"

class IRenderable {
public:
	virtual ~IRenderable() {}
	virtual void Render(SurfaceShader& shader, const glm::mat4x4& viewProjection) {};
};
