#pragma once
#include "ChunkData.h"
#include "../../Engine/Mesh.h"
#include "../../Engine/Shader.h"
#include "../../External/glm/vec3.hpp"
#include "../../External/glm/mat4x4.hpp"

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
	bool Render(SurfaceShader& shader, const glm::mat4x4& viewProjection) const;
	
};