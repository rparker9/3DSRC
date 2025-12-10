#pragma once

#include "Component.h"
#include "raylib.h"

class Transform3D;

/// <summary>
/// LightComponent represents a single directional light for the scene.
/// It uploads its data (direction, color, ambient) into a shared lighting shader.
/// Intended for a "single sun" style setup.
/// </summary>
class LightComponent : public Component
{
private:
    // Lighting shader used for this light (shared global lighting shader).
    Shader* shader = nullptr;

    // If true, the light direction is derived from the owner's Transform forward.
    // If false, explicitDirection is used.
    bool useTransformDirection = false;

    // Fallback / explicit direction, normalized.
    Vector3 explicitDirection { -0.3f, -1.0f, -0.2f };

    // Diffuse color + intensity scale.
    Color  diffuseColor{ WHITE };
    float  intensity = 1.0f;

    // Ambient lighting term (0..1 per component).
    Vector3 ambientColor { 0.15f, 0.15f, 0.20f };
    float   ambientIntensity = 1.0f;

    // Cached uniform locations (optional but typical).
    int locDirection = -1;
    int locColor     = -1;
    int locAmbient   = -1;

public:
    /// <summary>
    /// Constructs a light component tied to the given lighting shader.
    /// </summary>
    explicit LightComponent(Shader* lightingShader);

    /// <summary>
    /// Enable or disable using the owner's Transform forward as the light direction.
    /// When enabled, editor / scripts just rotate the Transform.
    /// </summary>
    void SetUseTransformDirection(bool enabled);

    /// <summary>
    /// Sets an explicit world-space direction (normalized internally).
    /// Used when not using Transform-based direction.
    /// </summary>
    void SetDirection(const Vector3& dir);

    /// <summary>
    /// Gets the effective light direction in world space.
    /// If useTransformDirection is true, returns owner Transform forward.
    /// Otherwise returns the explicitDirection value.
    /// </summary>
    Vector3 GetDirection() const;

    /// <summary>
    /// Sets the light diffuse color (0..255) and intensity multiplier.
    /// </summary>
    void SetColor(Color color, float newIntensity);

    /// <summary>
    /// Sets the ambient color contribution (0..1).
    /// </summary>
    void SetAmbientColor(Vector3 color);

    // Sets the ambient intensity multiplier (1.0 = normal).
    void SetAmbientIntensity(float intensity);

    /// <summary>
    /// Called once when the component is added / scene started.
    /// </summary>
    void Start() override;

    /// <summary>
    /// Each frame, uploads light uniforms into the shader.
    /// </summary>
    void Update(float deltaTime) override;
};
