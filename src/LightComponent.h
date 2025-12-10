#pragma once

#include "Component.h"
#include "raylib.h"

/// <summary>
/// LightComponent represents a single directional light for the scene.
/// It uploads its data (direction, color, ambient) into a shared lighting shader.
/// Intended for a "single sun" style setup.
/// </summary>
class LightComponent : public Component
{
private:
    Shader* shader = nullptr;

    // Direction the light is pointing *towards* in world space (normalized).
    Vector3 direction = { -0.3f, -1.0f, -0.2f };

    // Diffuse color (0..1) and intensity multiplier
    Vector3 diffuseColor = { 1.0f, 1.0f, 1.0f };
    float   intensity    = 1.0f;

    // Ambient term (0..1) added everywhere
    Vector3 ambientColor = { 0.15f, 0.15f, 0.2f };

    // Cached uniform locations
    int locLightDir     = -1;
    int locLightColor   = -1;
    int locAmbientColor = -1;

public:
    /// <summary>
    /// Constructs a LightComponent bound to a given lighting shader.
    /// The shader must remain valid for the lifetime of the light.
    /// </summary>
    LightComponent(Shader* lightingShader);

    /// <summary>
    /// Sets the light direction in world space (will be normalized).
    /// </summary>
    void SetDirection(Vector3 dir);

    /// <summary>
    /// Returns the current world-space light direction.
    /// </summary>
    Vector3 GetDirection() const { return direction; }

    /// <summary>
    /// Sets the light diffuse color (0..255) and intensity multiplier.
    /// </summary>
    void SetColor(Color color, float newIntensity);

    /// <summary>
    /// Sets the ambient color contribution (0..1).
    /// </summary>
    void SetAmbient(Vector3 ambient);

    /// <summary>
    /// Each frame, uploads light uniforms into the shader.
    /// </summary>
    void Update(float deltaTime) override;
};
