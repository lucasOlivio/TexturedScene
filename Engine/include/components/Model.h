#pragma once

#include "Component.h"
#include "drawInfo.h"
#include "EngineRenderer/iShaderInfo.h"
#include <string>

class ModelComponent : public Component
{
public:
	ModelComponent();
	virtual ~ModelComponent() {};

	std::vector<std::string> models;
	std::string collisionName;
	std::string material;
	std::string friendlyName;
	std::string parentTagName;
	bool isWireframe;
	bool doNotLight;
	bool useColorTexture;

	sMesh* pCollisionMesh;

	void SetActive(bool isActive);
	bool IsActive();

	void SetMeshes(std::vector<sMesh*> meshes);
	void SetMesh(uint index, sMesh* pMesh);
	std::vector<sMesh*> GetMeshes();

	void AddMesh(sMesh* pMesh);
	sMesh* GetMesh(uint index);
	sMesh* GetCurrentMesh();

	virtual void GetInfo(sComponentInfo& compInfoOut);
	virtual void SetParameter(sParameterInfo& parameterIn);

	void Update(double deltaTime);

private:
	bool m_isActive;
	int m_currFrame;
	double m_elapsedTime;

	std::vector <sMesh*> m_pMeshes;

	void m_UpdateFrames(double deltaTime);
};