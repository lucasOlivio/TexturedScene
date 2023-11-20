#pragma once

#include <string>

class iEvent;

class iListener
{
public:
	virtual ~iListener() {};

	virtual void Notify(std::string eventName, iEvent* pEvent) = 0;
};