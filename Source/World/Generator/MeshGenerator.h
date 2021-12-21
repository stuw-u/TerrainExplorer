#pragma once
#include "../../Engine/Mesh.h"
#include "../../External/glm/vec3.hpp"
#include "../Chunk/Chunk.h"
#include "ChunkMeshData.h"
#include <vector>

enum class FaceDirection {Pos_X, Neg_X, Pos_Y, Neg_Y, Pos_Z, Neg_Z};

class MeshGenerator {

    static const glm::vec3 enumToVert[6][4];
    static const glm::vec3 enumToNormal[6];

    void AddFace (std::vector<Chunk*>& chunks, ChunkMeshData& mdat, glm::vec3 blockPos, FaceDirection dir, glm::vec3 color);

    float CalculateAO (std::vector<Chunk*>& chunks, glm::ivec3 p, glm::ivec3 left, glm::ivec3 right, glm::ivec3 up);

public:
    static const glm::ivec3 enumToIntNormal[6];

    void GenerateChunkMesh(Chunk* chunk, std::vector<Chunk*>& chunks);
    
    static BlockAsset* GetCellInChunks (glm::ivec3 blockPos, std::vector<Chunk*>& chunks);

    static int GetPaletteIndexInChunks (glm::ivec3 blockPos, std::vector<Chunk*>& chunks);
};