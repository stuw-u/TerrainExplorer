#pragma once
#include <vector>
#include "../External/glm/vec3.hpp"
#include "VertexData.h"

class Mesh {
private:
	unsigned int vertsBuffer = 0;
	unsigned int vertsBufferSize = 0;
	unsigned int indexBuffer = 0;
	unsigned int indexBufferSize = 0;
	unsigned int vaoID = 0;

public:
	~Mesh();

	void Render() const;

	void SetData (std::vector<VertexData>& verticies, std::vector<uint32_t>& indicies);

	bool IsAllocated () const;
	bool IsCompleted () const;
}; 