#include "DemoScene3D.h"

#include "RenderPipeline.h"
#include "GameObject.h"
#include "Transform3D.h"
#include "CameraComponent.h"
#include "CameraController.h"
#include "PlayerController.h"
#include "BoxCollider.h"
#include "MeshRenderer.h"
#include "MeshFilter.h"
#include "LightComponent.h"
#include "ShadowMap.h"

#include "raylib.h"

DemoScene3D::DemoScene3D(RenderPipeline& pipelineRef)
    : pipeline(pipelineRef)
{
    // Build the GameObject graph and cache references.
    BuildSceneGraph();
}

void DemoScene3D::BuildSceneGraph()
{
    sceneRoot = std::make_shared<GameObject>("Scene Root");

    // -------------------
    // Player
    // -------------------
    auto playerGO = std::make_shared<GameObject>("Player");
    playerGO->GetTransform()->SetPosition({ 0, 1, 0 });
    playerGO->AddComponent<CameraController>(0.15f);
    playerGO->AddComponent<CameraComponent>();
    playerGO->AddComponent<BoxCollider>(
        Vector3{ 0.7f, 2.0f, 0.7f },    // size
        Vector3{ 0, 0, 0 },             // offset
        false                           // show collider
    );
    playerGO->AddComponent<PlayerController>(5.0f, sceneRoot.get());
    sceneRoot->AddChild(playerGO);

    player = playerGO.get();
    camera = player->GetComponent<CameraComponent>();

    // -------------------
    // Ground
    // -------------------
    auto ground = std::make_shared<GameObject>("Ground");
    ground->GetTransform()->SetPosition({ 0, -0.5f, 0 });
    ground->GetTransform()->SetScale({ 20, 1, 20 });
    ground->AddComponent<MeshRenderer>(MeshRenderer::CUBE, GRAY);
    ground->AddComponent<BoxCollider>(Vector3{ 20, 1, 20 }, Vector3{ 0, 0, 0 }, false);
    sceneRoot->AddChild(ground);

    // -------------------
    // Obstacles
    // -------------------
    for (int i = 0; i < 5; ++i)
    {
        auto cube = std::make_shared<GameObject>("Cube_" + std::to_string(i));
        float x = (i - 2) * 3.0f;
        cube->GetTransform()->SetPosition({ x, 0.75f, -5 });
        cube->GetTransform()->SetScale({ 2, 1.5f, 2 });

        Color c = { static_cast<unsigned char>(50 + i * 40), 100, 150, 255 };

        cube->AddComponent<MeshRenderer>(MeshRenderer::CUBE, c);
        cube->AddComponent<BoxCollider>(Vector3{ 2, 1.5f, 2 }, Vector3{ 0, 0, 0 }, false);
        sceneRoot->AddChild(cube);
    }

    // -------------------
    // Walls
    // -------------------
    auto wall1 = std::make_shared<GameObject>("Wall1");
    wall1->GetTransform()->SetPosition({ 10, 2, 0 });
    wall1->GetTransform()->SetScale({ 1, 4, 20 });
    wall1->AddComponent<MeshRenderer>(MeshRenderer::CUBE, GRAY);
    wall1->AddComponent<BoxCollider>(Vector3{ 1, 4, 20 }, Vector3{ 0, 0, 0 }, false);
    sceneRoot->AddChild(wall1);

    auto wall2 = std::make_shared<GameObject>("Wall2");
    wall2->GetTransform()->SetPosition({ -10, 2, 0 });
    wall2->GetTransform()->SetScale({ 1, 4, 20 });
    wall2->AddComponent<MeshRenderer>(MeshRenderer::CUBE, GRAY);
    wall2->AddComponent<BoxCollider>(Vector3{ 1, 4, 20 }, Vector3{ 0, 0, 0 }, false);
    sceneRoot->AddChild(wall2);

    // -------------------
    // 3D Model GameObject
    // -------------------
    auto modelGO = std::make_shared<GameObject>("ImportedModel");

    // Position it somewhere visible
    modelGO->GetTransform()->SetPosition({ 0.0f, 0.0f, -3.0f });
    modelGO->GetTransform()->SetScale({ 1.0f, 1.0f, 1.0f });

    // 1) MeshFilter: load the model file.
    //    Adjust the path + extension to your real asset.
    //    Raylib will try to load any referenced materials/textures automatically.
    modelGO->AddComponent<MeshFilter>("resources/models/hgrunt.obj");
    // or: "resources/models/crate.obj"

    // 2) MeshRenderer in CUSTOM mode: will use MeshFilter's Model.
    modelGO->AddComponent<MeshRenderer>(MeshRenderer::CUSTOM, WHITE);

    // (Optional) if you want the model to block the player:
    modelGO->AddComponent<BoxCollider>(
        Vector3{ 0.7f, 2.0f, 0.7f }, 
        Vector3{ 0, 1.0f, 0 }, 
        true
    );

    sceneRoot->AddChild(modelGO);

    // ------------------------------
    // Directional Light + Shadow Map
    // ------------------------------
    auto sun = std::make_shared<GameObject>("Directional Light");
    LightComponent* light = sun->AddComponent<LightComponent>(pipeline.GetLightingShader());
    light->SetDirection({ -0.3f, -1.0f, -0.2f });
    light->SetColor(WHITE, 1.0f);
    light->SetAmbientColor({ 0.2f, 0.2f, 0.25f });
    light->SetAmbientIntensity(1.5f);

    // Same here: GetShadowShader() returns Shader*
    ShadowMap* sm = sun->AddComponent<ShadowMap>(pipeline.GetShadowShader(), 2048);
    sceneRoot->AddChild(sun);

    sunLight  = light;
    shadowMap = sm;
}

void DemoScene3D::Start()
{
    if (sceneRoot)
        sceneRoot->Start();

    // Tell the pipeline which scene + camera/light/shadow to render.
    // This is the same wiring you had in main/SceneFactory before.
    pipeline.SetScene(sceneRoot, player, camera, sunLight, shadowMap);
}

void DemoScene3D::Update(float deltaTime)
{
    if (!sceneRoot)
        return;

    sceneRoot->Update(deltaTime);
}

void DemoScene3D::LateUpdate(float deltaTime)
{
    if (!sceneRoot)
        return;

    sceneRoot->LateUpdate(deltaTime);
}

void DemoScene3D::Draw(RenderPipeline& pipelineRef)
{
    // For now, pipelineRef is the same as `pipeline`, but we use the parameter
    // to match the Scene::Draw interface. All rendering (including shadow pass)
    // is handled inside RenderPipeline::Tick().
    pipelineRef.Tick();
}
