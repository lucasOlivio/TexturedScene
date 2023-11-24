#include "EngineRenderer/WindowSystem.h"
#include "common/openglerrors.h"
#include "common/opengl.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

WindowSystem::WindowSystem(iShaderInfo* pShaderInfo)
{
    this->m_pWindow = nullptr;
    this->m_pShaderInfo = pShaderInfo;
}

WindowSystem::~WindowSystem()
{
}

bool WindowSystem::Initialize(uint windowWidth, uint windowHeight, const std::string& windowName,
                              GLFWkeyfun KeyCallback, GLFWmousebuttonfun MousePressCallback, GLFWcursorposfun MousePosCallback)
{
    glfwSetErrorCallback(GlfwErrorCallback);
    if (!glfwInit()) {
        printf("Error initializing GLFW!\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    this->m_width = windowWidth;
    this->m_height = windowHeight;

    this->m_pWindow = glfwCreateWindow(this->m_width, this->m_height, windowName.c_str(), NULL, NULL);
    if (!this->m_pWindow)
    {
        printf("Error creating window '%s'!\n", windowName.c_str());
        glfwTerminate();
        return false;
    }
    glfwSetKeyCallback(this->m_pWindow, KeyCallback);
    glfwSetMouseButtonCallback(this->m_pWindow, MousePressCallback);
    glfwSetCursorPosCallback(this->m_pWindow, MousePosCallback);

    glfwMakeContextCurrent(this->m_pWindow);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // Enabling error outputs
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    return true;
}

void WindowSystem::Destroy()
{
    if (this->m_pWindow) {
        glfwDestroyWindow(this->m_pWindow);
    }
    glfwTerminate();
}

glm::mat4 WindowSystem::GetProjection()
{
    return glm::perspective(0.6f,
                            this->m_ratio,
                            100.0f,
                            10000.0f);
}

void WindowSystem::NewFrame(int shaderID)
{
    // Update viewport
    glfwGetFramebufferSize(this->m_pWindow, &this->m_width, &this->m_height);

    if (this->m_height == 0)
    {
        this->m_ratio = 0.0f;
    }
    else 
    {
        this->m_ratio = this->m_width / (float)this->m_height;
    }

    glViewport(0, 0, this->m_width, this->m_height);

    // Clear frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // While drawing a pixel, see if the pixel that's already there is closer or not?
    glEnable(GL_DEPTH_TEST);
    // Activate transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // set to not draw "back facing" triangles
    glCullFace(GL_BACK);
}

void WindowSystem::UpdateUL(int shaderID)
{
    if(this->m_ratio == 0.0)
    {
        return;
    }

    // Update matrix projection in shader
    glm::mat4 matProjection = this->GetProjection();
    GLint matProjectionUL = this->m_pShaderInfo->GetUL(shaderID, "matProjection");
    glUniformMatrix4fv(matProjectionUL, 1, GL_FALSE, glm::value_ptr(matProjection));
}

void WindowSystem::EndFrame()
{
    glfwSwapBuffers(this->m_pWindow);
    glfwPollEvents();
}

bool WindowSystem::WindowShouldClose() {
    return glfwWindowShouldClose(this->m_pWindow);
}

GLFWwindow* WindowSystem::GetWindow()
{
    return this->m_pWindow;
}

void WindowSystem::GetWindowSize(int& windowHeight, int& windowWidth)
{
    windowHeight = this->m_height;
    windowWidth = this->m_width;
}

float WindowSystem::GetCurrentWindowRatio() {
    return this->m_ratio;
}

void WindowSystem::CaptureMouse()
{
    glfwSetInputMode(this->m_pWindow,
                    GLFW_CURSOR,
                    GLFW_CURSOR_DISABLED);
}

void WindowSystem::ReleaseMouse()
{
    glfwSetInputMode(this->m_pWindow,
                    GLFW_CURSOR,
                    GLFW_CURSOR_NORMAL);
}
