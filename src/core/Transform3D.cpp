#include "Transform3D.h"
#include "raymath.h"

Vector3 Transform3D::Forward() const
{
    // Convention: rotation.x = pitch, rotation.y = yaw, degrees
    float pitch = rotation.x * DEG2RAD;
    float yaw   = rotation.y * DEG2RAD;

    Vector3 f;
    f.x = cosf(pitch) * sinf(yaw);
    f.y = sinf(pitch);
    f.z = cosf(pitch) * cosf(yaw);

    return Vector3Normalize(f);
}

Vector3 Transform3D::Right() const
{
    // Right = cross(forward, worldUp)
    const Vector3 worldUp { 0.0f, 1.0f, 0.0f };
    Vector3 f = Forward();
    Vector3 r = Vector3CrossProduct(f, worldUp);

    // If forward is almost parallel to up, this can go near-zero;
    // in practice for FPS-style this is fine.
    if (Vector3Length(r) < 0.0001f)
        return { 1.0f, 0.0f, 0.0f }; // fallback

    return Vector3Normalize(r);
}

Vector3 Transform3D::Up() const
{
    Vector3 f = Forward();
    Vector3 r = Right();
    Vector3 u = Vector3CrossProduct(r, f);
    return Vector3Normalize(u);
}

void Transform3D::Translate(Vector3 offset)
{
    position = Vector3Add(position, offset);
}

void Transform3D::Rotate(Vector3 eulerDelta)
{
    rotation.x += eulerDelta.x;
    rotation.y += eulerDelta.y;
    rotation.z += eulerDelta.z;
}
