#include "WorldManager.h"
#include <iostream>
#include <chrono>
#define BLOCK_LIST_FILEPATH "../../Ressources/Data/Block/BlockList.txt"


void WorldManager::Init () {
	// Initialize all needed ressources for the world
	m_blockAssetManager = new BlockAssetManager(BLOCK_LIST_FILEPATH);
	m_chunkDataGenerator = ChunkDataGenerator(0, CHUNK_SIZE, GRID_SIZE, m_blockAssetManager);
	m_chunks = std::unordered_map<glm::ivec3, Chunk*>();

	int boundMinXZ = -12;
	int boundMaxXZ = 12;
	int boundMinY = -8;
	int boundMaxY = 8;

	// Create a debug chunks for testing
	// Generate grid data
	auto beginChunk = std::chrono::steady_clock::now();
	int index = 0;
	for(int x = boundMinXZ; x <= boundMaxXZ; x++) {
		for(int y = boundMinY; y <= boundMaxY; y++) {
			for(int z = boundMinXZ; z <= boundMaxXZ; z++) {
				Chunk* chunk = CreateChunk(glm::ivec3(x, y, z));
				m_chunkDataGenerator.GridDataPass(chunk);

				index++;
			}
		}
	}
	auto endChunk = std::chrono::steady_clock::now();
	auto durationChunk = std::chrono::duration_cast<std::chrono::microseconds>(endChunk - beginChunk).count();
	std::cout << "GridDataPass / Chunk : " << (durationChunk / index) << "[microseconds]" << std::endl;


	// Generate chunk data
	auto beginData = std::chrono::steady_clock::now();
	index = 0;
	for(int x = boundMinXZ; x <= boundMaxXZ - 1; x++) {
		for(int y = boundMinY; y <= boundMaxY - 1; y++) {
			for(int z = boundMinXZ; z <= boundMaxXZ - 1; z++) {
				auto chunk_it = m_chunks.find(glm::ivec3(x, y, z));
				if(chunk_it == m_chunks.end()) continue;

				Chunk* chunk = chunk_it->second;
				glm::ivec3 chunkPos = chunk->chunkPosition;
				std::vector<Chunk*> chunks = GetChunkGridAdjacent(chunkPos);

				m_chunkDataGenerator.ChunkDataPass(chunk, chunks);

				index++;
			}
		}
	}
	auto endData = std::chrono::steady_clock::now();
	auto durationData = std::chrono::duration_cast<std::chrono::microseconds>(endData - beginData).count();
	std::cout << "ChunkDataPass / Chunk : " << (durationData / index) << "[microseconds]" << std::endl;


	// Generate surface
	auto beginSurface = std::chrono::steady_clock::now();
	index = 0;
	for(int x = boundMinXZ + 1; x <= boundMaxXZ - 2; x++) {
		for(int y = boundMinY + 1; y <= boundMaxY - 2; y++) {
			for(int z = boundMinXZ + 1; z <= boundMaxXZ - 2; z++) {
				auto chunk_it = m_chunks.find(glm::ivec3(x, y, z));
				if(chunk_it == m_chunks.end()) continue;

				Chunk* chunk = chunk_it->second;
				glm::ivec3 chunkPos = chunk->chunkPosition;
				std::vector<Chunk*> chunks = GetChunkNeighbours(chunkPos);

				m_chunkDataGenerator.SurfacePass(chunk, chunks);

				index++;
			}
		}
	}
	auto endSurface = std::chrono::steady_clock::now();
	auto durationSurface = std::chrono::duration_cast<std::chrono::microseconds>(endSurface - beginSurface).count();
	std::cout << "SurfacePass / Chunk : " << (durationSurface / index) << "[microseconds]" << std::endl;


	// Generate mesh
	auto beginMesh = std::chrono::steady_clock::now();
	index = 0;
	for(int x = boundMinXZ + 1; x <= boundMaxXZ - 2; x++) {
		for(int y = boundMinY + 1; y <= boundMaxY - 2; y++) {
			for(int z = boundMinXZ + 1; z <= boundMaxXZ - 2; z++) {
				auto chunk_it = m_chunks.find(glm::ivec3(x, y, z));
				if(chunk_it == m_chunks.end()) continue;

				Chunk* chunk = chunk_it->second;
				glm::ivec3 chunkPos = chunk->chunkPosition;
				std::vector<Chunk*> chunks = GetChunkNeighbours(chunkPos);

				chunk->SetMesh(m_meshGenerator.GenerateChunkMesh(chunk, chunks));

				index++;
			}
		}
	}
	auto endMesh = std::chrono::steady_clock::now();
	auto durationMesh = std::chrono::duration_cast<std::chrono::microseconds>(endMesh - beginMesh).count();
	std::cout << "MeshGeneration / Chunk : " << (durationMesh / index) << "[microseconds]" << std::endl;
}

WorldManager::~WorldManager () {
	delete m_blockAssetManager;
}

Chunk* WorldManager::CreateChunk (glm::ivec3 chunkPos) {
	Chunk* newChunk = new Chunk(CHUNK_SIZE, GRID_SIZE, chunkPos, m_blockAssetManager);
	m_chunks.insert(std::pair<glm::ivec3, Chunk*>(chunkPos, newChunk));
	return newChunk;
}

void WorldManager::Render(SurfaceShader& shader, const glm::mat4x4& viewProjection) {
	for(auto& element : m_chunks) {
		element.second->Render(shader, viewProjection);
	}
}

std::vector<Chunk*> WorldManager::GetChunkNeighbours (glm::ivec3 chunkPosition) {
	std::vector<Chunk*> neighbours = std::vector<Chunk*>(27);
	for(int k = -1; k <= 1; k++) {
		for(int j = -1; j <= 1; j++) {
			for(int i = -1; i <= 1; i++) {
				int index = (i + 1) + (j + 1) * 3 + (k + 1) * 9;
				auto neighbour_it = m_chunks.find(
					glm::ivec3(chunkPosition.x + i, chunkPosition.y + j, chunkPosition.z + k)
				);
				if(neighbour_it == m_chunks.end())
					neighbours[index] = nullptr;
				else
					neighbours[index] = neighbour_it->second;
			}
		}
	}
	return neighbours;
}

std::vector<Chunk*> WorldManager::GetChunkGridAdjacent (glm::ivec3 chunkPosition) {
	std::vector<Chunk*> neighbours = std::vector<Chunk*>(8);
	for(int k = 0; k <= 1; k++) {
		for(int j = 0; j <= 1; j++) {
			for(int i = 0; i <= 1; i++) {
				int index = i + j * 2 + k * 4;
				auto neighbour_it = m_chunks.find(
					glm::ivec3(chunkPosition.x + i, chunkPosition.y + j, chunkPosition.z + k)
				);
				if(neighbour_it == m_chunks.end())
					neighbours[index] = nullptr;
				else
					neighbours[index] = neighbour_it->second;
			}
		}
	}
	return neighbours;
}