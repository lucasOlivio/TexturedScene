#pragma once

#include "common/utils.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

// Shape types for collisions
enum eShape
{
	UNKNOWN_OR_UNDEFINED,
	SPHERE,
	PLANE,
	TRIANGLE,
	AABB,
	CAPSULE,
	MESH_OF_TRIANGLES_INDIRECT,
	MESH_OF_TRIANGLES_LOCAL_VERTICES,
	GRID,
	OBB,
	AABB2D
};

enum eBodyType
{
	STATIC,
	KINEMATIC,
	DYNAMIC
};

// STARTOF: Shape type structures
#pragma region Shape_Type_Structures
struct sSphere
{
	sSphere(float radius)
	{
		this->radius = radius;
	}
	sSphere()
	{
		this->radius = 0.0f;
	}

	float radius = 0.0f;
};

struct sPlane
{
	sPlane(glm::vec3 normal)
	{
		this->normalToSurface = normal;
	}
	sPlane()
	{
		this->normalToSurface = glm::vec3(0.0f);
	}

	glm::vec3 normalToSurface;
};

struct sTriangle
{
	static const unsigned int NUMBER_OF_VERTICES = 3;
	sTriangle()
	{
		this->vertices[0] = glm::vec3(0.0f);
		this->vertices[1] = glm::vec3(0.0f);
		this->vertices[2] = glm::vec3(0.0f);
	}
	sTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
	{
		this->vertices[0] = v0;
		this->vertices[1] = v1;
		this->vertices[2] = v2;
		this->calcNormal();
	}
	sTriangle(glm::vec3 vertices[NUMBER_OF_VERTICES])
	{
		this->vertices[0] = vertices[0];
		this->vertices[1] = vertices[1];
		this->vertices[2] = vertices[2];
		this->calcNormal();
	}
	glm::vec3 vertices[NUMBER_OF_VERTICES];

	void calcNormal(void)
	{
		glm::vec3 v0_1 = this->vertices[1] - this->vertices[0];
		glm::vec3 v0_2 = this->vertices[2] - this->vertices[1];
		this->normal = glm::normalize(glm::cross(v0_1, v0_2));
		return;
	}
	glm::vec3 normal = glm::vec3(0.0f);
	glm::vec3 getCentre(void)
	{
		return ((this->vertices[0] + this->vertices[1] + this->vertices[2]) / (float)NUMBER_OF_VERTICES);
	}
};

struct sAABB
{
	sAABB(glm::vec3 minXYZ, glm::vec3 maxXYZ, glm::vec3 offset)
	{
		this->minXYZ = minXYZ + offset;
		this->maxXYZ = maxXYZ + offset;
		this->offset = offset;
	}
	sAABB()
	{
		this->minXYZ = glm::vec3(0.0f);
		this->maxXYZ = glm::vec3(0.0f);
		this->offset = glm::vec3(0.0f);
	}

	glm::vec3 minXYZ = glm::vec3(0.0f);
	glm::vec3 maxXYZ = glm::vec3(0.0f);
	glm::vec3 offset = glm::vec3(0.0f);
};

struct sAABB2D
{
	sAABB2D(glm::vec2 minXY, glm::vec2 maxXY, glm::vec2 offset)
	{
		this->minXY = minXY + offset;
		this->maxXY = maxXY + offset;
		this->offset = offset;
	}
	sAABB2D()
	{
		this->minXY = glm::vec2(0.0f);
		this->maxXY = glm::vec2(0.0f);
		this->offset = glm::vec2(0.0f);
	}

	glm::vec2 minXY = glm::vec2(0.0f);
	glm::vec2 maxXY = glm::vec2(0.0f);
	glm::vec2 offset = glm::vec2(0.0f);
};

struct sOBB {
	sOBB(glm::vec3 center, glm::mat4 matRotation, glm::vec3 maxXYZ)
	{
		this->center = glm::vec3(0.0f);
		this->maxXYZ = glm::vec3(0.0f);
	}

	sOBB()
	{
		this->center = glm::vec3(0.0f);
		this->maxXYZ = glm::vec3(0.0f);
	}

	glm::vec3 center;
	glm::vec3 maxXYZ;      // Positive halfwidth extents of OBB along each axis
};

struct sCapsule
{
	sCapsule(float halfLength, float radius)
	{
		this->halfLength = halfLength;
		this->radius = radius;
	}
	sCapsule()
	{
		this->halfLength = 0;
		this->radius = 0;
	}

	float halfLength;
	float radius;
};

// This corresponds to a mesh that's been loaded by something 
//	that might use sMesh, which has all the vertex info
struct sMeshOfTriangles_Indirect
{
	sMeshOfTriangles_Indirect(std::string meshName)
	{
		this->meshName = meshName;
	}
	sMeshOfTriangles_Indirect()
	{
		this->meshName = "";
	}

	//	These are used to look up mesh information:
	std::string meshName;
};

// Here the vertices are stored "locally"
struct sMeshOfTriangles_LocalVertices
{
	// This triangle stores the vertices and normals here
	std::vector< sTriangle > vecTriangles;
};

// Here we have a vector of points representing walls in a matrix
struct sGrid
{
	std::vector<glm::vec3> vecGrid;
	std::string gridFile;
	glm::vec3 minXYZ;
	glm::vec3 maxXYZ;

	sGrid(std::string filename, glm::vec3 minXYZ, glm::vec3 maxXYZ)
	{
		// Load the grid from a txt file with 'X' as 1 (walls)
		// and ' ' as 0 (paths)
		bool loaded = myutils::TxtToPointVector(filename, 'X', ' ', minXYZ, maxXYZ, this->vecGrid);
		if (loaded)
		{
			this->gridFile = filename;
			this->minXYZ = minXYZ;
			this->maxXYZ = maxXYZ;
		}
		else
		{
			this->vecGrid.clear();
			this->minXYZ = glm::vec3(0.0f);
			this->maxXYZ = glm::vec3(0.0f);
		}
	}

	sGrid()
	{
		this->vecGrid.clear();
		this->minXYZ = glm::vec3(0.0f);
		this->maxXYZ = glm::vec3(0.0f);
	}
};

#pragma endregion Shape_Type_Structures
// ENDOF: Shape type structures
