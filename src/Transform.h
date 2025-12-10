#pragma once
#include <raylib.h>

// Transform
//  - Unity-like transform storing world-space position and Euler rotation.
//  - rotationEuler uses degrees: x = pitch, y = yaw, z = roll.
struct Transform3D
{
    Vector3 position;       // World-space position
    Vector3 rotationEuler;  // Pitch (x), Yaw (y), Roll (z), in degrees

    Transform3D();
};
