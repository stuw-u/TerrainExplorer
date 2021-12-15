#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <vector>
#include <unordered_map>
#include "../External/glm/vec3.hpp"
#include "../External/glm/gtx/hash.hpp"
#include "Asset/BlockAssetManager.h"
#include "Chunk/Chunk.h"
#include "Generator/ChunkDataGenerator.h"
#include "Generator/MeshGenerator.h"
#include "../Engine/IRenderable.h"

#define CHUNK_SIZE 16

class WorldManager : public IRenderable {
private:
	BlockAssetManager* m_blockAssetManager = nullptr;
	ChunkDataGenerator m_chunkDataGenerator;
	MeshGenerator m_meshGenerator;

	std::unordered_map<glm::ivec3, Chunk*> m_chunks;

public:
	~WorldManager();

	void Init ();

	Chunk* CreateChunk(glm::ivec3 chunkPos);

	void Render() override;
};