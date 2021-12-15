#include "Chunk.h"
#include <iostream>

Chunk::Chunk(uint8_t size, glm::ivec3 chunkPosition, BlockAssetManager* context):
	data(size, context), 
	m_size(size), 
	chunkPosition(chunkPosition),
	m_mesh() {
}

Chunk::~Chunk() {
}

void Chunk::SetMesh(Mesh mesh) {
	m_mesh = mesh;
}

bool Chunk::Render() const {
	if(!m_mesh.IsCompleted())
		return false;
	m_mesh.Render();
	return true;
}