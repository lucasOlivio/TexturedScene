#include "components/Collision.h"
#include "common/utils.h"

CollisionComponent::CollisionComponent()
{
	this->m_eBodyType = eBodyType::STATIC;
	this->m_eShape = eShape::UNKNOWN_OR_UNDEFINED;
	this->m_pShape = nullptr;
	this->m_onColStaticEntity = 0;
	this->m_onColKinematicEntity = 0;
}

CollisionComponent::~CollisionComponent()
{
	if (this->m_pShape)
	{
		delete this->m_pShape;
	}
}

bool CollisionComponent::IsActive()
{
	if (this->m_isActive)
	{
		return true;
	}
	else {
		return false;
	}
}

void CollisionComponent::SetActive(bool isActive)
{
	this->m_isActive = isActive;
}

void CollisionComponent::SetShape(eShape shape, sParameterInfo& parameterIn)
{
	// TODO: way to be more dynamic here
	if (shape == eShape::SPHERE)
	{
		sSphere* pSphere = (sSphere*)this->m_pShape;
		pSphere->radius = parameterIn.parameterFloatValue;
	}
	else if (shape == eShape::MESH_OF_TRIANGLES_INDIRECT)
	{
		sMeshOfTriangles_Indirect* pMesh = (sMeshOfTriangles_Indirect*)this->m_pShape;
		pMesh->meshName = parameterIn.parameterStrValue;
	}
	else if (shape == eShape::AABB)
	{
		sAABB* pAABB = (sAABB*)this->m_pShape;

		if (parameterIn.parameterName == "minXYZ")
		{
			pAABB->minXYZ = parameterIn.parameterVec3Value;
		}
		else if (parameterIn.parameterName == "maxXYZ")
		{
			pAABB->maxXYZ = parameterIn.parameterVec3Value;
		}
		else if (parameterIn.parameterName == "offset")
		{
			pAABB->offset = parameterIn.parameterVec3Value;
			pAABB->minXYZ += pAABB->offset;
			pAABB->maxXYZ += pAABB->offset;
		}
	}
	else if (shape == eShape::AABB2D)
	{
		sAABB2D* pAABB2D = (sAABB2D*)this->m_pShape;

		if (parameterIn.parameterName == "minXY")
		{
			pAABB2D->minXY = parameterIn.parameterVec3Value;
		}
		else if (parameterIn.parameterName == "maxXY")
		{
			pAABB2D->maxXY = parameterIn.parameterVec3Value;
		}
		else if (parameterIn.parameterName == "offset")
		{
			pAABB2D->offset = parameterIn.parameterVec3Value;
			pAABB2D->minXY += pAABB2D->offset;
			pAABB2D->maxXY += pAABB2D->offset;
		}
	}
	else if (shape == eShape::OBB)
	{
		sOBB* pOBB = (sOBB*)this->m_pShape;

		if (parameterIn.parameterName == "maxXYZ")
		{
			pOBB->maxXYZ = parameterIn.parameterVec3Value;
		}
		else if (parameterIn.parameterName == "center")
		{
			pOBB->center = parameterIn.parameterVec3Value;
		}
	}
	else if (shape == eShape::GRID)
	{
		sGrid* pGrid = (sGrid*)this->m_pShape;

		if (parameterIn.parameterName == "minXYZ")
		{
			pGrid->minXYZ = parameterIn.parameterVec3Value;
		}
		else if (parameterIn.parameterName == "maxXYZ")
		{
			pGrid->maxXYZ = parameterIn.parameterVec3Value;
		}
		else if (parameterIn.parameterName == "gridFile")
		{
			// TODO: Bring this from config file
			std::string filename = "assets/models/" + parameterIn.parameterStrValue;
			pGrid->gridFile = parameterIn.parameterStrValue;
			bool isLoaded = myutils::TxtToPointVector(filename, 'X', ' ', pGrid->minXYZ, pGrid->maxXYZ, pGrid->vecGrid);
			if (!isLoaded)
			{
				printf("Unable to load '%s' grid!", filename.c_str());
			}
		}
	}
	else
	{
		printf("Shape #%d not implemented yet...", shape);
		return;
	}
}

eShape CollisionComponent::Get_eShape()
{
	return this->m_eShape;
}

eBodyType CollisionComponent::Get_eBodyType()
{
	return this->m_eBodyType;
}

void CollisionComponent::Set_eShape(eShape shape)
{
	this->m_eShape = shape;
	if (this->m_eShape == eShape::SPHERE)
	{
		this->m_pShape = new sSphere();
	}
	else if (this->m_eShape == eShape::MESH_OF_TRIANGLES_INDIRECT)
	{
		this->m_pShape = new sMeshOfTriangles_Indirect();
	}
	else if (this->m_eShape == eShape::AABB)
	{
		this->m_pShape = new sAABB();
	}
	else if (this->m_eShape == eShape::AABB2D)
	{
		this->m_pShape = new sAABB2D();
	}
	else if (this->m_eShape == eShape::GRID)
	{
		this->m_pShape = new sGrid();
	}
	else if (this->m_eShape == eShape::OBB)
	{
		this->m_pShape = new sGrid();
	}
}

void CollisionComponent::GetInfo(sComponentInfo& compInfoOut)
{
	compInfoOut.componentName = "collision";
	compInfoOut.componentParameters.clear();

	this->AddCompParInfo("shape", "int", this->m_eShape, compInfoOut);
	this->AddCompParInfo("bodyType", "int", this->m_eBodyType, compInfoOut);
	this->AddCompParInfo("isActive", "bool", this->m_isActive, compInfoOut);

	// TODO: way to be more dynamic here
	if (this->m_eShape == eShape::SPHERE)
	{
		sSphere* sphere = (sSphere*)this->m_pShape;
		this->AddCompParInfo("radius", "float", sphere->radius, compInfoOut);
	}
	else if (this->m_eShape == eShape::MESH_OF_TRIANGLES_INDIRECT)
	{
		sMeshOfTriangles_Indirect* meshIndirect = (sMeshOfTriangles_Indirect*)this->m_pShape;
		this->AddCompParInfo("meshName", "string", meshIndirect->meshName, compInfoOut);
	}
	else if (this->m_eShape == eShape::AABB)
	{
		sAABB* pAABB = (sAABB*)this->m_pShape;
		this->AddCompParInfo("minXYZ", "vec3", pAABB->minXYZ - pAABB->offset, compInfoOut);
		this->AddCompParInfo("maxXYZ", "vec3", pAABB->maxXYZ - pAABB->offset, compInfoOut);
		this->AddCompParInfo("offset", "vec3", pAABB->offset, compInfoOut);
	}
	else if (this->m_eShape == eShape::AABB2D)
	{
		// TODO: Create vec2 for parameters
		sAABB2D* pAABB2D = (sAABB2D*)this->m_pShape;
		this->AddCompParInfo("minXY", "vec3", glm::vec3(pAABB2D->minXY - pAABB2D->offset, 0) , compInfoOut);
		this->AddCompParInfo("maxXY", "vec3", glm::vec3(pAABB2D->maxXY - pAABB2D->offset, 0), compInfoOut);
		this->AddCompParInfo("offset", "vec3", glm::vec3(pAABB2D->offset, 0), compInfoOut);
	}
	else if (this->m_eShape == eShape::GRID)
	{
		sGrid* pGrid = (sGrid*)this->m_pShape;
		this->AddCompParInfo("minXYZ", "vec3", pGrid->minXYZ, compInfoOut);
		this->AddCompParInfo("maxXYZ", "vec3", pGrid->maxXYZ, compInfoOut);
		this->AddCompParInfo("gridFile", "string", pGrid->gridFile, compInfoOut);
	}
	else if (this->m_eShape == eShape::OBB)
	{
		sOBB* pOBB = (sOBB*)this->m_pShape;
		this->AddCompParInfo("maxXYZ", "vec3", pOBB->maxXYZ, compInfoOut);
		this->AddCompParInfo("center", "vec3", pOBB->center, compInfoOut);
	}

	return;
}

void CollisionComponent::SetParameter(sParameterInfo& parameterIn)
{
	if (parameterIn.parameterName == "shape") {
		this->Set_eShape((eShape)parameterIn.parameterIntValue);
	}
	else if (parameterIn.parameterName == "bodyType") {
		this->m_eBodyType = (eBodyType)parameterIn.parameterIntValue;
	}
	else if (parameterIn.parameterName == "isActive") {
		this->m_isActive = parameterIn.parameterBoolValue;
	}
	else // Any other CollisionComponent parameter should come before here
	{
		// TODO: This can't depend on "shape" been setted first
		this->SetShape(this->m_eShape, parameterIn);
	}
}
