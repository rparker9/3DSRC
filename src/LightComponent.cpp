#include "LightComponent.h"
#include "raymath.h"

/// <summary>
/// Helper: converts raylib Color (0..255) to Vector3 (0..1).
/// </summary>
static Vector3 ColorToVec3(Color c)
{
    return {
        c.r / 255.0f,
        c.g / 255.0f,
        c.b / 255.0f
    };
}

LightComponent::LightComponent(Shader* lightingShader)
    : shader(lightingShader)
{
    if (!shader) return;

    // Cache uniform locations once
    locLightDir     = GetShaderLocation(*shader, "u_lightDir");
    locLightColor   = GetShaderLocation(*shader, "u_lightColor");
    locAmbientColor = GetShaderLocation(*shader, "u_ambientColor");
}

void LightComponent::SetDirection(Vector3 dir)
{
    // Normalize to avoid weird lighting from arbitrary length vectors
    direction = Vector3Normalize(dir);
}

void LightComponent::SetColor(Color color, float newIntensity)
{
    diffuseColor = ColorToVec3(color);
    intensity    = newIntensity;
}

void LightComponent::SetAmbient(Vector3 ambient)
{
    ambientColor = ambient;
}

void LightComponent::Update(float /*deltaTime*/)
{
    if (!shader) return;

    // Light direction: shader expects a direction in world space.
    // We pass it as-is (normalized in SetDirection).
    if (locLightDir >= 0)
    {
        SetShaderValue(*shader, locLightDir, &direction.x, SHADER_UNIFORM_VEC3);
    }

    // Diffuse color * intensity
    if (locLightColor >= 0)
    {
        Vector3 col = {
            diffuseColor.x * intensity,
            diffuseColor.y * intensity,
            diffuseColor.z * intensity
        };
        SetShaderValue(*shader, locLightColor, &col.x, SHADER_UNIFORM_VEC3);
    }

    // Ambient color
    if (locAmbientColor >= 0)
    {
        SetShaderValue(*shader, locAmbientColor, &ambientColor.x, SHADER_UNIFORM_VEC3);
    }
}
