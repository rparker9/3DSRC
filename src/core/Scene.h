#pragma once

class RenderPipeline;

/// Base class for a game scene (3D or otherwise).
/// Each scene owns its world (GameObjects, etc.) and knows how to
/// update its logic and render via the RenderPipeline.
class Scene
{
public:
    virtual ~Scene() = default;

    // Called once after the scene is created / swapped in.
    virtual void Start() {}

    // Called once per frame to update game logic.
    virtual void Update(float deltaTime) = 0;

    // Optional late update hook (order-sensitive stuff).
    virtual void LateUpdate(float deltaTime) {}

    // Called once per frame to render the scene.
    // RenderPipeline already encapsulates shadow pass + final pass.
    virtual void Draw(RenderPipeline& pipeline) = 0;
};
