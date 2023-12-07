#include "Application.h"

Application::Application()
	: Engine::Engine()
{
}

Application::~Application()
{
}

void Application::LoadGM()
{
}

void Application::Update(double fixedDeltaTime)
{
	if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
	{
		SetRunning(false);
		return;
	}

	Engine::Update(fixedDeltaTime);
}

void Application::ChangeMode()
{
	if (this->m_pScene->IsPlaying())
	{
		this->m_pWindowSystem->ReleaseMouse();

		this->Engine::ChangeMode();
	}
	else
	{
		this->Engine::ChangeMode();

		this->m_pWindowSystem->CaptureMouse();
		this->LoadGM();
	}
}
