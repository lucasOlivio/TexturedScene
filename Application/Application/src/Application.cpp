#include "Application.h"
#include "GameObjects/GameObjectFactory.h"

Application::Application()
	: Engine::Engine()
{
	this->m_pGM = nullptr;
}

Application::~Application()
{
	delete this->m_pGM;
}

void Application::LoadGM()
{
	GameObjectFactory gameObjFactory(this->m_pScene);
	this->m_pGM = gameObjFactory.CreateGameObject("GM", false);

	if (this->m_pGM)
	{
		return;
	}
	else
	{
		CheckEngineError("Error loading Game manager object");
		return;
	}
}

void Application::Update(double fixedDeltaTime)
{
	Engine::Update(fixedDeltaTime);

	if (this->m_pScene->IsPlaying())
	{
		this->m_pGM->Update(fixedDeltaTime);
	}
}

void Application::ChangeMode()
{
	if (this->m_pScene->IsPlaying())
	{
		this->m_pWindowSystem->ReleaseMouse();
		this->m_pGM->OnExit(this->m_pCollisionEvent);
		delete this->m_pGM;
		this->m_pGM = NULL;

		this->Engine::ChangeMode();
	}
	else
	{
		this->Engine::ChangeMode();

		this->m_pWindowSystem->CaptureMouse();
		this->LoadGM();
		this->m_pGM->OnStart(this->m_pCollisionEvent);
	}
}
