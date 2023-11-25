#include "Engine.h"
#include "scene/Scene.h"
#include "common/ConfigReadWriteFactory.h"
#include "events/GLFWWrapper.h"
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
	m_isInitialized = false;
	m_isRunning = false;

	m_lastTime = 0.0;

	// Singleton
	assert(!m_pInstance);
	m_pInstance = this;
}

Engine::~Engine()
{
	m_pInstance = nullptr;
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
	if (m_isInitialized)
	{
		// Already initialized
		return true;
	}

	sceneFilePath = baseConfigsPath + sceneName + ".json";
	windowName = sceneName;

	// Events
	m_pCollisionEvent = new CollisionEvent();
	m_pKeyEvent = new KeyEvent();
	m_pMouseEvent = new MouseEvent();
	GLFWWrapper::SetKeyEvent(m_pKeyEvent);
	GLFWWrapper::SetMouseEvent(m_pMouseEvent);

	printf("Creating systems...\n");
	m_pScene = new Scene(m_pKeyEvent, m_pCollisionEvent);
	m_pShaderManager = new ShaderManager(baseShadersPath);
	m_pRenderer = new Renderer();
	m_pWindowSystem = new WindowSystem(m_pShaderManager);
	m_pEditor = new Editor(m_pKeyEvent, m_pScene, this, m_pWindowSystem);
	m_pPhysics = new Physics(m_pScene, m_pCollisionEvent);
	m_pInput = new Input(m_pKeyEvent, m_pMouseEvent);
	m_pMediaPlayer = MediaPlayer::Get();
	m_pDebugSystem = DebugSystem::Get();

	printf("Initializing systems...\n");
	// Initializes all systems

	bool isWSInitialized = m_pWindowSystem->Initialize(windowWidth,
													   windowHeight,
													   windowName,
													   GLFWWrapper::KeyCallback,
													   GLFWWrapper::MousePressCallback,
													   GLFWWrapper::MousePosCallback);
	if (!isWSInitialized)
	{
		CheckEngineError("initializing Window system");
		return false;
	}

	printf("Creating shaders...\n");
	// Creates main shader program
	bool isShaderCreated = m_pShaderManager->AddShaderProgram(shaderProgramName);
	if (!isShaderCreated)
	{
		CheckEngineError("creating shader program");
		return false;
	}
	m_currShaderID = m_pShaderManager->GetIDFromShaderProgramName(shaderProgramName);

	bool isERInitialized = m_pRenderer->Initialize(baseModelPath,
												   baseTexturesPath,
												   m_pShaderManager,
												   m_currShaderID,
												   m_pScene);
	if (!isERInitialized)
	{
		CheckEngineError("Engine renderer initialization");
		return false;
	}

	bool isMediaInit = m_pMediaPlayer->Initialize(baseAudioPath, m_pScene);
	if (!isMediaInit)
	{
		CheckEngineError("Engine media player initialization");
		return false;
	}

	bool sceneLoaded = LoadScene();
	if (!sceneLoaded)
	{
		return false;
	}

	bool isDebugInit = m_pDebugSystem->Initialize(m_pScene, m_pShaderManager, baseModelPath);
	if (!isDebugInit)
	{
		CheckEngineError("Engine debug initialization");
		return false;
	}

	m_lastTime = glfwGetTime();

	m_isInitialized = true;
	printf("Scene '%s' created scussesfully!\n", sceneName.c_str());

    return true;
}

void Engine::Run()
{
	if (!m_isInitialized)
	{
		return;
	}

	m_isRunning = true;

	while (IsRunning())
	{
		double fixedDeltaTime = GetFixedDeltaTime();
		
		m_pShaderManager->UseShaderProgram(shaderProgramName);

		m_pWindowSystem->NewFrame(m_currShaderID);
		m_pPhysics->NewFrame();

		Update(fixedDeltaTime);

		m_pWindowSystem->EndFrame();

		m_pScene->ClearListenerDeleted();
		m_pScene->ClearDeleted();
	}
}

void Engine::Update(double fixedDeltaTime)
{
	if (m_pEditor->IsRunning())
	{
		m_pEditor->Update(fixedDeltaTime);
	}
	else
	{
		m_pMediaPlayer->Update(fixedDeltaTime);
		m_pPhysics->Update(fixedDeltaTime);
	}
	m_pWindowSystem->UpdateUL(m_currShaderID);
	m_pRenderer->RenderScene(fixedDeltaTime);
	m_pDebugSystem->Update(fixedDeltaTime, m_pRenderer->GetCamera()->GetViewMat(), m_pWindowSystem->GetProjection());
}

bool Engine::IsRunning()
{
	if (m_isRunning
		&& m_pRenderer->IsRunning()
		&& !m_pWindowSystem->WindowShouldClose())
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
	double deltaTime = currentTime - m_lastTime;
	m_lastTime = currentTime;

	// Debugging performance:
	// printf("delta real: %.2f\n", deltaTime);

	// Clamp delta time to the maximum frame time
	if (deltaTime > FRAME_DURATION) {
		deltaTime = FRAME_DURATION;
	}

	// Add the frame time to the list
	m_frameTimes.push_back(deltaTime);

	// Limit the number of frames
	const size_t maxFrameCount = 60; // Store frame times for a second
	if (m_frameTimes.size() > maxFrameCount) {
		m_frameTimes.erase(m_frameTimes.begin());
	}

	// Calculate the average frame time
	double averageFrameTime = 0;
	for (double time : m_frameTimes) {
		averageFrameTime += time;
	}
	averageFrameTime /= m_frameTimes.size();

	return averageFrameTime;
}

void Engine::Exit()
{
	if (!m_isInitialized)
	{
		return;
	}

	m_pRenderer->Destroy();
	m_pWindowSystem->Destroy();

	m_isInitialized = false;
	m_isRunning = false;

	delete m_pRenderer;
	delete m_pEditor;
	delete m_pScene;

	delete m_pKeyEvent;
	delete m_pDebugSystem;
	delete m_pMediaPlayer;

	return;
}

void Engine::Exit(std::string errorMsg)
{
	Exit();

	CheckEngineError(errorMsg.c_str());
	exit(EXIT_FAILURE);

	return;
}

GLFWwindow* Engine::GetWindow()
{
	return m_pWindowSystem->GetWindow();
}

glm::mat4 Engine::GetWindowProjectionMat()
{
	return m_pWindowSystem->GetProjection();
}

void Engine::GetWindowSize(int& windowHeight, int& windowWidth)
{
	m_pWindowSystem->GetWindowSize(windowHeight, windowWidth);
}

glm::mat4 Engine::GetViewMat()
{
	return m_pRenderer->GetCamera()->GetViewMat();
}

void Engine::SetRunning(bool isRunning)
{
	m_isRunning = isRunning;
}

void Engine::ChangeMode()
{
	bool editorRunning = m_pEditor->IsRunning();

	bool isLoaded = true;
	if (editorRunning)
	{
		// Entering play mode
		SaveScene();

		m_pEditor->SetRunning(false);
		m_pPhysics->SetRunning(true);
		m_pScene->SetPlaying(true);

		isLoaded = LoadScene();
	}
	else
	{
		m_pPhysics->SetRunning(false);
		m_pScene->SetPlaying(false);

		isLoaded = LoadScene();
		m_pEditor->SetRunning(true);
	}

	if (!isLoaded)
	{
		CheckEngineError("Scene loading error\n\n");
		Exit();
	}

	return;
}

void Engine::SaveScene(std::string filePath)
{
	iConfigReadWrite* pConfigrw = ConfigReadWriteFactory::CreateConfigReadWrite(filePath);

	bool isSceneSaved = pConfigrw->WriteScene(filePath, m_pScene);
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
	SaveScene(sceneFilePath);
}

bool Engine::LoadScene(std::string filePath)
{
	m_pShaderManager->UseShaderProgram(m_currShaderID);

	m_pScene->Clear();

	iConfigReadWrite* pConfigrw = ConfigReadWriteFactory::CreateConfigReadWrite("json");

	bool isLoaded = pConfigrw->ReadScene(filePath, m_pScene);
	if (!isLoaded)
	{
		Exit("Scene loading error\n\n");
		return false;
	}

	delete pConfigrw; // Used only to load configs

	isLoaded = m_pRenderer->LoadScene(baseModelPath);
	if (!isLoaded)
	{
		Exit("Renderer loading error\n\n");
		return false;
	}

	isLoaded = m_pEditor->LoadScene();
	if (!isLoaded)
	{
		Exit("Editor loading error\n\n");
		return false;
	}

	isLoaded = m_pMediaPlayer->LoadScene();
	if (!isLoaded)
	{
		Exit("Media loading error\n\n");
		return false;
	}

	return true;
}

bool Engine::LoadScene()
{
	bool isLoaded = LoadScene(sceneFilePath);
	if (isLoaded)
	{
		return true;
	}

	return false;
}