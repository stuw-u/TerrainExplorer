#pragma once
#include "ChunkData.h"
#include "../../Engine/Mesh.h"
#include "../../External/glm/vec3.hpp"

class Chunk {
private:
	uint8_t m_size;
	Mesh m_mesh;
public:
	ChunkData data;
	glm::ivec3 chunkPosition;

public:
	Chunk() = delete;
	Chunk(uint8_t size, glm::ivec3 chunkPosition, BlockAssetManager* context);
	~Chunk();

	void SetMesh (Mesh mesh);
	bool Render() const;
	
};