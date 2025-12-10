// RenderPipeline.h
#pragma once

#include <memory>
#include "raylib.h"

// Forward declarations: we only need pointers/references here
class GameObject;
class CameraComponent;
class LightComponent;
class ShadowMap;

// Encapsulates rendering + shadow pass.
// Owns shaders and knows how to render a bound scene.
class RenderPipeline
{
public:
    RenderPipeline(int screenWidth, int screenHeight);

    // Load shaders and set up global render state.
    bool Initialize();

    // Expose shaders so scene creation can attach proper light/shadow components.
    Shader* GetLightingShader();
    Shader* GetShadowShader();

    // Bind a scene and its key components (player, camera, sun light, shadow map).
    void SetScene(const std::shared_ptr<GameObject>& scene,
                  GameObject* player,
                  CameraComponent* camera,
                  LightComponent* sunLight,
                  ShadowMap* shadowMap);

    // One frame: update scene, run shadow pass, render.
    void Tick();

    // Release GPU resources.
    void Shutdown();

private:
    int m_screenWidth  = 0;
    int m_screenHeight = 0;

    Shader m_lightingShader{};
    Shader m_shadowShader{};

    std::shared_ptr<GameObject> m_scene;
    GameObject*      m_player    = nullptr;
    CameraComponent* m_camera    = nullptr;
    LightComponent*  m_sunLight  = nullptr;
    ShadowMap*       m_shadowMap = nullptr;

    int  m_locViewPos    = -1;
    int  m_locLightSpace = -1;
    bool m_showShadowMap = true;
};
