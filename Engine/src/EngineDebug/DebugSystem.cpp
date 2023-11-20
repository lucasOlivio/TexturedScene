#include "EngineDebug/DebugSystem.h"

DebugSystem* DebugSystem::Get()
{
	if (DebugSystem::m_pInstance == nullptr)
	{
		DebugSystem::m_pInstance = new DebugSystem();
	}
	return DebugSystem::m_pInstance;
}
