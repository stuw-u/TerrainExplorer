#include "MeshGenerator.h"
#include <iostream>

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

const glm::vec3 MeshGenerator::enumToColor[6]{
    glm::vec3(1,0,0), glm::vec3(1,0,0),
    glm::vec3(0,1,0), glm::vec3(0,1,0),
    glm::vec3(0,0,1), glm::vec3(0,0,1)
};

void MeshGenerator::AddFace (ChunkMeshData& mdat, glm::vec3 blockPos, FaceDirection dir) {
    VertexData vdat0, vdat1, vdat2, vdat3;
    vdat0.pos = enumToVert[(int)dir][0] + blockPos;
    vdat1.pos = enumToVert[(int)dir][1] + blockPos;
    vdat2.pos = enumToVert[(int)dir][2] + blockPos;
    vdat3.pos = enumToVert[(int)dir][3] + blockPos;
    vdat0.normal = enumToNormal[(int)dir];
    vdat1.normal = enumToNormal[(int)dir];
    vdat2.normal = enumToNormal[(int)dir];
    vdat3.normal = enumToNormal[(int)dir];
    vdat0.color = enumToColor[(int)dir];
    vdat1.color = enumToColor[(int)dir];
    vdat2.color = enumToColor[(int)dir];
    vdat3.color = enumToColor[(int)dir];
    mdat.verticies.push_back(vdat0);
    mdat.verticies.push_back(vdat1);
    mdat.verticies.push_back(vdat2);
    mdat.verticies.push_back(vdat3);
    mdat.indicies.insert(mdat.indicies.end(), {
        mdat.indiciesCount,
        mdat.indiciesCount + 1,
        mdat.indiciesCount + 2,
        mdat.indiciesCount + 2,
        mdat.indiciesCount + 3,
        mdat.indiciesCount
    });
    mdat.indiciesCount += 4;
}

Mesh MeshGenerator::GenerateChunkMesh (Chunk* chunk) {
	Mesh mesh;
	ChunkMeshData mdata;

	// Fill up the chunk mesh data
    for(uint8_t z = 0; z < 16; z++) {
        for(uint8_t y = 0; y < 16; y++) {
            for(uint8_t x = 0; x < 16; x++) {
                if(chunk->data.GetCell(x, y, z)->id != 0) {
                    for(int i = 0; i < 6; i++) {
                        glm::vec3 normal = enumToNormal[i];
                        BlockAsset* blockAsset = chunk->data.GetCellCheck(x + normal.x, y + normal.y, z + normal.z);
                        if(blockAsset == nullptr || blockAsset->id == 0) {
                            AddFace(mdata, glm::vec3(x, y, z), (FaceDirection)i);
                        }
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