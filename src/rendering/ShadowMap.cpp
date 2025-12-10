#include "ShadowMap.h"
#include "rlgl.h"
#include <cmath>
#include <cstdio>

ShadowMap::ShadowMap(Shader* shader, int res)
    : shadowShader(shader)
    , resolution(res)
{
    // Create a standard render texture
    shadowRT = LoadRenderTexture(resolution, resolution);
    
    printf("Shadow RT created: texture.id=%d, depth.id=%d\n", shadowRT.texture.id, shadowRT.depth.id);

    if (shadowShader)
    {
        locLightView = GetShaderLocation(*shadowShader, "u_lightView");
        locLightProj = GetShaderLocation(*shadowShader, "u_lightProj");
    }
}

ShadowMap::~ShadowMap()
{
    UnloadRenderTexture(shadowRT);
}

void ShadowMap::UpdateLightMatrices(Vector3 lightDir, Vector3 focusPoint)
{
    Vector3 dir = Vector3Normalize(lightDir);

    // Position light far enough to cover the scene
    float distance = 40.0f;
    Vector3 lightPos = Vector3Subtract(focusPoint, Vector3Scale(dir, distance));

    // Compute proper up vector
    Vector3 up = { 0.0f, 1.0f, 0.0f };
    float dotUp = fabsf(Vector3DotProduct(dir, up));
    if (dotUp > 0.99f)
        up = { 1.0f, 0.0f, 0.0f };

    lightView = MatrixLookAt(lightPos, focusPoint, up);
    
    // Create orthographic projection for directional light
    // Tighter bounds for better shadow resolution
    lightProj = MatrixOrtho(-15.0f, 15.0f,
                            -15.0f, 15.0f,
                            1.0f, 80.0f);
}

void ShadowMap::BeginDepthPass()
{
    BeginTextureMode(shadowRT);
    
    // Clear to BLACK (depth = 0.0)
    ClearBackground(BLACK);
    
    // Make sure depth test is enabled
    rlEnableDepthTest();
    
    // Enable depth mask to ensure depth is written
    rlEnableDepthMask();
}

void ShadowMap::EndDepthPass()
{
    EndTextureMode();
}

Texture ShadowMap::GetDepthTexture() const
{
    // Use the COLOR texture since we're rendering depth as color
    return shadowRT.texture;
}