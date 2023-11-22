#include "EngineDebug/DebugSystem.h"
#include "components/Model.h"
#include "common/utils.h"
#include "common/utilsMat.h"
#include "common/opengl.h"

DebugSystem* DebugSystem::m_pInstance = nullptr;
const unsigned int LINE_VERTEX_BUFFER_SIZE = 5'000;

DebugSystem* DebugSystem::Get()
{
	if (DebugSystem::m_pInstance == nullptr)
	{
		DebugSystem::m_pInstance = new DebugSystem();
	}
	return DebugSystem::m_pInstance;
}

bool DebugSystem::Initialize(ShaderManager* pShaderManager, std::string baseModelsPath)
{
    using namespace myutils;

    m_pShaderManager = pShaderManager;
    m_pVAOManager = new VAOManager(m_pShaderManager);

    m_pVAOManager->SetBasePath(baseModelsPath);

    // Creates debug shader program
    m_debugShaderName = "debugshader";
    bool isShaderCreated = m_pShaderManager->AddShaderProgram(m_debugShaderName);
    if (!isShaderCreated)
    {
        CheckEngineError("creating shader program");
        return false;
    }
    m_debugShaderID = m_pShaderManager->GetIDFromShaderProgramName(m_debugShaderName);
    m_pShaderProgram = m_pShaderManager->GetShaderProgramFromID(m_debugShaderID);
    m_pShaderManager->UseShaderProgram(m_debugShaderName);

    m_vecSpheresToDraw = {};

    m_InitializeLineVertex();

    // Generate and bind VAO
    glGenVertexArrays(1, &m_debugVAO);
    glBindVertexArray(m_debugVAO);

    // Generate and bind VBO
    glGenBuffers(1, &m_debugVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_debugVBO);

    glBufferData(GL_ARRAY_BUFFER,
        LINE_VERTEX_BUFFER_SIZE * sizeof(sLineVertex),
        (GLvoid*)m_pLinesVerticesToDraw,
        GL_DYNAMIC_DRAW);

    GLint vPosition_location = glGetAttribLocation(m_debugShaderID, "vPos");
    GLint vColour_location = glGetAttribLocation(m_debugShaderID, "vCol");

    // Specify the vertex data layout
    glEnableVertexAttribArray(vPosition_location);
    glVertexAttribPointer(vPosition_location, 4,
        GL_FLOAT,
        GL_FALSE,
        sizeof(sLineVertex),                   // Stride
        (void*)offsetof(sLineVertex, x));     // Offset in bytes to "x"

    glEnableVertexAttribArray(vColour_location);
    glVertexAttribPointer(vColour_location, 4,
        GL_FLOAT,
        GL_FALSE,
        sizeof(sLineVertex),
        (void*)offsetof(sLineVertex, r));     // Offset in bytes to "r"

    // Unbind VAO and VBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Load the other debug objects
    std::string sphereFile = "debug/sphere.ply";
    m_pSphereMesh = m_pVAOManager->LoadModelIntoVAO(sphereFile, m_debugShaderID, true, false, false);

    m_sizeOfLineVBO = 0;

    return true;
}

void DebugSystem::ResetDebugObjects()
{

    m_sizeOfLineVBO = 0;
    m_vecSpheresToDraw.clear();
}

void DebugSystem::AddLine(glm::vec3 startXYZ, glm::vec3 endXYZ, glm::vec4 RGBA)
{
    // Next line to add 

    m_pLinesVerticesToDraw[m_sizeOfLineVBO + 0].x = startXYZ.x;
    m_pLinesVerticesToDraw[m_sizeOfLineVBO + 0].y = startXYZ.y;
    m_pLinesVerticesToDraw[m_sizeOfLineVBO + 0].z = startXYZ.z;

    m_pLinesVerticesToDraw[m_sizeOfLineVBO + 0].r = RGBA.r;
    m_pLinesVerticesToDraw[m_sizeOfLineVBO + 0].g = RGBA.g;
    m_pLinesVerticesToDraw[m_sizeOfLineVBO + 0].b = RGBA.b;
    m_pLinesVerticesToDraw[m_sizeOfLineVBO + 0].a = RGBA.a;

    m_pLinesVerticesToDraw[m_sizeOfLineVBO + 1].x = endXYZ.x;
    m_pLinesVerticesToDraw[m_sizeOfLineVBO + 1].y = endXYZ.y;
    m_pLinesVerticesToDraw[m_sizeOfLineVBO + 1].z = endXYZ.z;

    m_pLinesVerticesToDraw[m_sizeOfLineVBO + 1].r = RGBA.r;
    m_pLinesVerticesToDraw[m_sizeOfLineVBO + 1].g = RGBA.g;
    m_pLinesVerticesToDraw[m_sizeOfLineVBO + 1].b = RGBA.b;
    m_pLinesVerticesToDraw[m_sizeOfLineVBO + 1].a = RGBA.a;

    // Increment by two
    m_sizeOfLineVBO += 2;

    return;
}

void DebugSystem::AddGizmo(glm::vec3 position, int size)
{
    // X-axis (red)
    AddLine(position, glm::vec3(position.x + size, position.y, position.z), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    // Y-axis (green)
    AddLine(position, glm::vec3(position.x, position.y + size, position.z), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    // Z-axis (blue)
    AddLine(position, glm::vec3(position.x, position.y, position.z + size), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
}

void DebugSystem::AddRectangle(glm::vec3 minXYZ, glm::vec3 maxXYZ, glm::vec4 RGBA)
{
    // Bottom face
    AddLine(glm::vec3(minXYZ.x, minXYZ.y, minXYZ.z), glm::vec3(maxXYZ.x, minXYZ.y, minXYZ.z), RGBA);
    AddLine(glm::vec3(maxXYZ.x, minXYZ.y, minXYZ.z), glm::vec3(maxXYZ.x, minXYZ.y, maxXYZ.z), RGBA);
    AddLine(glm::vec3(maxXYZ.x, minXYZ.y, maxXYZ.z), glm::vec3(minXYZ.x, minXYZ.y, maxXYZ.z), RGBA);
    AddLine(glm::vec3(minXYZ.x, minXYZ.y, maxXYZ.z), glm::vec3(minXYZ.x, minXYZ.y, minXYZ.z), RGBA);

    // Top face
    AddLine(glm::vec3(minXYZ.x, maxXYZ.y, minXYZ.z), glm::vec3(maxXYZ.x, maxXYZ.y, minXYZ.z), RGBA);
    AddLine(glm::vec3(maxXYZ.x, maxXYZ.y, minXYZ.z), glm::vec3(maxXYZ.x, maxXYZ.y, maxXYZ.z), RGBA);
    AddLine(glm::vec3(maxXYZ.x, maxXYZ.y, maxXYZ.z), glm::vec3(minXYZ.x, maxXYZ.y, maxXYZ.z), RGBA);
    AddLine(glm::vec3(minXYZ.x, maxXYZ.y, maxXYZ.z), glm::vec3(minXYZ.x, maxXYZ.y, minXYZ.z), RGBA);

    // Vertical edges
    AddLine(glm::vec3(minXYZ.x, minXYZ.y, minXYZ.z), glm::vec3(minXYZ.x, maxXYZ.y, minXYZ.z), RGBA);
    AddLine(glm::vec3(maxXYZ.x, minXYZ.y, minXYZ.z), glm::vec3(maxXYZ.x, maxXYZ.y, minXYZ.z), RGBA);
    AddLine(glm::vec3(maxXYZ.x, minXYZ.y, maxXYZ.z), glm::vec3(maxXYZ.x, maxXYZ.y, maxXYZ.z), RGBA);
    AddLine(glm::vec3(minXYZ.x, minXYZ.y, maxXYZ.z), glm::vec3(minXYZ.x, maxXYZ.y, maxXYZ.z), RGBA);
}

void DebugSystem::AddSphere(glm::vec3 position, float radius)
{
    // For now we only deal with 1 debug sphere and update its position and scale at render time
    std::pair<glm::vec3, float> pairSphere;
    pairSphere.first = position;
    pairSphere.second = radius;
    m_vecSpheresToDraw.push_back(pairSphere);
}

void DebugSystem::Update(double deltaTime, glm::mat4 matView, glm::mat4 matProjection)
{
    m_pShaderManager->UseShaderProgram(m_debugShaderID);

    m_pShaderProgram->IsWireframe(true);

    m_pShaderProgram->SetUniformMatrix4f("matView", matView);
    m_pShaderProgram->SetUniformMatrix4f("matProjection", matProjection);

    m_DrawLines();
    m_DrawSpheres();

    return;
}

void DebugSystem::m_InitializeLineVertex()
{
    // Allocate a C++/CPU side array to hold our lines
    m_pLinesVerticesToDraw = new sLineVertex[LINE_VERTEX_BUFFER_SIZE];
    // Clear this to zero
    memset(m_pLinesVerticesToDraw, 0,
        LINE_VERTEX_BUFFER_SIZE * sizeof(sLineVertex));
    m_sizeOfLineVBO = 0;
}

void DebugSystem::m_DrawLines()
{
    glm::mat4 matModel = glm::mat4(1.0f);
    glm::mat4 matModelIT = glm::inverse(glm::transpose(matModel));
    m_pShaderProgram->SetUniformMatrix4f("matModel", matModel);
    m_pShaderProgram->SetUniformMatrix4f("matModel_IT", matModelIT);

    // Bind VAO
    glBindVertexArray(m_debugVAO);

    // Update VBO data
    glBindBuffer(GL_ARRAY_BUFFER, m_debugVBO);
    // Overwrite the contents of an exiting buffer
    glBufferSubData(GL_ARRAY_BUFFER,
        0,
        m_sizeOfLineVBO * sizeof(sLineVertex),
        (GLvoid*)m_pLinesVerticesToDraw);

    // Draw the lines
    glDrawArrays(GL_LINES, 0, m_sizeOfLineVBO);

    // Unbind VAO and VBO
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DebugSystem::m_DrawSpheres()
{
    using namespace std;
    using namespace glm;
    using namespace myutils;

    for (pair<vec3, float> sphere : m_vecSpheresToDraw)
    {
        mat4 matModel(1.0f);
        ApplyTransformInModelMat(sphere.first, vec3(0.0f), sphere.second, matModel);
        glm::mat4 matModelIT = glm::inverse(glm::transpose(matModel));

        this->m_pShaderProgram->SetUniformMatrix4f("matModel", matModel);
        this->m_pShaderProgram->SetUniformMatrix4f("matModel_IT", matModelIT);

        glBindVertexArray(m_pSphereMesh->VAO_ID);
        glDrawElements(GL_TRIANGLES,
            m_pSphereMesh->numberOfIndices,
            GL_UNSIGNED_INT,
            0);
        glBindVertexArray(0);
    }
}