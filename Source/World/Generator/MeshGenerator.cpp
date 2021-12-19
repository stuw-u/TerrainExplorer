#include "MeshGenerator.h"
#include <iostream>
#include <cmath>

constexpr float BLOCK_SIZE = 1.0f;

const glm::vec3 MeshGenerator::enumToVert[6][4]{
    {   // Pos_X
        glm::vec3(BLOCK_SIZE, 0,          BLOCK_SIZE),
        glm::vec3(BLOCK_SIZE, 0,          0),
        glm::vec3(BLOCK_SIZE, BLOCK_SIZE, 0),
        glm::vec3(BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE)
    },{ // Neg_X
        glm::vec3(0, 0,           0),
        glm::vec3(0, 0,           BLOCK_SIZE),
        glm::vec3(0, BLOCK_SIZE,  BLOCK_SIZE),
        glm::vec3(0, BLOCK_SIZE,  0)
    },{ // Pos_Y
        glm::vec3(0,          BLOCK_SIZE, BLOCK_SIZE),
        glm::vec3(BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE),
        glm::vec3(BLOCK_SIZE, BLOCK_SIZE, 0),
        glm::vec3(0,          BLOCK_SIZE, 0)
    },{ // Neg_Y
        glm::vec3(0,          0, 0),
        glm::vec3(BLOCK_SIZE, 0, 0),
        glm::vec3(BLOCK_SIZE, 0, BLOCK_SIZE),
        glm::vec3(0,          0, BLOCK_SIZE)
    },{ // Pos_Z
        glm::vec3(0,          0,          BLOCK_SIZE),
        glm::vec3(BLOCK_SIZE, 0,          BLOCK_SIZE),
        glm::vec3(BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE),
        glm::vec3(0,          BLOCK_SIZE, BLOCK_SIZE)
    },{ // Neg_Z
        glm::vec3(BLOCK_SIZE, 0,          0),
        glm::vec3(0,          0,          0),
        glm::vec3(0,          BLOCK_SIZE, 0),
        glm::vec3(BLOCK_SIZE, BLOCK_SIZE, 0)
    }
};

const glm::vec3 MeshGenerator::enumToNormal[6]{
    glm::vec3(1,0,0), glm::vec3(-1,0,0),
    glm::vec3(0,1,0), glm::vec3(0,-1,0),
    glm::vec3(0,0,1), glm::vec3(0,0,-1)
};

const glm::ivec3 MeshGenerator::enumToIntNormal[6]{
    glm::ivec3(1,0,0), glm::ivec3(-1,0,0),
    glm::ivec3(0,1,0), glm::ivec3(0,-1,0),
    glm::ivec3(0,0,1), glm::ivec3(0,0,-1)
};

void MeshGenerator::AddFace (
    ChunkMeshData& mdat, glm::vec3 blockPos, 
    FaceDirection dir, glm::vec3 color) {
    
    VertexData vdat0, vdat1, vdat2, vdat3;
    vdat0.pos = enumToVert[(int)dir][0] + blockPos;
    vdat1.pos = enumToVert[(int)dir][1] + blockPos;
    vdat2.pos = enumToVert[(int)dir][2] + blockPos;
    vdat3.pos = enumToVert[(int)dir][3] + blockPos;
    vdat0.normal = enumToNormal[(int)dir];
    vdat1.normal = enumToNormal[(int)dir];
    vdat2.normal = enumToNormal[(int)dir];
    vdat3.normal = enumToNormal[(int)dir];
    vdat0.color = color;
    vdat1.color = color;
    vdat2.color = color;
    vdat3.color = color;
    mdat.verticies.push_back(vdat0);
    mdat.verticies.push_back(vdat1);
    mdat.verticies.push_back(vdat2);
    mdat.verticies.push_back(vdat3);
    mdat.indicies.push_back(mdat.indiciesCount + 0);
    mdat.indicies.push_back(mdat.indiciesCount + 1);
    mdat.indicies.push_back(mdat.indiciesCount + 2);
    mdat.indicies.push_back(mdat.indiciesCount + 2);
    mdat.indicies.push_back(mdat.indiciesCount + 3);
    mdat.indicies.push_back(mdat.indiciesCount + 0);
    mdat.indiciesCount += 4;
}

Mesh MeshGenerator::GenerateChunkMesh (Chunk* chunk, std::vector<Chunk*>& chunks) {
	Mesh mesh;
	ChunkMeshData mdata;

	// Fill up the chunk mesh data
    for(uint8_t z = 0; z < 16; z++) {
        for(uint8_t y = 0; y < 16; y++) {
            for(uint8_t x = 0; x < 16; x++) {
                BlockAsset* centerAsset = chunk->data.GetCell(x, y, z);
                if(centerAsset->id == 0)
                    continue;
                glm::vec3 color = glm::vec3(
                    centerAsset->colorR / (float)255,
                    centerAsset->colorG / (float)255,
                    centerAsset->colorB / (float)255);
                for(int i = 0; i < 6; i++) {
                    glm::vec3 normal = enumToNormal[i];
                    glm::ivec3 blockPos = glm::ivec3(x + normal.x, y + normal.y, z + normal.z);

                    BlockAsset* blockAsset = GetCellInChunks(blockPos, chunks);
                    if(blockAsset == nullptr || blockAsset->id == 0) {
                        AddFace(mdata, glm::vec3(x, y, z), (FaceDirection)i, color);
                    }
                }
            }
        }
    }

    //std::cout << "Index buffer size: " << mdata.indicies.size() << std::endl;
    //std::cout << "Verts buffer size: " << mdata.verticies.size() << std::endl;
    

	// Send it all to the gpu
    mesh.SetData(mdata.verticies, mdata.indicies);

	return mesh;
}

BlockAsset* MeshGenerator::GetCellInChunks (glm::ivec3 blockPos, std::vector<Chunk*>& chunks) {
    int size = chunks[13]->data.size(); // 13 is the middle chunk. It's guarenteed that it exists
    int x = blockPos.x, 
        y = blockPos.y, 
        z = blockPos.z;
    int chunkX = floorl(x / (double)size),
        chunkY = floorl(y / (double)size),
        chunkZ = floorl(z / (double)size);
    x -= chunkX * size;
    y -= chunkY * size;
    z -= chunkZ * size;
    int index = (chunkX + 1) + (chunkY + 1) * 3 + (chunkZ + 1) * 9;
    if(chunks[index] == nullptr)
        return nullptr;
    return chunks[index]->data.GetCell(x, y, z);
}
