#pragma once

#include "Component.h"
#include "raylib.h"
#include "raymath.h"

/// <summary>
/// Manages a RenderTexture used as a shadow map for a single
/// directional light, plus its view/projection matrices.
/// </summary>
class ShadowMap : public Component
{
private:
    RenderTexture2D shadowRT{};
    Shader* shadowShader = nullptr;

    Matrix lightView = MatrixIdentity();
    Matrix lightProj = MatrixIdentity();

    // Tunable shadow volume
    float orthoSize = 20.0f;
    float nearPlane = 1.0f;
    float farPlane  = 60.0f;

    int locLightView = -1;
    int locLightProj = -1;

    int resolution = 2048;

public:
    ShadowMap(Shader* shader, int res = 2048);
    ~ShadowMap();

    void UpdateLightMatrices(Vector3 lightDir, Vector3 focusPoint);

    void BeginDepthPass();
    void EndDepthPass();

    Texture GetDepthTexture() const;

    const Matrix& GetLightView() const { return lightView; }
    const Matrix& GetLightProj() const { return lightProj; }
};
