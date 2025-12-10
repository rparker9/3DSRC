#pragma once

#include "raylib.h"
#include "Component.h"

// CameraComponent:
// - Wraps a raylib Camera3D.
// - Builds the camera each frame from the GameObject's Transform3D.
// - Applies a local eye offset (e.g. eye height) before feeding raylib.
class CameraComponent : public Component
{
private:
    float    fov    = 60.0f;       // Field of view in degrees
    Camera3D camera = { 0 };       // The camera raylib will use

    // Local offset from the GameObject's origin to the camera eye position.
    // Example: {0, 0.8, 0} to place the camera near the top of a 2-unit capsule.
    Vector3  localOffset { 0.0f, 0.8f, 0.0f };

public:
    CameraComponent();

    // Build the Camera3D from the Transform3D each frame.
    void Update(float deltaTime) override;


    // Call before drawing 3D geometry.
    // </summary>
    void BeginMode();

    // Call after drawing 3D geometry.
    void EndMode();

    float GetFOV() const;
    void  SetFOV(float f);

    // Set the local offset from the GameObject's origin to the eye position.
    void SetLocalOffset(const Vector3& offset) { localOffset = offset; }

    // Read-only access to the underlying Camera3D.
    const Camera3D& GetCamera() const { return camera; }
};
