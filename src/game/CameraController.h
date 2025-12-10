#pragma once

#include "Component.h"

class Transform3D;

// CameraController:
// - Reads mouse input each frame.
// - Maintains yaw (Y) and pitch (X) in degrees.
// - Writes the result into the attached Transform3D.
// - Does NOT own or feed a Camera3D directly; it only rotates the transform.
class CameraController : public Component
{
private:
    Transform3D* transform = nullptr;   // Cached pointer to this GameObject's Transform3D

    float yawDegrees   = 0.0f;          // Rotation around Y (left/right)
    float pitchDegrees = 0.0f;          // Rotation around X (up/down)
    float mouseSensitivity = 0.15f;     // Degrees per pixel of mouse movement

public:
    // Construct a CameraController with the given mouse sensitivity.
    explicit CameraController(float sensitivity = 0.15f)
        : mouseSensitivity(sensitivity)
    {
    }

    // Cache the Transform3D and initialize yaw/pitch from its current rotation.
    void Start() override;

    // Read mouse input, update yaw/pitch, and write back to Transform3D.
    void Update(float deltaTime) override;
};
