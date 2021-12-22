#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <vector>
#include "../Engine/IRenderable.h"
#include "../Engine/Chrono.h"
#include "../External/glm/vec3.hpp"
#include "../External/glm/gtx/hash.hpp"
#include "../Math/Frustrum.h"
#include "Chunk/Chunk.h"
#include "Generator/MeshGenerator.h"
#include "Generator/ChunkDataGenerator.h"
#include "Asset/BlockAssetManager.h"

#include <unordered_map>
#include <iostream>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>

#define MAX_MESH_IN_APPLY_QUEUE 256
#define CHUNK_SIZE 16
#define GRID_SIZE 4

class World : public IRenderable {
private:
	BlockAssetManager* m_blockAssetManager = nullptr;
	ChunkDataGenerator m_chunkDataGenerator;
	MeshGenerator m_meshGenerator;

	std::unordered_map<glm::ivec3, Chunk*> m_chunks;
	glm::ivec3 lastChunkPos;

	std::atomic<bool> m_closeThread;
	std::atomic<bool> m_chunkPosChangedSync;
	std::atomic<glm::ivec3> m_lastChunkPosSync;
	std::thread m_genThread;
	std::vector<glm::ivec3> m_applyMeshQueue;
	std::vector<Mesh> m_disposeMeshQueue;
	std::queue<glm::ivec3> m_missingMeshQueue;

	std::mutex m_meshApplyLock;
	std::mutex m_disposeMeshQueueLock;
	std::mutex m_deleteCreateChunkLock;

	std::vector<Chunk*> GetChunkNeighbours (glm::ivec3 chunkPosition, int& minState);

	int RunInBound (int sizeXZ, int sizeY, glm::ivec3 center, int inset, void(World::* iterator)(int x, int y, int z));
	int RunForChunkInBound (
		int sizeXZ, int sizeY, glm::ivec3 center, int inset, Chunk::State newState, Chunk::State minState,
		void(World::*iterator)(glm::ivec3 pos, Chunk* chunk, std::vector<Chunk*>& neighbours));

	Chunk* CreateChunk(glm::ivec3 chunkPos);
	void TryCreateAndFillChunk (int x, int y, int z);
	void TrySamplingPass (glm::ivec3 pos, Chunk* chunk, std::vector<Chunk*>& neighbours);
	void TrySurfacePassAndMesh (glm::ivec3 pos, Chunk* chunk, std::vector<Chunk*>& neighbours);
	void UnloadFarChunks (glm::ivec3 center, int maxRangeXZ, int maxRangeY);
	void TryGeneratingMissingMesh ();

public:
	~World();

	void Init ();

	void OnUpdate ();

	void ThreadUpdate ();


	void OnPlayerMove (glm::ivec3 position);


	void Render(SurfaceShader& shader, const glm::mat4x4& viewProjection, const Frustum& frustum) override;

};