#pragma once
#include "Transform.h"
#include "InputManager.h"

// PlayerController
//  - Moves a Transform in the XZ plane based on InputManager axes.
//  - Movement is relative to the Transform's yaw (rotationEuler.y).
class PlayerController
{
public:
    // Construct controller with references to a Transform and InputManager.
    PlayerController(Transform3D& transform, InputManager& input);

    // Updates player position using current input and deltaTime.
    void Update(float deltaTime);

    // Sets movement speed in units per second.
    void SetMoveSpeed(float speed);

private:
    Transform&   transform;    // Player transform to modify
    InputManager& input;       // Shared input manager
    float        moveSpeed;    // Units per second
};
