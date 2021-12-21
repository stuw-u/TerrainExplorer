#define BLOCK_LIST_FILEPATH "../../Ressources/Data/Block/BlockList.txt"
#include "World.h"


void World::Init () {
	// Initialize all needed ressources for the world
	m_blockAssetManager = new BlockAssetManager(BLOCK_LIST_FILEPATH);
	m_chunkDataGenerator = ChunkDataGenerator(0, CHUNK_SIZE, GRID_SIZE, m_blockAssetManager);
	m_chunks = std::unordered_map<glm::ivec3, Chunk*>();

	m_genThread = std::thread(&World::ThreadUpdate, this);

	return;
}

void World::OnUpdate () {
	m_meshUpdateLock.lock();
	for(auto pos : m_chunkToUpdateMesh) {
		auto chunk_it = m_chunks.find(pos);
		if(chunk_it == m_chunks.end()) continue;

		Chunk* chunk = chunk_it->second;
		if(chunk->CanRender())
			continue;

		Mesh mesh;
		mesh.SetData(chunk->meshData.verticies, chunk->meshData.indicies);
		chunk->SetMesh(mesh);
		chunk->meshData.Dispose();
	}
	m_chunkToUpdateMesh.clear();
	m_estimatedChunkInMeshQueue = 0;
	m_meshUpdateLock.unlock();

	m_deleteLock.lock();
	for(auto pos : m_chunkToDelete) {
		auto chunk_it = m_chunks.find(pos);
		if(chunk_it == m_chunks.end()) continue;

		Chunk* chunk = chunk_it->second;
		m_chunks.erase(pos);
		delete chunk;
	}
	m_chunkToDelete.clear();
	m_deleteLock.unlock();
}

void World::OnPlayerMove (glm::ivec3 position) {
	glm::ivec3 chunkPos = glm::floor(static_cast<glm::dvec3>(position) / (double)CHUNK_SIZE);
	if(chunkPos == lastChunkPos) {
		return;
	}
	lastChunkPos = chunkPos;

	m_lastChunkPosSync = lastChunkPos;
	m_chunkPosChangedSync = true;
}

void World::ThreadUpdate () {

	// *Passes		*Neighbours required	*Neighbours min pass
	// Grid data	0						-
	// Chunk data	1						Grid data
	// Surface		2						Sampling
	// Mesh			2						Sampling

	while(!m_closeThread) {

		// If we DONT'T have a position change
		if(!m_chunkPosChangedSync.load()) {
			
			TryGeneratingMissingMesh();

			std::this_thread::sleep_for(std::chrono::microseconds(50));
			continue;
		}

		// If we have a position change
		glm::ivec3 chunkPos = m_lastChunkPosSync.load();
		m_chunkPosChangedSync = false;

		int boundXZ = 13;
		int boundY = 5;

		// Create a debug chunks for testing
		// Generate grid data
		DebugChrono chunkChrono;
		chunkChrono.Start();
		int index = RunInBound(boundXZ, boundY, chunkPos, 0, &World::TryCreateAndFillChunk);
		chunkChrono.StopAndPrint("GridDataPass / Chunk : ", index);


		// Generate chunk data
		DebugChrono samplingChrono;
		samplingChrono.Start();
		index = RunForChunkInBound(boundXZ, boundY, chunkPos, 1, Chunk::State::Sampling, Chunk::State::GridData, &World::TrySamplingPass);
		samplingChrono.StopAndPrint("ChunkDataPass / Chunk : ", index);


		// Generate surface + mesh
		DebugChrono surfaceMeshChrono;
		surfaceMeshChrono.Start();
		index = RunForChunkInBound(boundXZ, boundY, chunkPos, 2, Chunk::State::Complete, Chunk::State::Sampling, &World::TrySurfacePassAndMesh);
		surfaceMeshChrono.StopAndPrint("SurfacePass&MeshGeneration / Chunk : ", index);


		UnloadFarChunks(chunkPos, boundXZ + 1, boundXZ + 1);


		std::this_thread::sleep_for(std::chrono::microseconds(50));
	}
}

World::~World () {
	m_closeThread = true;
	m_genThread.join();
	delete m_blockAssetManager;
}

Chunk* World::CreateChunk (glm::ivec3 chunkPos) {
	Chunk* newChunk = new Chunk(CHUNK_SIZE, GRID_SIZE, chunkPos, m_blockAssetManager);
	m_chunks.insert(std::pair<glm::ivec3, Chunk*>(chunkPos, newChunk));
	return newChunk;
}

void World::Render(SurfaceShader& shader, const glm::mat4x4& viewProjection, const Frustum& frustum) {
	m_deleteLock.lock();
	for(auto& element : m_chunks) {
		if(!element.second->CanRender()) {
			continue;
		}
		if(frustum.IsBoxInFrustum(element.second->GetBox())) {
			element.second->Render(shader, viewProjection);
		}
	}
	m_deleteLock.unlock();
}

std::vector<Chunk*> World::GetChunkNeighbours (glm::ivec3 chunkPosition, int& minState) {
	std::vector<Chunk*> neighbours = std::vector<Chunk*>(27);
	for(int k = -1; k <= 1; k++) {
		for(int j = -1; j <= 1; j++) {
			for(int i = -1; i <= 1; i++) {
				int index = (i + 1) + (j + 1) * 3 + (k + 1) * 9;
				auto neighbour_it = m_chunks.find(
					glm::ivec3(chunkPosition.x + i, chunkPosition.y + j, chunkPosition.z + k)
				);
				if(neighbour_it == m_chunks.end()) {
					minState = 0;
					neighbours[index] = nullptr;
				}
				else {
					int neighbourState = static_cast<int>(neighbour_it->second->state);
					minState = (minState < neighbourState) ? minState : neighbourState;
					neighbours[index] = neighbour_it->second;
				}
			}
		}
	}
	return neighbours;
}

int World::RunInBound (int sizeXZ, int sizeY, glm::ivec3 center, int inset, void(World::*iterator)(int x, int y, int z)) {
	int xMin = center.x - sizeXZ + inset;
	int xMax = center.x + sizeXZ - inset;
	int yMin = center.y - sizeY + inset;
	int yMax = center.y + sizeY - inset;
	int zMin = center.z - sizeXZ + inset;
	int zMax = center.z + sizeXZ - inset;

	int index = 0;
	for(int x = xMin; x <= xMax; x++) {
		for(int y = yMin; y <= yMax; y++) {
			for(int z = zMin; z <= zMax; z++) {
				std::invoke(iterator, this, x, y, z);
				index++;
			}
		}
	}
	return index;
}

int World::RunForChunkInBound (
	int sizeXZ, int sizeY, glm::ivec3 center, int inset, Chunk::State newState, Chunk::State minState,
	void(World::* iterator)(glm::ivec3 pos, Chunk* chunk, std::vector<Chunk*>& neighbours))
{	
	int xMin = center.x - sizeXZ + inset;
	int xMax = center.x + sizeXZ - inset;
	int yMin = center.y - sizeY + inset;
	int yMax = center.y + sizeY - inset;
	int zMin = center.z - sizeXZ + inset;
	int zMax = center.z + sizeXZ - inset;

	int index = 0;
	for(int x = xMin; x <= xMax; x++) {
		for(int y = yMin; y <= yMax; y++) {
			for(int z = zMin; z <= zMax; z++) {
				auto chunk_it = m_chunks.find(glm::ivec3(x, y, z));
				if(chunk_it == m_chunks.end()) continue;

				Chunk* chunk = chunk_it->second;
				if(chunk->state >= newState) continue;


				glm::ivec3 chunkPos = chunk->chunkPosition;

				int minNeighbourState = static_cast<int>(Chunk::State::Complete);
				std::vector<Chunk*> neighbours = GetChunkNeighbours(chunkPos, minNeighbourState);
				if(minNeighbourState >= static_cast<int>(minState)) {
					std::invoke(iterator, this, chunkPos, chunk, neighbours);
				}
				index++;
			}
		}
	}
	return index;
}


void World::TryCreateAndFillChunk (int x, int y, int z) {
	auto chunk_it = m_chunks.find(glm::ivec3(x, y, z));
	if(chunk_it != m_chunks.end()) return;

	Chunk* chunk = CreateChunk(glm::ivec3(x, y, z));
	m_chunkDataGenerator.GridDataPass(chunk);

	chunk->state = Chunk::State::GridData;
}

void World::TrySamplingPass(glm::ivec3 pos, Chunk* chunk, std::vector<Chunk*>& neighbours) {
	m_chunkDataGenerator.ChunkDataPass(chunk, neighbours);
	chunk->state = Chunk::State::Sampling;
}

void World::TrySurfacePassAndMesh(glm::ivec3 pos, Chunk* chunk, std::vector<Chunk*>& neighbours) {

	// Surface pass
	m_chunkDataGenerator.SurfacePass(chunk, neighbours);
	chunk->state = Chunk::State::Surface;

	// Check if mesh is allowed
	int meshCount = m_estimatedChunkInMeshQueue.load();
	if(meshCount >= MAX_MESH) {

		// We cannot, add to queue
		m_missingMesh.push(pos);
		return;
	}

	// We can, generate mesh and change state
	m_meshGenerator.GenerateChunkMesh(chunk, neighbours);
	chunk->state = Chunk::State::Complete;

	// Request main thread to send mesh to GPU
	m_meshUpdateLock.lock();
	m_chunkToUpdateMesh.push_back(pos);
	m_meshUpdateLock.unlock();
}

void World::TryGeneratingMissingMesh () {

	int meshCount = m_estimatedChunkInMeshQueue.load();
	int totalNewMesh = 0;
	Chunk::State minState = Chunk::State::Sampling;

	while(meshCount < MAX_MESH && m_missingMesh.size() > 0) {
		
		// Find a reference to chunk with missing mesh
		glm::ivec3 pos = m_missingMesh.front();
		m_missingMesh.pop();

		auto chunk_it = m_chunks.find(pos);
		if(chunk_it != m_chunks.end()) return;
		Chunk* chunk = chunk_it->second;

		// Ensure neighbours are valid
		int minNeighbourState = static_cast<int>(Chunk::State::Complete);
		std::vector<Chunk*> neighbours = GetChunkNeighbours(pos, minNeighbourState);
		if(minNeighbourState < static_cast<int>(minState)) { continue; } // Invalid neighbours

		// Generate the mesh and change state
		m_meshGenerator.GenerateChunkMesh(chunk, neighbours);
		chunk->state = Chunk::State::Complete;

		// Request main thread to send mesh to GPU
		m_meshUpdateLock.lock();
		m_chunkToUpdateMesh.push_back(pos);
		m_meshUpdateLock.unlock();

		meshCount++;
		totalNewMesh++;
	}
	m_estimatedChunkInMeshQueue += totalNewMesh;
}

void World::UnloadFarChunks (glm::ivec3 center, int maxRangeXZ, int maxRangeY) {
	m_deleteLock.lock();
	for(auto &element : m_chunks) {
		glm::ivec3 pos = element.first;
		int distX = glm::abs(center.x - pos.x);
		int distY = glm::abs(center.y - pos.y);
		int distZ = glm::abs(center.z - pos.z);
		
		if(distY > maxRangeY || glm::max(distX, distZ) > maxRangeXZ) {
			m_chunkToDelete.push_back(element.first);
		}
	}
	m_deleteLock.unlock();
}