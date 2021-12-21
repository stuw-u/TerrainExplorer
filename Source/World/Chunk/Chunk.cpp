#include "Chunk.h"
#include <iostream>

Chunk::Chunk(uint8_t size, uint8_t gridSize, glm::ivec3 chunkPosition, BlockAssetManager* context):
	data(size, context),
	gridData(gridSize),
	m_size(size), 
	chunkPosition(chunkPosition),
	m_mesh(),
	m_box(glm::vec3(size, size, size))
{
	m_box.SetPosition(static_cast<glm::vec3>(chunkPosition) * static_cast<float>(size));
}

Chunk::~Chunk() {
	m_mesh.Dispose();
}

void Chunk::SetMesh(Mesh mesh) {
	m_mesh = mesh;
}

bool Chunk::Render(SurfaceShader& shader, const glm::mat4x4& viewProjection) {
	if(!m_mesh.IsCompleted()) {
		return false;
	}
	m_mesh.Render(shader, viewProjection, static_cast<glm::vec3>(chunkPosition) * static_cast<float>(m_size));
	return true;
}

bool Chunk::CanRender () {
	return m_mesh.IsCompleted();
}

const AABB& Chunk::GetBox () const {
	return m_box;
};