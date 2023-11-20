#include "Engine.h"
#include "scene/Scene.h"
#include "common/ConfigReadWriteFactory.h"
#include "events/KeyWrapper.h"
#include "common/utils.h"
#include "common/constants.h"


// TODO: All this should come from a config file
std::string baseConfigsPath = "configs/";
std::string sceneFilePath = "";
std::string baseShadersPath = "assets/shaders/";
std::string baseModelPath = "assets/models/";
std::string baseAudioPath = "assets/audios/";
std::string baseTexturesPath = "assets/textures/";
std::string shaderProgramName = "Shader01";
uint windowWidth = 1080;
uint windowHeight = 720;
std::string windowName = "";

Engine* Engine::m_pInstance = nullptr;

// TODO: Map all data used by the other classes from the scene
// to pass only the needed data and decouple all from scene

Engine::Engine()
{
	this->m_isInitialized = false;
	this->m_isRunning = false;

	this->m_lastTime = 0.0;

	// Singleton
	assert(!this->m_pInstance);
	m_pInstance = this;
}

Engine::~Engine()
{
	this->m_pInstance = nullptr;
}

Engine* Engine::Get()
{
	if (Engine::m_pInstance == nullptr)
	{
		Engine::m_pInstance = new Engine();
	}
	return Engine::m_pInstance;
}

bool Engine::Initialize(const std::string& sceneName)
{
	if (this->m_isInitialized)
	{
		// Already initialized
		return true;
	}

	// Events
	this->m_pCollisionEvent = new CollisionEvent();
	this->m_pKeyEvent = new KeyEvent();
	KeyWrapper::SetKeyEvent(this->m_pKeyEvent);

	printf("Initializing creation of scene '%s'\n", sceneName.c_str());

	this->m_pScene = new Scene(this->m_pKeyEvent, this->m_pCollisionEvent);
	iConfigReadWrite* pConfigrw = ConfigReadWriteFactory::CreateConfigReadWrite("json");

	// TODO: This should come from a config file
	// Load all config variables 
	sceneFilePath = baseConfigsPath + sceneName + ".json";
	windowName = sceneName;

	printf("Loading configs...\n");
	// Load scene components and entities from database file
	bool isSceneLoaded = pConfigrw->ReadScene(sceneFilePath, this->m_pScene);
	if (!isSceneLoaded)
	{
		CheckEngineError("Scene loading");
		return false;
	}

	delete pConfigrw; // Used only to load all configs

	printf("Creating systems...\n");
	this->m_pShaderManager = new ShaderManager(baseShadersPath);
	this->m_pRenderer = new Renderer();
	this->m_pEditor = new Editor(this->m_pKeyEvent, this->m_pScene, this);
	this->m_pPhysics = new Physics(this->m_pScene, this->m_pCollisionEvent);
	this->m_pMediaPlayer = MediaPlayer::Get();

	this->m_pWindowSystem = new WindowSystem(this->m_pShaderManager);

	printf("Initializing systems...\n");
	// Initializes all systems

	bool isWSInitialized = this->m_pWindowSystem->Initialize(windowWidth,
		windowHeight,
		windowName,
		KeyWrapper::KeyCallback);
	if (!isWSInitialized)
	{
		CheckEngineError("initializing Window system");
		return false;
	}

	printf("Creating shaders...\n");
	// Creates main shader program
	bool isShaderCreated = this->m_pShaderManager->AddShaderProgram(shaderProgramName);
	if (!isShaderCreated)
	{
		CheckEngineError("creating shader program");
		return false;
	}
	this->m_currShaderID = this->m_pShaderManager->GetIDFromShaderProgramName(shaderProgramName);
	this->m_pShaderManager->UseShaderProgram(this->m_currShaderID);

	bool isERInitialized = this->m_pRenderer->Initialize(baseModelPath,
														 baseTexturesPath,
														 this->m_pShaderManager,
														 this->m_currShaderID,
														 this->m_pScene);
	if (!isERInitialized)
	{
		CheckEngineError("Engine renderer initialization");
		return false;
	}

	bool isMediaInit = this->m_pMediaPlayer->Initialize(baseAudioPath, this->m_pScene);
	if (!isMediaInit)
	{
		CheckEngineError("Engine media player initialization");
		return false;
	}

	m_lastTime = glfwGetTime();

	this->m_isInitialized = true;
	printf("Scene '%s' created scussesfully!\n", sceneName.c_str());

    return true;
}

void Engine::Run()
{
	if (!this->m_isInitialized)
	{
		return;
	}

	this->m_isRunning = true;

	while (this->IsRunning())
	{
		double fixedDeltaTime = this->GetFixedDeltaTime();

		this->m_pWindowSystem->NewFrame(this->m_currShaderID);
		this->m_pPhysics->NewFrame();

		this->Update(fixedDeltaTime);

		this->m_pWindowSystem->EndFrame();

		this->m_pScene->ClearListenerDeleted();
		this->m_pScene->ClearDeleted();
	}
}

void Engine::Update(double fixedDeltaTime)
{
	this->m_pPhysics->Update(fixedDeltaTime);
	this->m_pMediaPlayer->Update(fixedDeltaTime);
	this->m_pWindowSystem->UpdateUL(this->m_currShaderID);
	this->m_pRenderer->RenderScene(fixedDeltaTime);
}

bool Engine::IsRunning()
{
	if (this->m_isRunning
		&& this->m_pRenderer->IsRunning()
		&& !this->m_pWindowSystem->WindowShouldClose())
	{
		return true;
	}
	else
	{
		return false;
	}
}

double Engine::GetFixedDeltaTime()
{
	double currentTime = glfwGetTime();
	double deltaTime = currentTime - this->m_lastTime;
	this->m_lastTime = currentTime;

	// Debugging performance:
	// printf("delta real: %.2f\n", deltaTime);

	// Clamp delta time to the maximum frame time
	if (deltaTime > FRAME_DURATION) {
		deltaTime = FRAME_DURATION;
	}

	// Add the frame time to the list
	this->m_frameTimes.push_back(deltaTime);

	// Limit the number of frames
	const size_t maxFrameCount = 60; // Store frame times for a second
	if (this->m_frameTimes.size() > maxFrameCount) {
		this->m_frameTimes.erase(this->m_frameTimes.begin());
	}

	// Calculate the average frame time
	double averageFrameTime = 0;
	for (double time : this->m_frameTimes) {
		averageFrameTime += time;
	}
	averageFrameTime /= this->m_frameTimes.size();

	return averageFrameTime;
}

void Engine::Exit()
{
	if (!this->m_isInitialized)
	{
		return;
	}

	this->m_pRenderer->Destroy();
	this->m_pWindowSystem->Destroy();

	this->m_isInitialized = false;
	this->m_isRunning = false;

	delete this->m_pRenderer;
	delete this->m_pEditor;
	delete this->m_pScene;

	delete this->m_pKeyEvent;

	return;
}

GLFWwindow* Engine::GetWindow()
{
	return this->m_pWindowSystem->GetWindow();
}

glm::mat4 Engine::GetWindowProjectionMat()
{
	return this->m_pWindowSystem->GetProjection();
}

void Engine::GetWindowSize(int& windowHeight, int& windowWidth)
{
	this->m_pWindowSystem->GetWindowSize(windowHeight, windowWidth);
}

glm::mat4 Engine::GetViewMat()
{
	return this->m_pRenderer->GetCamera()->GetViewMat();
}

void Engine::SetRunning(bool isRunning)
{
	this->m_isRunning = isRunning;
}

void Engine::ChangeMode()
{
	bool editorRunning = this->m_pEditor->IsRunning();

	if (editorRunning)
	{
		// Entering play mode
		this->SaveScene();

		this->m_pEditor->SetRunning(false);
		this->m_pPhysics->SetRunning(true);
		this->m_pScene->SetPlaying(true);

		this->LoadScene();
	}
	else
	{
		this->m_pEditor->SetRunning(true);
		this->m_pPhysics->SetRunning(false);
		this->m_pScene->SetPlaying(false);

		this->LoadScene();
	}

	return;
}

void Engine::SaveScene(std::string filePath)
{
	iConfigReadWrite* pConfigrw = ConfigReadWriteFactory::CreateConfigReadWrite(filePath);

	bool isSceneSaved = pConfigrw->WriteScene(filePath, this->m_pScene);
	if (!isSceneSaved)
	{
		CheckEngineError("Scene saving to file");
		return;
	}

	delete pConfigrw; // Used only to save configs

	return;
}

void Engine::SaveScene()
{
	this->SaveScene(sceneFilePath);
}

void Engine::LoadScene(std::string filePath)
{
	this->m_pScene->Clear();

	iConfigReadWrite* pConfigrw = ConfigReadWriteFactory::CreateConfigReadWrite(filePath);

	bool isSceneLoaded = pConfigrw->ReadScene(filePath, this->m_pScene);
	if (!isSceneLoaded)
	{
		CheckEngineError("Scene loading from file");
		return;
	}

	delete pConfigrw; // Used only to load configs

	this->m_pRenderer->LoadScene(baseModelPath);

	return;
}

void Engine::LoadScene()
{
	this->LoadScene(sceneFilePath);
}