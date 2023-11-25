#pragma once

#include "common/types.h"
#include "scene/SceneView.h"
#include "scene/iSceneDirector.h"
#include "events/KeyEvent.h"
#include "events/MouseEvent.h"
#include "EngineRenderer/WindowSystem.h"
#include "components/Transform.h"
#include "components/Camera.h"

// For now only some info been printed on console
// and handling keycallbacks from glfw
class Editor : public iListener
{
public:
	// ctors & dtors
	Editor(KeyEvent* pKeyEvent, SceneView* pSceneView, iSceneDirector* pSceneDirector, WindowSystem* pWindow);
	~Editor();

	bool LoadScene();
	void Update(double deltaTime);

	// Draw selected entity UI info
	void RedrawEntityUI();
	void DrawSelectedEntity();

	// On key callback we manage entities on the scene accordingly
	virtual void Notify(std::string eventName, iEvent* pEvent);

	bool IsRunning();
	void SetRunning(bool isRunning);

	void ChangeSelected(int& selected, int orientation, int count);
	void ChangeSelectedEntity(int orientation);
	void ChangeSelectedComponent(int orientation);
	void ChangeSelectedParameter(int orientation);

	void MouseActions();
	// Actions to take based on key pressed
	bool KeyActions(sKeyInfo keyInfo);
	void MoveCamera(double deltaTime);

	void ModifySelectedParameter(int axis, int orientation);
	void SetParameterManually(int axis);

private:
	int m_selectedEntity;
	int m_selectedComponent;
	int m_lastSelectedEntity;
	int m_selectedParameter;
	bool m_isRunning;
	std::vector<sComponentInfo> m_vecCompInfos;

	SceneView* m_pSceneView;
	iSceneDirector* m_pSceneDirector;

	// Flying camera
	bool m_oldMouseState;
	bool m_firstUpdate;
	float m_lastX;
	float m_lastY;
	float m_yaw;
	float m_pitch;
	float m_sensitivity;
	TransformComponent* m_pTransformCamera;
	CameraComponent* m_pCamera;

	WindowSystem* m_pWindow;

	// TODO: Better way to handle multiple value types from parameters
	void m_PrintParameter(std::string parName, std::string parValue);
	void m_PrintParameter(std::string parName, float parValue);
	void m_PrintParameter(std::string parName, int parValue);
	void m_PrintParameter(std::string parName, glm::vec3 parValue);
	void m_PrintParameter(std::string parName, glm::vec4 parValue);
	void m_PrintParameter(std::string parName, bool parValue);
	void m_PrintParameter(std::string parName, std::vector<std::string> parValue);
	void m_PrintParameter(std::string parName, std::vector<float>& parValue);

	void m_ModifySelected(int& value, int orientation);
	void m_ModifySelected(bool& value, int orientation);
	void m_ModifySelected(float& value, int orientation);
	void m_ModifySelected(glm::vec3& value, int orientation, int axis);
	void m_ModifySelected(glm::vec4& value, int orientation, int axis);
	void m_ModifySelected(std::vector<float>& value, int orientation, int axis);

	void m_UpdateCamera(float xpos, float ypos);
};