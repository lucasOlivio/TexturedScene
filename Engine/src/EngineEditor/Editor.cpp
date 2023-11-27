#include "EngineEditor/Editor.h"
#include "components.h" // TODO: Find a way to remove this dependency
#include "components/iComponent.h"
#include "common/opengl.h"
#include "common/utils.h"
#include "common/constants.h"
#include "common/Input.h"
#include "EngineDebug/DebugSystem.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>

float cameraSpeed      = 1000;
double changeStepFloat = 0.01;
int changeStepInt      = 1;

DebugSystem* pDebug = DebugSystem::Get();

Editor::Editor(KeyEvent* pKeyEvent, SceneView* pSceneView, iSceneDirector* pSceneDirector, WindowSystem* pWindow)
			: m_pSceneView(pSceneView), m_pSceneDirector(pSceneDirector), m_pWindow(pWindow)
{
	pKeyEvent->Attach(this);

	m_oldMouseState = GLFW_RELEASE;
	m_lastSelectedEntity = 0;
	m_selectedEntity = 0;
	m_selectedComponent = 0;
	m_selectedParameter = 0;
	m_isRunning = true;
	m_firstUpdate = true;
	m_lastX = 0;
	m_lastY = 0;
	m_yaw = 0;
	m_pitch = 0;
	m_sensitivity = 0.025f;
}

Editor::~Editor()
{
}

void Editor::RedrawEntityUI()
{
	if (!m_isRunning)
	{
		// Only allow changes on editor mode
		return;
	}

	system("cls");
	printf("Editor / Play mode: P\n");

	m_vecCompInfos = m_pSceneView->GetComponentsInfo(m_selectedEntity);
	sComponentInfo componentInfo = m_vecCompInfos[m_selectedComponent];
	TagComponent* pTag = m_pSceneView->GetComponent<TagComponent>(m_selectedEntity, "tag");

	printf("Select entity: PAGE_UP/PAGE_DOWN\n");
	printf("Select component: ARROW_RIGHT/ARROW_LEFT\n");
	printf("Select parameter: ARROW_UP/ARROW_DOWN\n");
	printf("Modify change step floats: +/-\n");
	printf("Modify change step ints: CTRL+/CTRL-\n");
	printf("Modify selected value: SHIFT + W/S/A/D/Q/E/Z/X\n");
	printf("Set value manually: ENTER\n");
	printf("Save scene: F1\n");
	printf("Load Scene: F5\n\n");

	printf("Toggle collision mode: C\n");
	printf("Debug collision mode: %d\n", (int)pDebug->IsModesOn(eDebugMode::COLLISION));
	// printf("Toggle normal mode: N\n");
	// printf("Debug normal mode: %d\n\n", (int)pDebug->IsModesOn(eDebugMode::NORMAL));

	printf("Change step floats: %.2f\n", changeStepFloat);
	printf("Change step ints: %d\n", changeStepInt);
	printf("Entity ID #%d\n\n", m_selectedEntity);
	printf("Entity TAG <%s>\n\n", pTag->name.c_str());

	printf("Component: %s\n", componentInfo.componentName.c_str());
	printf("%s\n", std::string(10, '-').c_str());

	// TODO: Better way to print the values by its type without sss
	// needing to convert every time we get from components?
	for (int i = 0; i < componentInfo.componentParameters.size(); i++)
	{
		if (i == m_selectedParameter)
		{
			printf(">> ");
		}
		else
		{
			printf("   ");
		}
		sParameterInfo parameterInfo = componentInfo.componentParameters[i];
		if (parameterInfo.parameterType == "string")
		{
			m_PrintParameter(parameterInfo.parameterName, parameterInfo.parameterStrValue);
		}
		else if (parameterInfo.parameterType == "int")
		{
			m_PrintParameter(parameterInfo.parameterName, parameterInfo.parameterIntValue);
		}
		else if (parameterInfo.parameterType == "bool")
		{
			m_PrintParameter(parameterInfo.parameterName, parameterInfo.parameterBoolValue);
		}
		else if (parameterInfo.parameterType == "float")
		{
			m_PrintParameter(parameterInfo.parameterName, parameterInfo.parameterFloatValue);
		}
		else if (parameterInfo.parameterType == "vec3")
		{
			m_PrintParameter(parameterInfo.parameterName, parameterInfo.parameterVec3Value);
		}
		else if (parameterInfo.parameterType == "vec4")
		{
			m_PrintParameter(parameterInfo.parameterName, parameterInfo.parameterVec4Value);
		}
		else if (parameterInfo.parameterType == "vecFloat")
		{
			m_PrintParameter(parameterInfo.parameterName, parameterInfo.parameterVecFloatValue);
		}
		else if (parameterInfo.parameterType == "vecStr")
		{
			m_PrintParameter(parameterInfo.parameterName, parameterInfo.parameterVecStrValue);
		}
		else
		{
			printf("warning '%s' of unkown type '%s'\n", 
					parameterInfo.parameterName.c_str(), 
					parameterInfo.parameterType.c_str());
		}
	}

	printf("\n\n");

	return;
}

void Editor::DrawSelectedEntity()
{
	using namespace glm;

	const vec4 SELECTED_COLOR = GREEN;

	int gizmoSize = 25;

	// Bounding box and gizmo to selected element
	iComponent* pModelComp = m_pSceneView->GetComponent(m_selectedEntity, "model");
	TransformComponent* pTransform = m_pSceneView->GetComponent<TransformComponent>(m_selectedEntity, "transform");

	if (pModelComp != nullptr)
	{
		mat4 worldMat = pTransform->GetTransform();
		ModelComponent* pModel = (ModelComponent*)pModelComp;
		sMesh* currMesh = pModel->GetCurrentMesh();

		vec3 minXYZ = vec3(currMesh->minX, currMesh->minY, currMesh->minZ);
		vec3 maxXYZ = vec3(currMesh->maxX, currMesh->maxY, currMesh->maxZ);

		minXYZ = worldMat * vec4(minXYZ, 1.0f);
		maxXYZ = worldMat * vec4(maxXYZ, 1.0f);
		gizmoSize = (int)distance(minXYZ, maxXYZ); // Gizmo the size of the object

		pDebug->AddRectangle(
			minXYZ,
			maxXYZ,
			SELECTED_COLOR
		);
	}
	else
	{
		pDebug->AddSphere(pTransform->GetPosition(), 10.0f, SELECTED_COLOR);
	}

	pDebug->AddGizmo(pTransform->GetPosition(), gizmoSize);
}

bool Editor::LoadScene()
{
	m_pTransformCamera = m_pSceneView->GetComponentByTag<TransformComponent>("camera", "transform");
	m_pCamera = m_pSceneView->GetComponentByTag<CameraComponent>("camera", "camera");
	m_vecCompInfos = m_pSceneView->GetComponentsInfo(m_selectedEntity);

	return true;
}

void Editor::Update(double deltaTime)
{	
	MouseActions();
	MoveCamera(deltaTime);

	DrawSelectedEntity();
}

void Editor::Notify(std::string eventName, iEvent* pEvent)
{
	if (eventName != "keyevent")
	{
		return;
	}

	KeyEvent* pKeyEvent = dynamic_cast<KeyEvent*>(pEvent);

	bool isValidKey = KeyActions(pKeyEvent->GetKeyInfo());
	if (isValidKey)
	{
		RedrawEntityUI();
	}
}

bool Editor::IsRunning()
{
	return m_isRunning;
}

void Editor::SetRunning(bool isRunning)
{
	if (isRunning)
	{
		m_pTransformCamera = m_pSceneView->GetComponentByTag<TransformComponent>("camera", "transform");
		m_pCamera = m_pSceneView->GetComponentByTag<CameraComponent>("camera", "camera");
	}

	m_isRunning = isRunning;
}

void Editor::MouseActions()
{
	if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1))
	{
		if (m_oldMouseState == GLFW_RELEASE)
		{
			m_firstUpdate = true;
			m_pWindow->CaptureMouse();
			m_oldMouseState = GLFW_PRESS;
		}

		glm::vec2 mouse = Input::MousePosition();
		m_UpdateCamera(mouse.x, mouse.y);
	}
	else
	{
		if (m_oldMouseState == GLFW_PRESS)
		{
			m_firstUpdate = false;
			m_pWindow->ReleaseMouse();
			m_oldMouseState = GLFW_RELEASE;
		}
	}
}

bool Editor::KeyActions(sKeyInfo keyInfo)
{
	// Editor mode/Game mode
	// --------------------------------------------
	if (keyInfo.pressedKey == GLFW_KEY_P && (keyInfo.action == GLFW_PRESS))
	{
		m_pSceneDirector->ChangeMode();
		return true;
	}

	if (!m_isRunning)
	{
		// Only allow changes on editor mode
		return false;
	}

	// Save/Load scene
	// --------------------------------------------
	if (keyInfo.pressedKey == GLFW_KEY_F1 && (keyInfo.action == GLFW_PRESS))
	{
		m_pSceneDirector->SaveScene();
		return true;
	}
	if (keyInfo.pressedKey == GLFW_KEY_F5 && (keyInfo.action == GLFW_PRESS))
	{
		m_pSceneDirector->LoadScene();

		m_pTransformCamera = m_pSceneView->GetComponentByTag<TransformComponent>("camera", "transform");
		m_pCamera = m_pSceneView->GetComponentByTag<CameraComponent>("camera", "camera");
		return true;
	}

	// Entity selection
	// --------------------------------------------
	if (keyInfo.pressedKey == GLFW_KEY_PAGE_UP && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
	{
		ChangeSelectedEntity(1);
		return true;
	}
	if (keyInfo.pressedKey == GLFW_KEY_PAGE_DOWN && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
	{
		ChangeSelectedEntity(-1);
		return true;
	}

	// Set value directly
	// --------------------------------------------
	if (keyInfo.pressedKey == GLFW_KEY_0 && (keyInfo.action == GLFW_PRESS))
	{
		SetParameterManually(0);
		return true;
	}
	if (keyInfo.pressedKey == GLFW_KEY_1 && (keyInfo.action == GLFW_PRESS))
	{
		SetParameterManually(1);
		return true;
	}
	if (keyInfo.pressedKey == GLFW_KEY_2 && (keyInfo.action == GLFW_PRESS))
	{
		SetParameterManually(2);
		return true;
	}
	if (keyInfo.pressedKey == GLFW_KEY_3 && (keyInfo.action == GLFW_PRESS))
	{
		SetParameterManually(3);
		return true;
	}

	// Component selection
	// --------------------------------------------
	if (keyInfo.pressedKey == GLFW_KEY_RIGHT && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
	{
		ChangeSelectedComponent(-1);
		return true;
	}
	if (keyInfo.pressedKey == GLFW_KEY_LEFT && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
	{
		ChangeSelectedComponent(1);
		return true;
	}

	// Parameter selection
	// --------------------------------------------
	if (keyInfo.pressedKey == GLFW_KEY_UP && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
	{
		ChangeSelectedParameter(-1);
		return true;
	}
	if (keyInfo.pressedKey == GLFW_KEY_DOWN && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
	{
		ChangeSelectedParameter(1);
		return true;
	}

	// Change step
	// --------------------------------------------
	if (keyInfo.mods == 0)
	{
		if (keyInfo.pressedKey == GLFW_KEY_EQUAL && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
		{
			changeStepFloat += 0.01f;
			return true;
		}
		if (keyInfo.pressedKey == GLFW_KEY_MINUS && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
		{
			changeStepFloat -= 0.01f;
			return true;
		}
		if (keyInfo.pressedKey == GLFW_KEY_RIGHT_BRACKET && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
		{
			changeStepFloat += 0.1f;
			return true;
		}
		if (keyInfo.pressedKey == GLFW_KEY_LEFT_BRACKET && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
		{
			changeStepFloat -= 0.1f;
			return true;
		}
	}

	if ((keyInfo.mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
	{
		// Duplicate entity
		if (keyInfo.pressedKey == GLFW_KEY_D && (keyInfo.action == GLFW_PRESS))
		{
			m_pSceneView->CreateEntity(m_selectedEntity);
			return true;
		}
	}

	// Debug modes
	// --------------------------------------------
	if (keyInfo.pressedKey == GLFW_KEY_C && keyInfo.action == GLFW_PRESS)
	{
		pDebug->ToggleMode(eDebugMode::COLLISION);
		return true;
	}
	//if (keyInfo.pressedKey == GLFW_KEY_N && keyInfo.action == GLFW_PRESS)
	//{
	//	pDebug->ToggleMode(eDebugMode::NORMAL);
	//	return true;
	//}

	// Parameter edition
	// --------------------------------------------
	if (keyInfo.mods == GLFW_MOD_SHIFT) // Shift down
	{
		if (keyInfo.pressedKey == GLFW_KEY_D && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
		{
			ModifySelectedParameter(0, 1);
			return true;
		}
		if (keyInfo.pressedKey == GLFW_KEY_A && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
		{
			ModifySelectedParameter(0, -1);
			return true;
		}

		if (keyInfo.pressedKey == GLFW_KEY_E && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
		{
			ModifySelectedParameter(1, 1);
			return true;
		}
		if (keyInfo.pressedKey == GLFW_KEY_Q && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
		{
			ModifySelectedParameter(1, -1);
			return true;
		}

		if (keyInfo.pressedKey == GLFW_KEY_W && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
		{
			ModifySelectedParameter(2, 1);
			return true;
		}
		if (keyInfo.pressedKey == GLFW_KEY_S && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
		{
			ModifySelectedParameter(2, -1);
			return true;
		}

		if (keyInfo.pressedKey == GLFW_KEY_X && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
		{
			ModifySelectedParameter(3, 1);
			return true;
		}
		if (keyInfo.pressedKey == GLFW_KEY_Z && (keyInfo.action == GLFW_PRESS || keyInfo.action == GLFW_REPEAT))
		{
			ModifySelectedParameter(3, -1);
			return true;
		}
	}

	return false;
}

void Editor::ChangeSelected(int& selected, int orientation, int count)
{
	using namespace myutils;

	// Make sure we don't overstep
	orientation = Sign(orientation);

	selected += orientation;
	WrapMinMax(0, count - 1, selected);
}

void Editor::ChangeSelectedEntity(int orientation)
{
	int maxEntities = (int)m_pSceneView->GetNumEntities();
	ChangeSelected(m_selectedEntity, orientation, maxEntities);
	m_selectedComponent = 0;
	m_selectedParameter = 0;
}

void Editor::ChangeSelectedComponent(int orientation)
{
	int numComponents = (int)m_vecCompInfos.size();
	ChangeSelected(m_selectedComponent, orientation, numComponents);
	m_selectedParameter = 0;
}

void Editor::ChangeSelectedParameter(int orientation)
{
	if (m_selectedComponent >= m_vecCompInfos.size())
	{
		return;
	}
	int numParameters = (int)m_vecCompInfos[m_selectedComponent].componentParameters.size();
	ChangeSelected(m_selectedParameter, orientation, numParameters);
}

void Editor::m_ModifySelected(int& value, int orientation)
{
	value += (int)(orientation * changeStepInt);
}

void Editor::m_ModifySelected(bool& value, int orientation)
{
	value += (orientation * changeStepInt);
}

void Editor::m_ModifySelected(float& value, int orientation)
{
	value += (float)(orientation * changeStepFloat);
}

void Editor::m_ModifySelected(glm::vec3& value, int orientation, int axis)
{
	if (axis > 2)
	{
		return;
	}

	value[axis] += (float)(orientation * changeStepFloat);
}

void Editor::m_ModifySelected(glm::vec4& value, int orientation, int axis)
{
	if (axis > 3)
	{
		return;
	}

	value[axis] += (float)(orientation * changeStepFloat);
}

void Editor::m_ModifySelected(std::vector<float>& value, int orientation, int axis)
{
	if (axis > value.size())
	{
		return;
	}

	value[axis] += (orientation * (float)changeStepFloat);
}

void Editor::m_UpdateCamera(float xpos, float ypos)
{
	using namespace glm;

	if (m_firstUpdate)
	{
		m_lastX = xpos;
		m_lastY = ypos;
		m_firstUpdate = false;
	}

	float xoffset = xpos - m_lastX;
	float yoffset = ypos - m_lastY;
	m_lastX = xpos;
	m_lastY = ypos;
	xoffset *= m_sensitivity;
	yoffset *= m_sensitivity;

	vec3 rotation = m_pTransformCamera->GetOrientation();

	rotation.x += xoffset;
	rotation.y -= yoffset;

	m_pTransformCamera->SetOrientation(rotation);
}

void Editor::ModifySelectedParameter(int axis, int orientation)
{
	using namespace myutils;

	if (m_selectedComponent >= m_vecCompInfos.size())
	{
		return;
	}

	sComponentInfo compInfo = m_vecCompInfos[m_selectedComponent];
	sParameterInfo paramInfo = compInfo.componentParameters[m_selectedParameter];
	
	if (paramInfo.parameterType == "string")
	{
		// TODO: Been able to change models at name changes etc.
		return;
	}

	// Make sure we don't overstep
	orientation = Sign(orientation);

	if (paramInfo.parameterType == "int" || 
		paramInfo.parameterType == "bool" ||
		paramInfo.parameterType == "float")
	{
		if (axis > 0)
		{
			return;
		}

		if (paramInfo.parameterType == "int")
		{
			m_ModifySelected(paramInfo.parameterIntValue, orientation);
		}
		else if (paramInfo.parameterType == "float")
		{
			m_ModifySelected(paramInfo.parameterFloatValue, orientation);
		}
		else
		{
			m_ModifySelected(paramInfo.parameterBoolValue, orientation);
		}
	}
	else if (paramInfo.parameterType == "vec3")
	{
		m_ModifySelected(paramInfo.parameterVec3Value, orientation, axis);
	}
	else if (paramInfo.parameterType == "vec4")
	{
		m_ModifySelected(paramInfo.parameterVec4Value, orientation, axis);
	}
	else if (paramInfo.parameterType == "vecFloat")
	{
		m_ModifySelected(paramInfo.parameterVecFloatValue, orientation, axis);
	}

	iComponent* pComponent = m_pSceneView->GetComponent<iComponent>(m_selectedEntity, 
																		  compInfo.componentName);
	pComponent->SetParameter(paramInfo);

	return;
}

void Editor::MoveCamera(double deltaTime)
{
	using namespace glm;

	vec3 cameraPosition = m_pTransformCamera->GetPosition();
	vec3 cameraRotation = m_pTransformCamera->GetOrientation();
	vec3 cameraUpVector = m_pCamera->upVector;
	
	vec3 cameraFront = normalize(m_pCamera->GetCameraFront(cameraPosition, cameraRotation));
	vec3 cameraSides = normalize(cross(cameraFront, m_pCamera->upVector));
	vec3 moveOffset = vec3(0.0f);

	// No ctrl or shift been pressed
	if (Input::GetKeyMods() != 0)
	{
		return;
	}

	// Handle key presses for movement
	if (Input::IsKeyPressed(GLFW_KEY_W)) {
		moveOffset = cameraSpeed * cameraFront * (float)deltaTime;
	}
	if (Input::IsKeyPressed(GLFW_KEY_S)) {
		moveOffset = -(cameraSpeed * cameraFront * (float)deltaTime);
	}
	if (Input::IsKeyPressed(GLFW_KEY_A)) {
		moveOffset = -(cameraSpeed * cameraSides * (float)deltaTime);
	}
	if (Input::IsKeyPressed(GLFW_KEY_D)) {
		moveOffset = cameraSpeed * cameraSides * (float)deltaTime;
	}

	cameraPosition += moveOffset;

	m_pTransformCamera->SetPosition(cameraPosition);
}

void Editor::SetParameterManually(int axis)
{
	using namespace myutils;

	// Get the value to set from the user
	printf("Value to set: ");
	std::string value;
	std::cin >> value;

	sComponentInfo compInfo = m_vecCompInfos[m_selectedComponent];
	sParameterInfo paramInfo = compInfo.componentParameters[m_selectedParameter];

	if (paramInfo.parameterType == "string")
	{
		// TODO: Been able to change models at name changes etc.
		return;
	}

	// TODO: Remove this code repetition validations
	if (paramInfo.parameterType == "int" ||
		paramInfo.parameterType == "bool" ||
		paramInfo.parameterType == "float")
	{
		if (axis > 0)
		{
			return;
		}

		if (paramInfo.parameterType == "float")
		{
			paramInfo.parameterFloatValue = std::stof(value);
		}
		else if (paramInfo.parameterType == "int")
		{
			paramInfo.parameterIntValue = std::stoi(value);
		}
		else
		{
			paramInfo.parameterBoolValue = std::stoi(value);
		}
	}
	else if (paramInfo.parameterType == "vec3")
	{
		if (axis > 3)
		{
			return;
		}
		paramInfo.parameterVec3Value[axis] = std::stof(value);
	}
	else if (paramInfo.parameterType == "vec4")
	{
		paramInfo.parameterVec4Value[axis] = std::stof(value);
	}

	iComponent* pComponent = m_pSceneView->GetComponent<iComponent>(m_selectedEntity,
		compInfo.componentName);
	pComponent->SetParameter(paramInfo);
}


void Editor::m_PrintParameter(std::string parName, std::string parValue)
{
	printf("%s: %s\n", parName.c_str(), parValue.c_str());
}

void Editor::m_PrintParameter(std::string parName, float parValue)
{
	printf("%s: %.2f\n", parName.c_str(), parValue);
}

void Editor::m_PrintParameter(std::string parName, int parValue)
{
	printf("%s: %d\n", parName.c_str(), parValue);
}

void Editor::m_PrintParameter(std::string parName, bool parValue)
{
	printf("%s: %d\n", parName.c_str(), parValue);
}

void Editor::m_PrintParameter(std::string parName, glm::vec3 parValue)
{
	printf("%s: %s\n", parName.c_str(), glm::to_string(parValue).c_str());
}

void Editor::m_PrintParameter(std::string parName, glm::vec4 parValue)
{
	printf("%s: %s\n", parName.c_str(), glm::to_string(parValue).c_str());
}

void Editor::m_PrintParameter(std::string parName, std::vector<std::string> parValue)
{
	printf("%s: [ ", parName.c_str());

	for (std::string str : parValue)
	{
		printf("'%s' ", str.c_str());
	}

	printf("]\n");
}

void Editor::m_PrintParameter(std::string parName, std::vector<float>& parValue)
{
	printf("%s: [ ", parName.c_str());

	for (float val : parValue)
	{
		printf("%.3f ", val);
	}

	printf("]\n");
}