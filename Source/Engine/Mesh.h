#pragma once
#include <vector>
#include "Shader.h"
#include "VertexData.h"
#include "../External/glm/vec3.hpp"
#include "../External/glm/mat4x4.hpp"

class Mesh {
private:
	unsigned int vertsBuffer = 0;
	unsigned int vertsBufferSize = 0;
	unsigned int indexBuffer = 0;
	unsigned int indexBufferSize = 0;
	unsigned int vaoID = 0;

public:
	void Render(SurfaceShader& shader, const glm::mat4x4& viewProjection, glm::vec3 position) const;

	void SetData (std::vector<VertexData>& verticies, std::vector<uint32_t>& indicies);

	bool IsAllocated () const;
	bool IsCompleted () const;

	void Dispose ();
}; 