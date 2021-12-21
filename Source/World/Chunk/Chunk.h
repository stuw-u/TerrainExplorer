#pragma once
#include "ChunkData.h"
#include "ChunkGridData.h"
#include "../../Math/AABB.h"
#include "../../Engine/Mesh.h"
#include "../../Engine/Shader.h"
#include "../../External/glm/vec3.hpp"
#include "../../External/glm/mat4x4.hpp"
#include <memory>
#include <thread>
#include <mutex>
#include "../Generator/ChunkMeshData.h"

class Chunk {
public:
	enum class State : int {
		Empty = 0,
		GridData,
		Sampling,
		Surface,
		Complete
	};
private:
	uint8_t m_size;
	Mesh m_mesh;
	AABB m_box;

public:

	// Most of the stuff here shouldn't be public
	ChunkData data;
	ChunkGridData gridData;
	ChunkMeshData meshData;
	glm::ivec3 chunkPosition;
	State state;


public:
	Chunk() = delete;
	Chunk(uint8_t size, uint8_t gridSize, glm::ivec3 chunkPosition, BlockAssetManager* context);
	~Chunk();


	void SetMesh (Mesh mesh);
	bool Render(SurfaceShader& shader, const glm::mat4x4& viewProjection);
	bool CanRender();
	const AABB& GetBox() const;
	
};