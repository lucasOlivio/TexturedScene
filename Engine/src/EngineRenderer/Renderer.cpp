#include "EngineRenderer/Renderer.h"
#include "common/utils.h"
#include "components/Model.h"
#include "components/Transform.h"
#include "components/Texture.h"
#include "components/Tiling.h"
#include "components/Light.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
	delete this->m_pCameraSystem;
	delete this->m_pModelSystem;
	delete this->m_pLightSystem;
}

bool Renderer::Initialize(std::string baseModelsPath,
						  std::string baseTexturesPath,
						  ShaderManager* pShaderManager,
						  uint currShaderID,
						  SceneView* pSceneView)
{
	if (this->m_isInitialized)
	{
		return true;
	}

	this->m_pSceneView = pSceneView;
	this->m_pShaderManager = pShaderManager;
	this->m_currShaderID = currShaderID;

	this->m_pShaderProgram = this->m_pShaderManager->GetShaderProgramFromID(this->m_currShaderID);
	this->m_pShaderManager->UseShaderProgram(this->m_currShaderID);

	printf("Initializing engine renderer...\n");
	// All systems that update info in shaders must have the shader info
	// To set their respectives uniforms and attributes
	this->m_pCameraSystem = new CameraSystem(pSceneView);
	this->m_pModelSystem = new ModelSystem(this->m_pShaderManager, pSceneView);
	this->m_pLightSystem = new LightSystem(this->m_pShaderManager, pSceneView);

	this->m_pCameraSystem->Initialize();

	// Initializing and loading all textures
	this->m_pTextureManager = new cBasicTextureManager();
	this->m_pTextureManager->SetBasePath(baseTexturesPath);

	// Loading models into VAO and getting IDs
	bool sceneLoaded = this->LoadScene(baseModelsPath);
	if (!sceneLoaded)
	{
		return false;
	}

	// Setting up lights
	bool isLSInitialized = this->m_pLightSystem->Initialize(this->m_currShaderID);
	if (!isLSInitialized)
	{
		CheckEngineError("setting up lights");
		return false;
	}

	this->m_isInitialized = true;
	this->m_isRunning = true;

    return true;
}

void Renderer::Destroy()
{
	if (!this->m_isInitialized)
	{
		return;
	}

	this->m_isInitialized = false;

	return;
}

bool Renderer::LoadScene(std::string baseModelsPath)
{
	this->m_pCameraSystem->Initialize();
	
	bool modelsLoaded = this->m_pModelSystem->LoadModels(baseModelsPath, this->m_currShaderID);
	if (!modelsLoaded)
	{
		return false;
	}

	bool textureLoaded = this->LoadTextures();
	if (!textureLoaded)
	{
		return false;
	}

	bool isLSInitialized = this->m_pLightSystem->Initialize(this->m_currShaderID);
	if (!isLSInitialized)
	{
		CheckEngineError("setting up lights");
		return false;
	}

	return true;
}

bool Renderer::LoadTextures()
{
	for (this->m_pSceneView->First("texture"); !this->m_pSceneView->IsDone(); this->m_pSceneView->Next())
	{
		EntityID entityID = this->m_pSceneView->CurrentKey();
		TextureComponent* pTexture = this->m_pSceneView->GetComponent<TextureComponent>(entityID, "texture");

		bool textureLoaded = this->m_pTextureManager->Create2DTextureFromBMPFile(pTexture->fileName, 
																				 pTexture->generateMIPMap);
		if (!textureLoaded)
		{
			CheckEngineError(this->m_pTextureManager->GetLastError().c_str());
			return false;
		}
	}

	return true;
}

void Renderer::BindTexture(std::string fileName, uint samplerId)
{
	uint textureId = this->m_pTextureManager->getTextureIDFromName(fileName);
	uint textureUnit = this->m_pTextureManager->GetUnitFromSamplerId(samplerId);
	std::string samplerName = this->m_pTextureManager->GetSamplerName(samplerId);

	this->m_pShaderProgram->BindTexture(textureUnit, textureId);

	//uniform sampler2D textureX;
	this->m_pShaderProgram->SetUniformInt(samplerName.c_str(), samplerId);
}

void Renderer::UpdateCamera()
{
	using namespace glm;

	// Updating camera view
	mat4 matView = this->m_pCameraSystem->GetViewMat();
	this->m_pShaderProgram->SetUniformMatrix4f("matView", matView);
}

void Renderer::RenderAllModels(double deltaTime)
{
	using namespace glm;

	// TODO: Find a better way to manage these rendering components and the mesh parents thing, is too messy right now
	for (this->m_pSceneView->First("model"); !this->m_pSceneView->IsDone(); this->m_pSceneView->Next())
	{
		EntityID entityID = this->m_pSceneView->CurrentKey();
		ModelComponent* pModel = this->m_pSceneView->CurrentValue<ModelComponent>();
		iComponent* pLightComp = this->m_pSceneView->GetComponent(entityID, "light");
		iComponent* pTilingComp = this->m_pSceneView->GetComponent(entityID, "tiling");
		// Default we only draw 1 time in each axis
		vec3 axis = vec3(1.0, 1.0, 1.0);
		vec3 offset = vec3(0.0, 0.0, 0.0);
		// If has tiling then we draw X times per axis based on the offset
		if (pTilingComp)
		{
			TilingComponent* pTiling = (TilingComponent*)pTilingComp;
			axis = pTiling->GetAxis();
			offset = pTiling->GetOffset();
		}
		
		// Texture set
		iComponent* pTextureComp = this->m_pSceneView->GetComponent(entityID, "texture");
		if (pTextureComp)
		{
			TextureComponent* pTexture = (TextureComponent*)pTextureComp;

			this->BindTexture(pTexture->fileName, pTexture->samplerId);
		}

		// Update uniforms
		TransformComponent* pTransform = this->m_pSceneView->GetComponent<TransformComponent>(entityID, "transform");

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
						TransformComponent* parentTransform = this->m_pSceneView->GetComponentByTag<TransformComponent>(pModel->parentTagName, "transform");

						transformMat = parentTransform->GetTransformNoScale();
					}

					this->UpdateTransform(pTransform->GetTransform(transformMat));

					// Model update and render
					pModel->Update(deltaTime);

					sMesh* currMesh = pModel->GetCurrentMesh();
					this->Draw(pModel->isWireframe,
						pModel->doNotLight,
						pModel->useVertexColour,
						currMesh->VAO_ID,
						currMesh->numberOfIndices);

					// Come back to initial frame place
					pTransform->ResetFramePosition();
				}
			}
		}
	}
}

void Renderer::RenderScene(double deltaTime)
{
	using namespace glm;

	if (!this->m_isInitialized)
	{
		return;
	}

	this->UpdateCamera();

	this->RenderAllModels(deltaTime);

	return;
}

void Renderer::UpdateTransform(glm::mat4 matModel)
{
	// Update model matrix on shader
	this->m_pShaderProgram->SetUniformMatrix4f("matModel", matModel);

	// Also calculate and pass the "inverse transpose" for the model matrix
	glm::mat4 matModelIT = glm::inverse(glm::transpose(matModel));
	this->m_pShaderProgram->SetUniformMatrix4f("matModel_IT", matModelIT);

	return;
}

void Renderer::Draw(bool isWireFrame,
					bool doNotLight,
					bool useVertexColour,
					int VAO_ID, 
					int numberOfIndices)
{
	this->m_pShaderProgram->IsWireframe(isWireFrame);
	this->m_pShaderProgram->SetUniformFloat("doNotLight", doNotLight);
	this->m_pShaderProgram->SetUniformFloat("bUseVertexColour", useVertexColour);

	glBindVertexArray(VAO_ID); //  enable VAO (and everything else)
	glDrawElements(GL_TRIANGLES,
		numberOfIndices,
		GL_UNSIGNED_INT,
		0);
	glBindVertexArray(0); 			  // disable VAO (and everything else)
}

bool Renderer::IsRunning()
{
	if (this->m_isRunning)
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
	return this->m_pCameraSystem;
}
