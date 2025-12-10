#include "CameraController.h"
#include "Transform3D.h"
#include "GameObject.h"
#include "raylib.h"

void CameraController::Start()
{
    // Try to get the Transform3D from this GameObject.
    transform = gameObject ? gameObject->GetTransform() : nullptr;
    if (!transform)
        return;

    // Initialize yaw/pitch from the existing rotation so there is no jump.
    // rotation.x = pitch, rotation.y = yaw (in degrees).
    const Vector3 rot = transform->GetRotation();
    pitchDegrees = rot.x;
    yawDegrees   = rot.y;

    // Optionally, center the mouse or lock it here if you want FPS-style capture.
    // DisableCursor(); // Example: raylib call, if you decide to lock the cursor.
}

void CameraController::Update(float /*deltaTime*/)
{
    // If we somehow lost the transform, try to reacquire once.
    if (!transform && gameObject)
        transform = gameObject->GetTransform();

    if (!transform)
        return;

    // 1) Read mouse movement since last frame.
    const Vector2 mouseDelta = GetMouseDelta();

    // 2) Update yaw/pitch in degrees.
    //    - Mouse X: horizontal movement → yaw (left/right).
    //    - Mouse Y: vertical movement   → pitch (up/down, inverted).
    yawDegrees   -= mouseDelta.x * mouseSensitivity;
    pitchDegrees -= mouseDelta.y * mouseSensitivity;

    // 3) Clamp pitch to avoid flipping over (FPS-style).
    if (pitchDegrees > 89.0f)  pitchDegrees = 89.0f;
    if (pitchDegrees < -89.0f) pitchDegrees = -89.0f;

    // 4) Write rotation back to the Transform3D.
    //    - rotation.x = pitch, rotation.y = yaw, rotation.z = roll (kept at 0 here).
    const Vector3 newRot { pitchDegrees, yawDegrees, 0.0f };
    transform->SetRotation(newRot);

    // NOTE:
    // Movement (WASD, etc.) belongs in a PlayerController or similar.
    // CameraController here is strictly responsible for orientation, like a Unity script.
}
