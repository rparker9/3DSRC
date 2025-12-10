#include "MeshRenderer.h"
#include "GameObject.h"
#include "Transform3D.h"
#include "MeshFilter.h"
#include "raymath.h"
#include "rlgl.h"
#include <cstdio>

Shader* MeshRenderer::sLightingShader = nullptr;
Shader* MeshRenderer::sShadowShader   = nullptr;

MeshRenderer::MeshRenderer(MeshType type, Color col)
    : meshType(type)
    , color(col)
{
    if (meshType == CUSTOM)
    {
        hasModel  = false;
        ownsModel = false;
        return;
    }

    Mesh mesh{};
    switch (meshType)
    {
        case CUBE:   mesh = GenMeshCube(1.0f, 1.0f, 1.0f); break;
        case SPHERE: mesh = GenMeshSphere(0.5f, 16, 16);   break;
        case PLANE:  mesh = GenMeshPlane(1.0f, 1.0f, 1, 1);break;
        case CUSTOM: default: break;
    }

    if (mesh.vertexCount > 0)
    {
        model    = LoadModelFromMesh(mesh);
        hasModel = true;
        ownsModel = true;
    }
}

MeshRenderer::~MeshRenderer()
{
    if (hasModel && ownsModel)
    {
        UnloadModel(model);
        hasModel  = false;
        ownsModel = false;
    }
}

void MeshRenderer::SetColor(Color col)
{
    color = col;
}

void MeshRenderer::SetDiffuseTexture(Texture2D tex)
{
    diffuse    = tex;
    hasTexture = true;
}

void MeshRenderer::SetModel(Model m, bool takeOwnership)
{
    if (hasModel && ownsModel)
        UnloadModel(model);

    model     = m;
    hasModel  = true;
    ownsModel = takeOwnership;
    meshType  = CUSTOM;
}

void MeshRenderer::SetGlobalShader(Shader* shader)
{
    sLightingShader = shader;
}

void MeshRenderer::SetShadowShader(Shader* shader)
{
    sShadowShader = shader;
}

void MeshRenderer::Draw()
{
    // Decide which model to draw
    Model* drawModel = nullptr;

    if (meshType == CUSTOM)
    {
        MeshFilter* filter = gameObject->GetComponent<MeshFilter>();
        if (filter && filter->HasModel())
        {
            drawModel = &filter->GetModel();
        }
        else if (hasModel)
        {
            drawModel = &model;
        }
        else
        {
            return;
        }
    }
    else
    {
        if (!hasModel) return;
        drawModel = &model;
    }

    Transform3D* t = gameObject->GetTransform();
    if (!t || !drawModel) return;

    Vector3 pos   = t->GetPosition();
    Vector3 rot   = t->GetRotation();
    Vector3 scale = t->GetScale();

    // Hook up lighting shader
    if (sLightingShader)
        drawModel->materials[0].shader = *sLightingShader;

    drawModel->materials[0].maps[MATERIAL_MAP_DIFFUSE].color = color;
    if (hasTexture)
        drawModel->materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = diffuse;

    DrawModelEx(*drawModel, pos, { 0, 1, 0 }, rot.y, scale, WHITE);
}

void MeshRenderer::DrawShadow()
{
    static int drawCount = 0;
    
    if (!sShadowShader)
    {
        printf("DrawShadow: No shadow shader!\n");
        return;
    }

    Model* drawModel = nullptr;

    if (meshType == CUSTOM)
    {
        MeshFilter* filter = gameObject->GetComponent<MeshFilter>();
        if (filter && filter->HasModel())
        {
            drawModel = &filter->GetModel();
        }
        else if (hasModel)
        {
            drawModel = &model;
        }
        else
        {
            return;
        }
    }
    else
    {
        if (!hasModel) return;
        drawModel = &model;
    }

    Transform3D* t = gameObject->GetTransform();
    if (!t || !drawModel) return;

    Vector3 pos   = t->GetPosition();
    Vector3 rot   = t->GetRotation();
    Vector3 scale = t->GetScale();

    if (drawCount < 1)
    {
        printf("DrawShadow called: pos=(%.1f,%.1f,%.1f), shader.id=%d\n", 
               pos.x, pos.y, pos.z, sShadowShader->id);
        drawCount++;
    }

    // Temporarily override the model's shader
    Shader oldShader = drawModel->materials[0].shader;
    drawModel->materials[0].shader = *sShadowShader;
    
    // Draw using raylib's normal function
    DrawModelEx(*drawModel, pos, { 0, 1, 0 }, rot.y, scale, WHITE);
    
    // Restore original shader
    drawModel->materials[0].shader = oldShader;
}