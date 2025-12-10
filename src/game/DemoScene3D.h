#pragma once

#include <memory>

#include "Scene.h"

class GameObject;
class CameraComponent;
class LightComponent;
class ShadowMap;
class RenderPipeline;

/// A simple 3D demo scene:
/// - Player with camera + controller
/// - Ground, cubes, walls
/// - Directional sun light + shadow map
class DemoScene3D : public Scene
{
public:
    /// Construct the scene bound to a given RenderPipeline.
    /// The pipeline owns the shaders; the scene uses them to build objects.
    explicit DemoScene3D(RenderPipeline& pipeline);

    void Start() override;
    void Update(float deltaTime) override;
    void LateUpdate(float deltaTime) override;
    void Draw(RenderPipeline& pipeline) override;

private:
    RenderPipeline&           pipeline;     // reference, not owned
    std::shared_ptr<GameObject> sceneRoot;  // root GameObject

    GameObject*      player        = nullptr;
    CameraComponent* camera        = nullptr;
    LightComponent*  sunLight      = nullptr;
    ShadowMap*       shadowMap     = nullptr;

    // Internal helper to build the GameObject hierarchy and cache pointers.
    void BuildSceneGraph();
};
