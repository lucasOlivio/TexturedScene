#include "EngineRenderer/Renderer.h"
#include "common/utils.h"
#include "components/Texture.h"
#include "components/Tiling.h"
#include "components/Material.h"
#include <algorithm>

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
	delete m_pCameraSystem;
	delete m_pModelSystem;
	delete m_pLightSystem;
	delete m_pMaterialManager;
}

bool Renderer::Initialize(std::string baseModelsPath,
	std::string baseTexturesPath,
	ShaderManager* pShaderManager,
	uint currShaderID,
	SceneView* pSceneView)
{
	if (m_isInitialized)
	{
		return true;
	}

	m_pSceneView = pSceneView;
	m_pShaderManager = pShaderManager;
	m_currShaderID = currShaderID;

	m_pShaderProgram = m_pShaderManager->GetShaderProgramFromID(m_currShaderID);
	m_pShaderManager->UseShaderProgram(m_currShaderID);

	printf("Initializing engine renderer...\n");
	// All systems that update info in shaders must have the shader info
	// To set their respectives uniforms and attributes
	m_pCameraSystem = new CameraSystem(pSceneView);
	m_pModelSystem = new ModelSystem(m_pShaderManager, pSceneView);
	m_pLightSystem = new LightSystem(m_pShaderManager, pSceneView);
	m_pMaterialManager = new MaterialManager(baseTexturesPath);

	m_isInitialized = true;
	m_isRunning = true;

	return true;
}

void Renderer::Destroy()
{
	if (!m_isInitialized)
	{
		return;
	}

	m_isInitialized = false;

	return;
}

bool Renderer::LoadScene(std::string baseModelsPath)
{
	printf("Loading renderer...\n\n");

	m_pCameraSystem->Initialize();

	bool materialsLoaded = LoadMaterials();
	if (!materialsLoaded)
	{
		CheckEngineError("loading materials");
		return false;
	}

	bool modelsLoaded = m_pModelSystem->LoadModels(baseModelsPath, m_currShaderID);
	if (!modelsLoaded)
	{
		CheckEngineError("loading models");
		return false;
	}

	bool isLSInitialized = m_pLightSystem->Initialize(m_currShaderID);
	if (!isLSInitialized)
	{
		CheckEngineError("setting up lights");
		return false;
	};

	return true;
}

bool Renderer::LoadMaterials()
{
	printf("Loading materials...\n");
	for (m_pSceneView->First("material"); !m_pSceneView->IsDone(); m_pSceneView->Next())
	{
		EntityID entityId = m_pSceneView->CurrentKey();
		MaterialComponent* pMaterial = m_pSceneView->CurrentValue<MaterialComponent>();

		bool isMaterialLoaded = m_pMaterialManager->LoadMaterial(m_pSceneView, pMaterial);
		if (!isMaterialLoaded)
		{
			CheckEngineError(("Not able to load " + pMaterial->materialName).c_str());
			return false;
		}
	}

	return true;
}

void Renderer::UpdateCamera()
{
	using namespace glm;

	// Updating camera view
	mat4 matView = m_pCameraSystem->GetViewMat();
	m_pShaderProgram->SetUniformMatrix4f("matView", matView);
}

void Renderer::RenderAllModels(double deltaTime)
{
	using namespace glm;

	std::vector<TransformComponent*> vecTransparentEntities;

	// TODO: Find a better way to manage these rendering components and the mesh parents thing, is too messy right now
	// Render all "not transparent" models
	for (m_pSceneView->First("model"); !m_pSceneView->IsDone(); m_pSceneView->Next())
	{
		EntityID entityID = m_pSceneView->CurrentKey();
		ModelComponent* pModel = m_pSceneView->CurrentValue<ModelComponent>();
		TransformComponent* pTransform = m_pSceneView->GetComponent<TransformComponent>(entityID, "transform");

		// Bind material
		iComponent* pMaterialComp = m_pSceneView->GetComponentByTag(pModel->material, "material");
		if (pMaterialComp)
		{
			MaterialComponent* pMaterial = (MaterialComponent*)pMaterialComp;

			// Transparent objects must be rendered after
			if (pMaterial->alphaValue < 1.0)
			{
				float distToCamera = distance(pTransform->GetPosition(), m_pCameraSystem->GetCameraPosition());
				pTransform->SetDistanceToCamera(distToCamera);
				vecTransparentEntities.push_back(pTransform);
				continue;
			}

			m_pMaterialManager->BindMaterial(m_pShaderProgram, pMaterial, deltaTime);
		}

		RenderModel(entityID, pModel, pTransform, deltaTime);
	}

	// Render all transparent models from more distant to closer to camera
	std::sort(vecTransparentEntities.begin(), vecTransparentEntities.end(), SortTransformFromCamera);
	for (TransformComponent* pTransform : vecTransparentEntities)
	{
		EntityID entityID = pTransform->GetEntityID();
		ModelComponent* pModel = m_pSceneView->GetComponent<ModelComponent>(entityID, "model");

		// Bind material
		MaterialComponent* pMaterial = m_pSceneView->GetComponentByTag<MaterialComponent>(pModel->material, "material");
		m_pMaterialManager->BindMaterial(m_pShaderProgram, pMaterial, deltaTime);

		RenderModel(entityID, pModel, pTransform, deltaTime);
	}
	vecTransparentEntities.clear();
}

void Renderer::RenderModel(EntityID entityID, ModelComponent* pModel,
	TransformComponent* pTransform, double deltaTime)
{
	using namespace glm;

	iComponent* pTilingComp = m_pSceneView->GetComponent(entityID, "tiling");

	// Default we only draw 1 time in each axis
	vec3 axis = vec3(1.0);
	vec3 offset = vec3(0.0);
	// If has tiling then we draw X times per axis based on the offset
	if (pTilingComp)
	{
		TilingComponent* pTiling = (TilingComponent*)pTilingComp;
		axis = pTiling->GetAxis();
		offset = pTiling->GetOffset();
	}

	pTransform->SetFramePosition();

	// Now go for each axis tiling to draw adding the offset
	for (int x = 0; x < axis[0]; x++)
	{
		for (int y = 0; y < axis[1]; y++)
		{
			for (int z = 0; z < axis[2]; z++)
			{
				vec3 delta = offset;
				delta.x = offset.x * x;
				delta.y = offset.y * y;
				delta.z = offset.z * z;
				pTransform->Move(delta);

				// If the model have a parent we must use the parents transform
				mat4 transformMat = mat4(1.0);
				if (pModel->parentTagName != "")
				{
					TransformComponent* parentTransform = m_pSceneView->GetComponentByTag<TransformComponent>(pModel->parentTagName, "transform");

					transformMat = parentTransform->GetTransformNoScale();
				}

				UpdateTransform(pTransform->GetTransform(transformMat));

				// Model update and render
				pModel->Update(deltaTime);

				sMesh* currMesh = pModel->GetCurrentMesh();
				Draw(pModel->isWireframe,
					pModel->doNotLight,
					pModel->useColorTexture,
					currMesh->VAO_ID,
					currMesh->numberOfIndices);

				// Come back to initial frame place
				pTransform->ResetFramePosition();
			}
		}
	}
}

void Renderer::RenderScene(double deltaTime)
{
	using namespace glm;

	if (!m_isInitialized)
	{
		return;
	}

	UpdateCamera();

	RenderAllModels(deltaTime);

	m_pLightSystem->Update(deltaTime);

	return;
}

void Renderer::UpdateTransform(glm::mat4 matModel)
{
	// Update model matrix on shader
	m_pShaderProgram->SetUniformMatrix4f("matModel", matModel);

	// Also calculate and pass the "inverse transpose" for the model matrix
	glm::mat4 matModelIT = glm::inverse(glm::transpose(matModel));
	m_pShaderProgram->SetUniformMatrix4f("matModel_IT", matModelIT);

	return;
}

void Renderer::Draw(bool isWireFrame,
	bool doNotLight,
	bool useColorTexture,
	int VAO_ID,
	int numberOfIndices)
{
	m_pShaderProgram->IsWireframe(isWireFrame);
	m_pShaderProgram->SetUniformFloat("doNotLight", doNotLight);
	m_pShaderProgram->SetUniformFloat("bUseColorTexture", useColorTexture);

	if (useColorTexture == false)
	{
		printf("");
	}

	glBindVertexArray(VAO_ID); //  enable VAO (and everything else)
	glDrawElements(GL_TRIANGLES,
		numberOfIndices,
		GL_UNSIGNED_INT,
		0);
	glBindVertexArray(0); 			  // disable VAO (and everything else)
}

bool Renderer::IsRunning()
{
	if (m_isRunning)
	{
		return true;
	}
	else
	{
		return false;
	}
}

CameraSystem* Renderer::GetCamera()
{
	return m_pCameraSystem;
}
