#pragma once

#include <memory>
#include <type_traits>
#include <utility>

#include "Scene.h"

class RenderPipeline;

/// Manages the currently active Scene.
/// Owns a single Scene instance and forwards Start / Update / Draw calls.
class SceneManager
{
public:
    // Sets the current scene to an already-created Scene instance.
    // Ownership is transferred into the manager.
    void SetScene(std::unique_ptr<Scene> newScene)
    {
        currentScene = std::move(newScene);
        if (currentScene)
            currentScene->Start();
    }

    // Convenience template to construct a scene of type T in-place.
    // Usage: sceneManager.MakeScene<DemoScene3D>(constructor args...);
    template <typename T, typename... Args>
    void MakeScene(Args&&... args)
    {
        static_assert(std::is_base_of<Scene, T>::value, "T must derive from Scene");
        currentScene = std::make_unique<T>(std::forward<Args>(args)...);
        currentScene->Start();
    }

    // Forwards per-frame update to the active scene (if any).
    void Update(float deltaTime)
    {
        if (currentScene)
            currentScene->Update(deltaTime);
    }

    // Forwards per-frame late update to the active scene (if any).
    void LateUpdate(float deltaTime)
    {
        if (currentScene)
            currentScene->LateUpdate(deltaTime);
    }

    // Forwards per-frame draw to the active scene (if any).
    void Draw(RenderPipeline& pipeline)
    {
        if (currentScene)
            currentScene->Draw(pipeline);
    }

private:
    std::unique_ptr<Scene> currentScene;
};
