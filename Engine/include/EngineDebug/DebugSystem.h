#pragma once

class DebugSystem
{
public:
	~DebugSystem() {};

	static DebugSystem* Get();
private:
	// Singleton
	DebugSystem();
	static DebugSystem* m_pInstance;
};
