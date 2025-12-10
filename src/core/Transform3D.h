#pragma once

#include "Component.h"
#include "raylib.h"

class Transform3D : public Component
{
private:
    Vector3 position { 0.0f, 0.0f, 0.0f };
    Vector3 rotation { 0.0f, 0.0f, 0.0f }; // pitch (x), yaw (y), roll (z) in degrees
    Vector3 scale    { 1.0f, 1.0f, 1.0f };

public:
    Vector3 GetPosition() const { return position; }
    void SetPosition(Vector3 p) { position = p; }

    Vector3 GetRotation() const { return rotation; }
    void SetRotation(Vector3 r) { rotation = r; }

    Vector3 GetScale() const { return scale; }
    void SetScale(Vector3 s) { scale = s; }

    // Direction vectors in world space
    Vector3 Forward() const;
    Vector3 Right() const;
    Vector3 Up() const;

    // Convenience alias used for lights/camera
    Vector3 GetForward() const { return Forward(); }

    void Translate(Vector3 offset);
    void Rotate(Vector3 eulerDelta);
};
