// RenderPipeline.cpp
#include "RenderPipeline.h"

#include "rlgl.h"

#include "GameObject.h"
#include "Transform3D.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "ShadowMap.h"
#include "MeshRenderer.h"
#include "PlayerController.h"

#include <cstdio>

RenderPipeline::RenderPipeline(int screenWidth, int screenHeight)
    : m_screenWidth(screenWidth)
    , m_screenHeight(screenHeight)
{
}

bool RenderPipeline::Initialize()
{
    // --- Load shaders from subdirectory ---
    const char* shaderDir = "shaders/";

    m_lightingShader = LoadShader(
        TextFormat("%slighting.vs", shaderDir),
        TextFormat("%slighting.fs", shaderDir)
    );
    m_shadowShader = LoadShader(
        TextFormat("%sshadow.vs", shaderDir),
        TextFormat("%sshadow.fs", shaderDir)
    );

    // Debug: Print shader locations (unchanged)
    std::printf("=== LIGHTING SHADER ===\n");
    std::printf("MVP: %d\n",       GetShaderLocation(m_lightingShader, "mvp"));
    std::printf("matModel: %d\n",  GetShaderLocation(m_lightingShader, "matModel"));
    std::printf("matNormal: %d\n", GetShaderLocation(m_lightingShader, "matNormal"));
    std::printf("u_lightSpaceMatrix: %d\n", GetShaderLocation(m_lightingShader, "u_lightSpaceMatrix"));
    std::printf("u_shadowMap: %d\n",        GetShaderLocation(m_lightingShader, "u_shadowMap"));
    std::printf("u_viewPos: %d\n",          GetShaderLocation(m_lightingShader, "u_viewPos"));

    std::printf("\n=== SHADOW SHADER ===\n");
    std::printf("MVP: %d\n", GetShaderLocation(m_shadowShader, "mvp"));

    // --- Hook up matrix locations for raylib (unchanged) ---
    m_lightingShader.locs[SHADER_LOC_MATRIX_MVP]    = GetShaderLocation(m_lightingShader, "mvp");
    m_lightingShader.locs[SHADER_LOC_MATRIX_MODEL]  = GetShaderLocation(m_lightingShader, "matModel");
    m_lightingShader.locs[SHADER_LOC_MATRIX_NORMAL] = GetShaderLocation(m_lightingShader, "matNormal");

    m_shadowShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(m_shadowShader, "mvp");

    // Shadow sampler uses texture unit 1 (unchanged)
    int locShadowMap = GetShaderLocation(m_lightingShader, "u_shadowMap");
    if (locShadowMap >= 0)
    {
        int samplerIndex = 1;
        SetShaderValue(m_lightingShader, locShadowMap, &samplerIndex, SHADER_UNIFORM_INT);
    }

    // Tell MeshRenderer which shaders to use (same as before)
    MeshRenderer::SetGlobalShader(&m_lightingShader);
    MeshRenderer::SetShadowShader(&m_shadowShader);

    // Cache uniform locations we need every frame
    m_locViewPos    = GetShaderLocation(m_lightingShader, "u_viewPos");
    m_locLightSpace = GetShaderLocation(m_lightingShader, "u_lightSpaceMatrix");

    m_showShadowMap = true;

    // Important: scene is bound later via SetScene()
    return true;
}

Shader* RenderPipeline::GetLightingShader()
{
    return &m_lightingShader;
}

Shader* RenderPipeline::GetShadowShader()
{
    return &m_shadowShader;
}

void RenderPipeline::SetScene(const std::shared_ptr<GameObject>& scene,
                              GameObject* player,
                              CameraComponent* camera,
                              LightComponent* sunLight,
                              ShadowMap* shadowMap)
{
    // Renderer just stores references; it does NOT control lifecycle.
    m_scene     = scene;
    m_player    = player;
    m_camera    = camera;
    m_sunLight  = sunLight;
    m_shadowMap = shadowMap;
}

void RenderPipeline::Tick()
{
    if (!m_scene)
    {
        BeginDrawing();
        ClearBackground(SKYBLUE);
        DrawText("No scene bound to RenderPipeline", 10, 10, 20, RED);
        EndDrawing();
        return;
    }

    // Input that is render-related (like toggling a debug overlay) is still fine here.
    if (IsKeyPressed(KEY_TAB))
        m_showShadowMap = !m_showShadowMap;

    // NOTE: We NO LONGER call m_scene->Update / LateUpdate here.
    // The scene is assumed to already be in the correct state for this frame.

    // --- SHADOW PASS (unchanged) ---
    if (m_sunLight && m_shadowMap)
    {
        Vector3 focus    = { 0.0f, 0.0f, 0.0f };
        Vector3 lightDir = m_sunLight->GetDirection();

        m_shadowMap->UpdateLightMatrices(lightDir, focus);

        Matrix lightView = m_shadowMap->GetLightView();
        Matrix lightProj = m_shadowMap->GetLightProj();

        Matrix lightSpace = MatrixMultiply(lightView, lightProj);

        if (m_locLightSpace >= 0)
            SetShaderValueMatrix(m_lightingShader, m_locLightSpace, lightSpace);

        m_shadowMap->BeginDepthPass();

        rlMatrixMode(RL_PROJECTION);
        rlLoadIdentity();
        rlMultMatrixf(MatrixToFloat(lightProj));

        rlMatrixMode(RL_MODELVIEW);
        rlLoadIdentity();
        rlMultMatrixf(MatrixToFloat(lightView));

        m_scene->DrawShadow();

        m_shadowMap->EndDepthPass();
    }

    // --- Per-frame uniforms for lighting shader ---
    if (m_camera && m_locViewPos >= 0 && m_player)
    {
        Vector3 camPos = m_player->GetTransform()->GetPosition();
        SetShaderValue(m_lightingShader, m_locViewPos, &camPos.x, SHADER_UNIFORM_VEC3);
    }

    if (m_shadowMap)
    {
        Texture depthTex = m_shadowMap->GetDepthTexture();
        rlActiveTextureSlot(1);
        rlEnableTexture(depthTex.id);
        rlActiveTextureSlot(0);
    }

    // --- FINAL RENDER PASS ---
    BeginDrawing();
    {
        ClearBackground(SKYBLUE);

        if (m_camera)
        {
            m_camera->BeginMode();
            m_scene->Draw();
            m_camera->EndMode();
        }

        if (m_showShadowMap && m_shadowMap)
        {
            Texture shadowTex = m_shadowMap->GetDepthTexture();
            int size = 256;
            Rectangle src = 
            { 
                0,                          // x
                0,                          // y
                (float)shadowTex.width,     // width
                -(float)shadowTex.height    // height
            };
            Rectangle dst = 
            { 
                (float)(m_screenWidth - size - 10), // x 
                20,                                 // y
                (float)size,                        // width
                (float)size                         // height
            };

            // Draw shadow map depth texture in top-right corner
            DrawTexturePro(shadowTex, src, dst, { 0, 0 }, 0, WHITE);
            DrawRectangleLines((int)dst.x, (int)dst.y, (int)dst.width, (int)dst.height, RED);
            DrawText("Shadow Map (TAB)", (int)dst.x, (int)dst.y - 20, 16, RED);
        }

        DrawText("3DSRC INDEV v0.02", 10, 10, 20, WHITE);
        DrawFPS(10, 40);
        // Draw text showing if player is grounded
        if (m_player)
        {
            PlayerController* pc = m_player->GetComponent<PlayerController>();
            if (pc)
            {
                const char* groundedText = pc->IsGrounded() ? "Grounded" : "Airborne";
                DrawText(groundedText, 10, 70, 20, pc->IsGrounded() ? GREEN : RED);
            }
        }
    }
    EndDrawing();
}

void RenderPipeline::Shutdown()
{
    UnloadShader(m_lightingShader);
    UnloadShader(m_shadowShader);
}
