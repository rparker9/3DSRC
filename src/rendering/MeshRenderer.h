#pragma once

#include "raylib.h"
#include "Component.h"

class MeshFilter;

/// <summary>
/// Renders a 3D model for a GameObject.
/// Supports simple built-in primitives (cube, sphere, plane) or CUSTOM
/// geometry provided by a MeshFilter on the same GameObject.
/// Also participates in the shadow pass.
/// </summary>
class MeshRenderer : public Component
{
public:
    enum MeshType { CUBE, SPHERE, PLANE, CUSTOM };

private:
    MeshType meshType = CUBE;

    Model model{};      // internal model for primitives
    bool  hasModel  = false;
    bool  ownsModel = false;

    Color    color      = WHITE;
    Texture2D diffuse   = { 0 };
    bool     hasTexture = false;

    // Global shader pointers for all MeshRenderer instances
    static Shader* sLightingShader;
    static Shader* sShadowShader;

public:
    MeshRenderer(MeshType type = CUBE, Color col = WHITE);
    ~MeshRenderer();

    void SetColor(Color col);
    void SetDiffuseTexture(Texture2D tex);
    void SetModel(Model m, bool takeOwnership = true);

    static void SetGlobalShader(Shader* shader);
    static void SetShadowShader(Shader* shader);

    void Draw() override;
    void DrawShadow() override;
};
