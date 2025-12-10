#include "LightComponent.h"
#include "Transform3D.h"
#include "GameObject.h"
#include "raymath.h"

LightComponent::LightComponent(Shader* lightingShader)
    : shader(lightingShader)
{
    if (!shader)
        return;

    // Cache uniform locations – adjust names to match your GLSL.
    locDirection = GetShaderLocation(*shader, "u_lightDir");
    locColor     = GetShaderLocation(*shader, "u_lightColor");
    locAmbient   = GetShaderLocation(*shader, "u_ambientColor");
}

void LightComponent::SetUseTransformDirection(bool enabled)
{
    useTransformDirection = enabled;
}

void LightComponent::SetDirection(const Vector3& dir)
{
    // Normalize once and store.
    explicitDirection = Vector3Normalize(dir);
}

Vector3 LightComponent::GetDirection() const
{
    if (useTransformDirection)
    {
        // Use owner's Transform forward (Unity-style directional light).
        Transform3D* t = GetGameObject()->GetTransform();
        if (t)
            return t->GetForward();
    }

    // Fallback: explicit direction value.
    return explicitDirection;
}

void LightComponent::SetColor(Color color, float newIntensity)
{
    diffuseColor = color;
    intensity    = newIntensity;
}

void LightComponent::SetAmbientColor(Vector3 color)
{
    ambientColor = color;
}

void LightComponent::SetAmbientIntensity(float inten)
{
    ambientIntensity = inten;
}

void LightComponent::Start()
{
    // Nothing special required for now; uniforms pushed each Update.
}

void LightComponent::Update(float /*deltaTime*/)
{
    if (!shader)
        return;

    // Upload direction
    if (locDirection >= 0)
    {
        Vector3 dir = GetDirection();
        SetShaderValue(*shader, locDirection, &dir.x, SHADER_UNIFORM_VEC3);
    }

    // Upload color * intensity
    if (locColor >= 0)
    {
        // Base color 0..1
        Vector3 colorVec = {
            diffuseColor.r / 255.0f,
            diffuseColor.g / 255.0f,
            diffuseColor.b / 255.0f
        };

        // Apply intensity scalar here
        colorVec = Vector3Scale(colorVec, intensity);

        SetShaderValue(*shader, locColor, &colorVec.x, SHADER_UNIFORM_VEC3);
    }

    if (locAmbient >= 0)
    {
        Vector3 ambientScaled = Vector3Scale(ambientColor, ambientIntensity);
        SetShaderValue(*shader, locAmbient, &ambientScaled.x, SHADER_UNIFORM_VEC3);
    }
}
