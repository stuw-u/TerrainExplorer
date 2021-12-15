#include "WorldManager.h"
#include <iostream>
#define BLOCK_LIST_FILEPATH "../../TerrainExplorer/Ressources/Data/Block/BlockList.txt"


void WorldManager::Init () {
	// Initialize all needed ressources for the world
	m_blockAssetManager = new BlockAssetManager(BLOCK_LIST_FILEPATH);
	m_chunkDataGenerator = ChunkDataGenerator(0, CHUNK_SIZE, m_blockAssetManager);
	m_chunks = std::unordered_map<glm::ivec3, Chunk*>(512);

	// Create a debug chunks for testing
	for(int x = 0; x < 32; x++) {
		for(int z = 0; z < 32; z++) {
			Chunk* chunk = CreateChunk(glm::ivec3(x, 0, z));
			m_chunkDataGenerator.GenerateChunkData(chunk);
			chunk->SetMesh(m_meshGenerator.GenerateChunkMesh(chunk));
		}
	}
}

WorldManager::~WorldManager () {
	delete m_blockAssetManager;
}

Chunk* WorldManager::CreateChunk (glm::ivec3 chunkPos) {
	Chunk* newChunk = new Chunk(CHUNK_SIZE, chunkPos, m_blockAssetManager);
	m_chunks.insert(std::pair<glm::ivec3, Chunk*>(chunkPos, newChunk));
	return newChunk;
}

void WorldManager::Render(SurfaceShader& shader, const glm::mat4x4& viewProjection) {
	for(auto& element : m_chunks) {
		element.second->Render(shader, viewProjection);
	}
}