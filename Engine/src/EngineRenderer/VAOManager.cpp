#include "EngineRenderer/VAOManager.h"
#include "common/opengl.h"
#include "common/utils.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <sstream>
#include <fstream>
#include "cFileLoader_Imp.h"

VAOManager::VAOManager(iShaderInfo* pShaderInfo)
{
    this->m_pShaderInfo = pShaderInfo;
}

VAOManager::~VAOManager()
{
}

void VAOManager::DestroyVBO(std::string modelName)
{
    sMesh* pMesh = this->FindMeshByModelName(modelName);
    if (!pMesh)
    {
        //Model not loaded
        return;
    }
    glDeleteBuffers(1, &(pMesh->vertexBufferID));
    delete pMesh;
    this->m_mapModelNameToMesh.erase(modelName);

    return;
}

void VAOManager::SetBasePath(std::string basePath)
{
    this->m_basePath = basePath;
    return;
}

sMesh* VAOManager::LoadModelIntoVAO(std::string& fileName, int shaderID, bool bIsDynamicBuffer, 
                                    bool hasNormals, bool hasTexture)
{
    // Load the model from file
    // Checking if the info already exist in our map
    sMesh* pMesh = this->FindMeshByModelName(fileName);
    if (pMesh)
    {
        // Info is already loaded
        return pMesh;
    }
    pMesh = new sMesh();
    pMesh->name = fileName;

    if (!this->m_LoadMeshData(fileName, pMesh))
    {
        return nullptr;
    }

    // Create a VAO (Vertex Array Object), which will 
    //	keep track of all the 'state' needed to draw 
    //	from this buffer...

    // Ask OpenGL for a new buffer ID...
    glGenVertexArrays(1, &(pMesh->VAO_ID));
    // "Bind" this buffer:
    // - aka "make this the 'current' VAO buffer
    glBindVertexArray(pMesh->VAO_ID);

    // Now ANY state that is related to vertex or index buffer
    //	and vertex attribute layout, is stored in the 'state' 
    //	of the VAO... 
    glGenBuffers(1, &(pMesh->vertexBufferID));

    glBindBuffer(GL_ARRAY_BUFFER, pMesh->vertexBufferID);

    glBufferData(GL_ARRAY_BUFFER,
        sizeof(sVertex) * pMesh->numberOfVertices,
        (GLvoid*)pMesh->pVertices,
        (bIsDynamicBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));

    // Copy the index buffer into the video card, 
    // too create an index buffer.
    glGenBuffers(1, &(pMesh->indexBufferID));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->indexBufferID);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(uint) * pMesh->numberOfIndices,
        (GLvoid*)pMesh->pIndices,
        GL_STATIC_DRAW);

    // Set the vertex attributes.
    GLint vpos_location = this->m_pShaderInfo->GetAL(shaderID, "vPos");
    GLint vcol_location = this->m_pShaderInfo->GetAL(shaderID, "vCol");
    GLint vNormal_location = this->m_pShaderInfo->GetAL(shaderID, "vNormal");
    GLint vTextureCoords_location = this->m_pShaderInfo->GetAL(shaderID, "vTextureCoords");

    // Set the vertex attributes for this shader
    // vPos
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, x));
    // vCol 
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 4,
        GL_FLOAT, GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, r));

    if (hasNormals)
    {
        glEnableVertexAttribArray(vNormal_location);
        glVertexAttribPointer(vNormal_location, 4,
            GL_FLOAT, GL_FALSE,
            sizeof(sVertex),
            (void*)offsetof(sVertex, nx));
    }

    if (hasTexture)
    {
        glEnableVertexAttribArray(vTextureCoords_location);
        glVertexAttribPointer(vTextureCoords_location, 2,
            GL_FLOAT, GL_FALSE,
            sizeof(sVertex),
            (void*)offsetof(sVertex, u));
    }

    // Now that all the parts are set up, set the VAO to zero
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(vpos_location);
    glDisableVertexAttribArray(vcol_location);

    if (hasNormals)
    {
        glDisableVertexAttribArray(vNormal_location);
    }

    if (hasNormals)
    {
        glDisableVertexAttribArray(vTextureCoords_location);
    }

    // Store the draw information into the map
    this->m_mapModelNameToMesh[pMesh->name] = pMesh;

    return pMesh;
}

sMesh* VAOManager::LoadModelIntoVAO(std::string& fileName,
    int shaderID,
    bool bIsDynamicBuffer /*=false*/)

{
    return LoadModelIntoVAO(fileName, shaderID, bIsDynamicBuffer, true, true);
}

// We don't want to return an int, likely
sMesh* VAOManager::FindMeshByModelName(std::string& filename)
{
    

    std::map< std::string /*model name*/,
        sMesh* /* info needed to draw*/ >::iterator
        itDrawInfo = this->m_mapModelNameToMesh.find(filename);

    // Find it? 
    if (itDrawInfo == this->m_mapModelNameToMesh.end())
    {
        // Nope
        return nullptr;
    }

    return itDrawInfo->second;
}

bool VAOManager::m_LoadMeshData(std::string theFileName, sMesh* pMesh)
{
    AssimpHelper::cFileLoader helper = AssimpHelper::cFileLoader();
    AssimpHelper::cFileLoader::sPostProcessFlags loadFlags;

    std::string fileFullPath = this->m_basePath + theFileName;
    bool isMeshLoaded = helper.Load3DModelFile(fileFullPath, loadFlags, pMesh);
    if (!isMeshLoaded)
    {
        std::string errorMsg = "Error: " + std::string(helper.getLastError().c_str());
        CheckEngineError(errorMsg.c_str());
        return false;
    }

    return true;
}

bool VAOManager::UpdateVAOBuffers(std::string& fileName,
                                  std::string& shaderProgram,
                                  sMesh* pUpdatedMesh)
{
    

    // This exists? 
    if (!this->FindMeshByModelName(fileName))
    {
        // Mesh not loaded
        return false;
    }

    // Set VBO
    glBindBuffer(GL_ARRAY_BUFFER, pUpdatedMesh->vertexBufferID);
    // Update data in buffer
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,  // Offset
                    sizeof(sVertex) * pUpdatedMesh->numberOfVertices,
                    (GLvoid*)pUpdatedMesh->pVertices);
    // Unbind VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Set index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pUpdatedMesh->indexBufferID);
    // Update data in buffer
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                    0,  // Offset
                    sizeof(uint) * pUpdatedMesh->numberOfIndices,
                    (GLvoid*)pUpdatedMesh->pIndices);
    // Unbind index buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}
