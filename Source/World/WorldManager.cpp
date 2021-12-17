#include "WorldManager.h"
#include <iostream>
#define BLOCK_LIST_FILEPATH "../../Ressources/Data/Block/BlockList.txt"


void WorldManager::Init () {
	// Initialize all needed ressources for the world
	m_blockAssetManager = new BlockAssetManager(BLOCK_LIST_FILEPATH);
	m_chunkDataGenerator = ChunkDataGenerator(0, CHUNK_SIZE, m_blockAssetManager);
	m_chunks = std::unordered_map<glm::ivec3, Chunk*>();

	// Create a debug chunks for testing
	// Generate data
	for(int x = 0; x < 32; x++) {
		for(int z = 0; z < 32; z++) {
			Chunk* chunk = CreateChunk(glm::ivec3(x, 0, z));
			m_chunkDataGenerator.GenerateChunkData(chunk);
		}
	}

	// Generate mesh
	for(int x = 0; x < 32; x++) {
		for(int z = 0; z < 32; z++) {
			auto chunk_it = m_chunks.find(glm::ivec3(x, 0, z));
			if(chunk_it == m_chunks.end()) continue;

			Chunk* chunk = chunk_it->second;
			glm::ivec3 chunkPos = chunk->chunkPosition;
			std::vector<Chunk*> neighbours = GetChunkNeighbours(chunkPos);

			chunk->SetMesh(m_meshGenerator.GenerateChunkMesh(chunk, neighbours));
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

std::vector<Chunk*> WorldManager::GetChunkNeighbours (glm::ivec3 chunkPosition) {
	std::vector<Chunk*> neighbours = std::vector<Chunk*>(6);
	for(int i = 0; i < 6; i++) {
		auto neighbour_it = m_chunks.find(chunkPosition + MeshGenerator::enumToIntNormal[i]);
		if(neighbour_it == m_chunks.end())
			neighbours[i] = nullptr;
		else
			neighbours[i] = neighbour_it->second;
	}
	return neighbours;
}