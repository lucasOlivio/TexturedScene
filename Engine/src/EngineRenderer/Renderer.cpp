#include "EngineRenderer/Renderer.h"
#include "common/utils.h"
#include "components/Model.h"
#include "components/Transform.h"
#include "components/Texture.h"
#include "components/Tiling.h"
#include "components/Light.h"
#include "components/Material.h"

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

	m_pCameraSystem->Initialize();

	// Loading models into VAO and getting IDs
	bool sceneLoaded = LoadScene(baseModelsPath);
	if (!sceneLoaded)
	{
		return false;
	}

	// Setting up lights
	bool isLSInitialized = m_pLightSystem->Initialize(m_currShaderID);
	if (!isLSInitialized)
	{
		CheckEngineError("setting up lights");
		return false;
	}

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
	m_pCameraSystem->Initialize();

	bool materialsLoaded = LoadMaterials();
	if (!materialsLoaded)
	{
		return false;
	}
	
	bool modelsLoaded = m_pModelSystem->LoadModels(baseModelsPath, m_currShaderID);
	if (!modelsLoaded)
	{
		return false;
	}

	bool isLSInitialized = m_pLightSystem->Initialize(m_currShaderID);
	if (!isLSInitialized)
	{
		CheckEngineError("setting up lights");
		return false;
	}

	return true;
}

bool Renderer::LoadMaterials()
{
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

	// TODO: Find a better way to manage these rendering components and the mesh parents thing, is too messy right now
	for (m_pSceneView->First("model"); !m_pSceneView->IsDone(); m_pSceneView->Next())
	{
		EntityID entityID = m_pSceneView->CurrentKey();
		ModelComponent* pModel = m_pSceneView->CurrentValue<ModelComponent>();
		iComponent* pLightComp = m_pSceneView->GetComponent(entityID, "light");
		iComponent* pTilingComp = m_pSceneView->GetComponent(entityID, "tiling");
		iComponent* pMaterialComp = m_pSceneView->GetComponentByTag(pModel->material, "material");

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

		// Bind material
		if (pMaterialComp)
		{
			MaterialComponent* pMaterial = (MaterialComponent*)pMaterialComp;
			m_pMaterialManager->BindMaterial(m_pShaderProgram, pMaterial);
		}

		// Update uniforms
		TransformComponent* pTransform = m_pSceneView->GetComponent<TransformComponent>(entityID, "transform");

		pTransform->SetFramePosition();

		// Now go for each axis tiling to draw adding the offset
		for (int x = 0; x < axis[0]; x++)
		{
			for (int y = 0; y < axis[1]; y++)
			{
				for (int z = 0; z < axis[2]; z++)
				{
					if (pLightComp)
					{
						// Setting object always behind light
						LightComponent* pLight = (LightComponent*)pLightComp;
						pTransform->SetPosition(pLight->GetPosition() + pLight->GetDirection());
					}

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
						currMesh->VAO_ID,
						currMesh->numberOfIndices);

					// Come back to initial frame place
					pTransform->ResetFramePosition();
				}
			}
		}

		// Unbind for the next model
		if (pMaterialComp)
		{
			m_pMaterialManager->UnbindMaterials();
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
					int VAO_ID, 
					int numberOfIndices)
{
	m_pShaderProgram->IsWireframe(isWireFrame);
	m_pShaderProgram->SetUniformFloat("doNotLight", doNotLight);

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
