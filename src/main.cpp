#include "raylib.h"
#include "rlgl.h"

#include "GameObject.h"
#include "Transform3D.h"
#include "CameraComponent.h"
#include "CameraController.h"
#include "PlayerController.h"
#include "BoxCollider.h"
#include "MeshRenderer.h"
#include "LightComponent.h"
#include "ShadowMap.h"

#include <memory>
#include <cstdio>

std::shared_ptr<GameObject> CreateScene(Shader* lightingShader,
                                        Shader* shadowShader,
                                        LightComponent** outLight,
                                        ShadowMap** outShadow)
{
    std::shared_ptr<GameObject> scene = std::make_shared<GameObject>("Scene Root");

    // Player
    auto player = std::make_shared<GameObject>("Player");
    player->GetTransform()->SetPosition({ 0, 1, 0 });
    player->AddComponent<CameraComponent>();
    player->AddComponent<CameraController>(0.15f);
    player->AddComponent<BoxCollider>(Vector3{ 0.7f, 2.0f, 0.7f }, Vector3{ 0, 0, 0 }, false);
    player->AddComponent<PlayerController>(5.0f, scene.get());
    scene->AddChild(player);

    // Ground
    auto ground = std::make_shared<GameObject>("Ground");
    ground->GetTransform()->SetPosition({ 0, -0.5f, 0 });
    ground->GetTransform()->SetScale({ 20, 1, 20 });
    ground->AddComponent<MeshRenderer>(MeshRenderer::CUBE, GRAY);
    ground->AddComponent<BoxCollider>(Vector3{ 20, 1, 20 }, Vector3{ 0, 0, 0 }, false);
    scene->AddChild(ground);

    // Obstacles
    for (int i = 0; i < 5; ++i)
    {
        auto cube = std::make_shared<GameObject>("Cube_" + std::to_string(i));
        float x = (i - 2) * 3.0f;
        cube->GetTransform()->SetPosition({ x, 0.75f, -5 });
        cube->GetTransform()->SetScale({ 2, 1.5f, 2 });

        Color c = { (unsigned char)(50 + i * 40), 100, 150, 255 };

        cube->AddComponent<MeshRenderer>(MeshRenderer::CUBE, c);
        cube->AddComponent<BoxCollider>(Vector3{ 2, 1.5f, 2 }, Vector3{ 0, 0, 0 }, false);
        scene->AddChild(cube);
    }

    // Walls
    auto wall1 = std::make_shared<GameObject>("Wall1");
    wall1->GetTransform()->SetPosition({ 10, 2, 0 });
    wall1->GetTransform()->SetScale({ 1, 4, 20 });
    wall1->AddComponent<MeshRenderer>(MeshRenderer::CUBE, BROWN);
    wall1->AddComponent<BoxCollider>(Vector3{ 1, 4, 20 }, Vector3{ 0, 0, 0 }, false);
    scene->AddChild(wall1);

    auto wall2 = std::make_shared<GameObject>("Wall2");
    wall2->GetTransform()->SetPosition({ -10, 2, 0 });
    wall2->GetTransform()->SetScale({ 1, 4, 20 });
    wall2->AddComponent<MeshRenderer>(MeshRenderer::CUBE, BROWN);
    wall2->AddComponent<BoxCollider>(Vector3{ 1, 4, 20 }, Vector3{ 0, 0, 0 }, false);
    scene->AddChild(wall2);

    // Sun light + ShadowMap
    auto sun = std::make_shared<GameObject>("SunLight");

    LightComponent* light = sun->AddComponent<LightComponent>(lightingShader);
    light->SetDirection({ -0.3f, -1.0f, -0.2f });
    light->SetColor(WHITE, 1.0f);
    light->SetAmbient({ 0.15f, 0.15f, 0.20f });

    ShadowMap* sm = sun->AddComponent<ShadowMap>(shadowShader, 2048);

    scene->AddChild(sun);

    if (outLight)  *outLight  = light;
    if (outShadow) *outShadow = sm;

    return scene;
}

int main()
{
    const int SCREEN_WIDTH  = 1280;
    const int SCREEN_HEIGHT = 720;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "3DSRC - Shadow Debug");
    SetTargetFPS(60);
    DisableCursor();

    // Shaders
    Shader lightingShader = LoadShader("lighting.vs", "lighting.fs");
    Shader shadowShader   = LoadShader("shadow.vs", "shadow.fs");

    // Debug: Print shader locations
    printf("=== LIGHTING SHADER ===\n");
    printf("MVP: %d\n", GetShaderLocation(lightingShader, "mvp"));
    printf("matModel: %d\n", GetShaderLocation(lightingShader, "matModel"));
    printf("matNormal: %d\n", GetShaderLocation(lightingShader, "matNormal"));
    printf("u_lightSpaceMatrix: %d\n", GetShaderLocation(lightingShader, "u_lightSpaceMatrix"));
    printf("u_shadowMap: %d\n", GetShaderLocation(lightingShader, "u_shadowMap"));
    printf("u_viewPos: %d\n", GetShaderLocation(lightingShader, "u_viewPos"));
    
    printf("\n=== SHADOW SHADER ===\n");
    printf("MVP: %d\n", GetShaderLocation(shadowShader, "mvp"));

    // Hook up matrix locations for raylib
    lightingShader.locs[SHADER_LOC_MATRIX_MVP]    = GetShaderLocation(lightingShader, "mvp");
    lightingShader.locs[SHADER_LOC_MATRIX_MODEL]  = GetShaderLocation(lightingShader, "matModel");
    lightingShader.locs[SHADER_LOC_MATRIX_NORMAL] = GetShaderLocation(lightingShader, "matNormal");

    // IMPORTANT: Set MVP location for shadow shader
    shadowShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shadowShader, "mvp");

    // Shadow sampler uses texture unit 1
    int locShadowMap = GetShaderLocation(lightingShader, "u_shadowMap");
    if (locShadowMap >= 0)
    {
        int samplerIndex = 1;
        SetShaderValue(lightingShader, locShadowMap, &samplerIndex, SHADER_UNIFORM_INT);
    }

    MeshRenderer::SetGlobalShader(&lightingShader);
    MeshRenderer::SetShadowShader(&shadowShader);

    // Scene + light + shadow map
    LightComponent* sunLight = nullptr;
    ShadowMap*      shadowMap = nullptr;

    std::shared_ptr<GameObject> scene =
        CreateScene(&lightingShader, &shadowShader, &sunLight, &shadowMap);
    scene->Start();

    GameObject* player = scene->GetChildren()[0].get();
    CameraComponent* camera = player->GetComponent<CameraComponent>();

    int locViewPos    = GetShaderLocation(lightingShader, "u_viewPos");
    int locLightSpace = GetShaderLocation(lightingShader, "u_lightSpaceMatrix");

    bool showShadowMap = true;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Toggle shadow map display
        if (IsKeyPressed(KEY_TAB))
            showShadowMap = !showShadowMap;

        scene->Update(dt);
        scene->LateUpdate(dt);

        // Shadow pass
        if (sunLight && shadowMap)
        {
            Vector3 focus = { 0.0f, 0.0f, 0.0f };
            Vector3 lightDir = sunLight->GetDirection();

            shadowMap->UpdateLightMatrices(lightDir, focus);

            Matrix lightView  = shadowMap->GetLightView();
            Matrix lightProj  = shadowMap->GetLightProj();
            
            // Compute light space matrix for main shader
            Matrix lightSpace = MatrixMultiply(lightView, lightProj);

            if (locLightSpace >= 0)
                SetShaderValueMatrix(lightingShader, locLightSpace, lightSpace);

            // *** KEY FIX: Set the light's view-projection as the "camera" for shadow pass ***
            shadowMap->BeginDepthPass();
            
            // Manually set rlgl's projection and modelview matrices for the shadow pass
            rlMatrixMode(RL_PROJECTION);
            rlLoadIdentity();
            rlMultMatrixf(MatrixToFloat(lightProj));
            
            rlMatrixMode(RL_MODELVIEW);
            rlLoadIdentity();
            rlMultMatrixf(MatrixToFloat(lightView));
            
            // Now draw - raylib will compute MVP = projection * modelview * model
            scene->DrawShadow();
            
            shadowMap->EndDepthPass();
        }

        // Camera position for specular
        if (camera && locViewPos >= 0)
        {
            Vector3 camPos = player->GetTransform()->GetPosition();
            SetShaderValue(lightingShader, locViewPos, &camPos.x, SHADER_UNIFORM_VEC3);
        }

        // Bind shadow map to texture unit 1 BEFORE drawing
        if (shadowMap)
        {
            Texture depthTex = shadowMap->GetDepthTexture();
            rlActiveTextureSlot(1);
            rlEnableTexture(depthTex.id);
            rlActiveTextureSlot(0); // Switch back to 0 for regular textures
        }

        BeginDrawing();
        {
            ClearBackground(SKYBLUE);

            if (camera)
            {
                camera->BeginMode();
                scene->Draw();
                camera->EndMode();
            }

            // Debug shadow map display
            if (showShadowMap && shadowMap)
            {
                Texture shadowTex = shadowMap->GetDepthTexture();
                int size = 256;
                Rectangle src = { 0, 0, (float)shadowTex.width, -(float)shadowTex.height };
                Rectangle dst = { (float)(SCREEN_WIDTH - size - 10), 10, (float)size, (float)size };
                
                DrawTexturePro(shadowTex, src, dst, {0,0}, 0, WHITE);
                DrawRectangleLines((int)dst.x, (int)dst.y, (int)dst.width, (int)dst.height, RED);
                DrawText("Shadow Map (TAB)", (int)dst.x, (int)dst.y - 20, 16, RED);
            }

            // Debug info
            DrawText("3DSRC - Shadow Debug", 10, 10, 20, DARKGRAY);
            DrawFPS(10, 40);
            DrawText("WASD: Move | Space: Jump | TAB: Toggle Shadow Map", 10, 70, 16, DARKGRAY);
        }
        EndDrawing();
    }

    UnloadShader(lightingShader);
    UnloadShader(shadowShader);
    CloseWindow();
    return 0;
}