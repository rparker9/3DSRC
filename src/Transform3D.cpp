#include "Transform3D.h"
#include "raymath.h"

Vector3 Transform3D::GetPosition() const { return position; }
void Transform3D::SetPosition(Vector3 pos) { position = pos; }

Vector3 Transform3D::GetRotation() const { return rotation; }
void Transform3D::SetRotation(Vector3 rot) { rotation = rot; }

Vector3 Transform3D::GetScale() const { return scale; }
void Transform3D::SetScale(Vector3 scl) { scale = scl; }

Vector3 Transform3D::Forward() const {
    float yaw = rotation.y * DEG2RAD;
    float pitch = rotation.x * DEG2RAD;
    return Vector3Normalize({
        sinf(yaw) * cosf(pitch),
        -sinf(pitch),
        cosf(yaw) * cosf(pitch)
    });
}

Vector3 Transform3D::Right() const {
    float yaw = rotation.y * DEG2RAD;
    return Vector3Normalize({cosf(yaw), 0, -sinf(yaw)});
}

// Returns the local up direction (normalized).
// This uses a right-handed basis: up = forward x right,
// so +Y actually points "up" in world space.
Vector3 Transform3D::Up() const
{
    // Cross product order is critical here. Forward × Right yields a true "up".
    // If you do Right × Forward instead, the result points downward.
    Vector3 up = Vector3CrossProduct(Forward(), Right());

    // Make sure the result is normalized so the camera up vector is unit length.
    return Vector3Normalize(up);
}


void Transform3D::Translate(Vector3 offset) {
    position = Vector3Add(position, offset);
}

void Transform3D::Rotate(Vector3 eulerAngles) {
    rotation = Vector3Add(rotation, eulerAngles);
    if (rotation.x > 89.0f) rotation.x = 89.0f;
    if (rotation.x < -89.0f) rotation.x = -89.0f;
}