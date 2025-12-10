#include "CameraComponent.h"
#include "GameObject.h"
#include "Transform3D.h"
#include "raymath.h"

CameraComponent::CameraComponent()
{
    // Initialize the Camera3D with sane defaults.
    camera.position   = { 0.0f, 0.0f, 0.0f };
    camera.target     = { 0.0f, 0.0f, 1.0f };
    camera.up         = { 0.0f, 1.0f, 0.0f };
    camera.fovy       = fov;
    camera.projection = CAMERA_PERSPECTIVE;
}

void CameraComponent::Update(float /*deltaTime*/)
{
    // Build the camera state purely from the Transform3D + local offset.
    Transform3D* transform = gameObject ? gameObject->GetTransform() : nullptr;
    if (!transform)
        return;

    // World-space basis from the Transform3D.
    Vector3 basePos = transform->GetPosition();
    Vector3 forward = transform->Forward();
    Vector3 up      = transform->Up();

    // Apply local offset in world space:
    // base position is usually the capsule center; localOffset lifts the camera to eye height.
    Vector3 eyePos = Vector3Add(basePos, localOffset);

    camera.position   = eyePos;
    camera.target     = Vector3Add(eyePos, forward);
    camera.up         = up;
    camera.fovy       = fov;
    camera.projection = CAMERA_PERSPECTIVE;
}

void CameraComponent::BeginMode()
{
    BeginMode3D(camera);
}

void CameraComponent::EndMode()
{
    EndMode3D();
}

float CameraComponent::GetFOV() const
{
    return fov;
}

void CameraComponent::SetFOV(float f)
{
    fov        = f;
    camera.fovy = f;
}