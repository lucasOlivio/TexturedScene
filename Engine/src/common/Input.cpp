#include "common/Input.h"
#include "common/opengl.h"
#include "Engine.h"
#include <glm/gtc/matrix_transform.hpp>

sKeyInfo Input::m_keyInfo = sKeyInfo();
sMouseInfo Input::m_mouseInfo = sMouseInfo();

Input::Input(KeyEvent* pKeyevent, MouseEvent* pMouseEvent)
{
    pKeyevent->Attach(this);
    pMouseEvent->Attach(this);
}

void Input::Notify(std::string eventName, iEvent* pEvent)
{
    if (eventName == "keyevent")
    {
        KeyEvent* pKeyEvent = (KeyEvent*)pEvent;
        m_keyInfo = pKeyEvent->GetKeyInfo();
    }
    else if (eventName == "mouseevent")
    {
        MouseEvent* pMouseEvent = (MouseEvent*)pEvent;
        m_mouseInfo = pMouseEvent->GetInfo();
    }
}

bool Input::IsKeyPressed(const int key)
{
    GLFWwindow* pWindow = Engine::Get()->GetWindow();

    int state = glfwGetKey(pWindow, static_cast<int32_t>(key));

    return state == GLFW_PRESS;
}

glm::vec2 Input::MousePosition()
{
    using namespace glm;

    double xPos, yPos;
    Engine* pEng = Engine::Get();

    // Relative window mouse position
    GLFWwindow* pWindow = pEng->GetWindow();
    glfwGetCursorPos(pWindow, &xPos, &yPos);

    return { (float)xPos, (float)yPos };
}

glm::vec2 Input::MousePosRelative()
{
    using namespace glm;

    Engine* pEng = Engine::Get();
    vec2 mousePosition = Input::MousePosition();

    mat4 viewMatrix = pEng->GetViewMat();
    mat4 projectionMatrix = pEng->GetWindowProjectionMat();
    mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
    mat4 inverseViewProjectionMatrix = inverse(viewProjectionMatrix);

    // Convert mouse coordinates to NDC (Normalized Device Coordinates)
    int windowHeight, windowWidth;
    Engine::Get()->GetWindowSize(windowHeight, windowWidth);

    float ndcX = (2.0f * mousePosition.x) / windowWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * mousePosition.y) / windowHeight;

    // Create a vector in homogeneous coordinates
    vec4 clipCoords(ndcX, ndcY, 1.0f, 1.0f); // z=1.0f to get the far plane

    // Transform the clip coordinates to view coordinates
    vec4 eyeCoords = inverseViewProjectionMatrix * clipCoords;

    // Convert the result to a world space position
    vec3 worldSpaceMousePosition = vec3(eyeCoords) / eyeCoords.w;

    return { worldSpaceMousePosition.x, worldSpaceMousePosition.y };
}

bool Input::IsMouseButtonPressed(int button)
{
    GLFWwindow* pWindow = Engine::Get()->GetWindow();

    int state = glfwGetMouseButton(pWindow, GLFW_MOUSE_BUTTON_LEFT);

    return state == GLFW_PRESS;
}

int Input::GetKeyMods()
{
    return m_keyInfo.mods;
}

int Input::GetMouseMods()
{
    return m_mouseInfo.mods;
}

int Input::GetKeyAction()
{
    return m_keyInfo.action;
}

int Input::GetMouseAction()
{
    return m_mouseInfo.action;
}

sKeyInfo Input::GetKeyInfo()
{
    return m_keyInfo;
}

sMouseInfo Input::GetMouseInfo()
{
    return m_mouseInfo;
}
