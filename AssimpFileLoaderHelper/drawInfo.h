#pragma once

#include <string>

struct sVertex
{
	float x, y, z, w;
	float r, g, b, a;
	float nx, ny, nz, nw;
	float u, v;
};

struct sMesh
{
	std::string name;

	unsigned int VAO_ID;

	unsigned int vertexBufferID;
	unsigned int vertexBuffer_Start_Index;
	unsigned int numberOfVertices;

	unsigned int indexBufferID;
	unsigned int indexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	// Mesh info
	float maxX, maxY, maxZ;
	float minX, minY, minZ;

	// The index buffer (CPU side)
	unsigned int* pIndices;
	sVertex* pVertices;
};