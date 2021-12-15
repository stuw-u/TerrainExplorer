#include "Mesh.h"
#include <glad/glad.h>
#include <iostream>
#include "RendererUtils.h"

Mesh::~Mesh () {
	// Dispose GPU data
	// Delete buffer
	if(vertsBuffer != 0) { 
		glDeleteBuffers(1, &vertsBuffer);
	}
	if(indexBuffer != 0) {
		glDeleteBuffers(1, &indexBuffer);
	}
}

void Mesh::Render() const {
	if(!IsCompleted()) {
		std::cout << "Cannot draw mesh, some buffers are uninitialised" << std::endl;
		return;
	}

	GLTry(glBindVertexArray(vaoID));
	GLTry(glDrawElements(GL_TRIANGLES, indexBufferSize, GL_UNSIGNED_INT, nullptr));
}

void Mesh::SetData (std::vector<VertexData>& verticies, std::vector<uint32_t>& indicies) {
	if(vertsBuffer != 0) {
		glDeleteBuffers(1, &vertsBuffer);
	}
	if(indexBuffer != 0) {
		glDeleteBuffers(1, &indexBuffer);
	}

	indexBufferSize = indicies.size() * sizeof(uint32_t);
	vertsBufferSize = verticies.size() * sizeof(VertexData);

	GLTry(glGenVertexArrays(1, &vaoID));
	GLTry(glGenBuffers(1, &indexBuffer));
	GLTry(glGenBuffers(1, &vertsBuffer));

	GLTry(glBindVertexArray(vaoID));

	
	GLTry(glBindBuffer(GL_ARRAY_BUFFER, vertsBuffer));
	GLTry(glBufferData(GL_ARRAY_BUFFER, vertsBufferSize, &verticies[0], GL_STATIC_DRAW));

	GLTry(glEnableVertexAttribArray(0));
	GLTry(glVertexAttribPointer(
		/*Att.Index*/0, /*ValueCount*/3, /*Type*/GL_FLOAT, /*Normalize*/false,
		/*BytesPerVert*/sizeof(VertexData), /*Offset*/reinterpret_cast<const void*>(offsetof(VertexData, pos))
	));
	GLTry(glEnableVertexAttribArray(1));
	GLTry(glVertexAttribPointer(
		/*Att.Index*/1, /*ValueCount*/3, /*Type*/GL_FLOAT, /*Normalize*/false,
		/*BytesPerVert*/sizeof(VertexData), /*Offset*/reinterpret_cast<const void*>(offsetof(VertexData, color))
	));
	GLTry(glEnableVertexAttribArray(2));
	GLTry(glVertexAttribPointer(
		/*Att.Index*/2, /*ValueCount*/3, /*Type*/GL_FLOAT, /*Normalize*/false,
		/*BytesPerVert*/sizeof(VertexData), /*Offset*/reinterpret_cast<const void*>(offsetof(VertexData, normal))
	));

	GLTry(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer));
	GLTry(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, &indicies[0], GL_STATIC_DRAW));

	GLTry(glBindVertexArray(0));
}

bool Mesh::IsAllocated () const {
	return vertsBuffer || vertsBufferSize || indexBuffer || indexBufferSize;
}

bool Mesh::IsCompleted () const {
	return vertsBuffer && vertsBufferSize && indexBuffer && indexBufferSize;
}