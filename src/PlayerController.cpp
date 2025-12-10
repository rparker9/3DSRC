#include "PlayerController.h"
#include <raylib.h>
#include <cmath>

// Constructor: store references and set default move speed.
PlayerController::PlayerController(Transform3D& transform, InputManager& input)
    : transform(transform)
    , input(input)
    , moveSpeed(5.0f)
{
}

// Update: move the player in the XZ plane based on input and yaw.
void PlayerController::Update(float deltaTime)
{
    // Read cached movement inputs.
    const float forwardInput = input.GetMoveForward();
    const float rightInput   = input.GetMoveRight();

    // Early out if no movement requested this frame.
    if (forwardInput == 0.0f && rightInput == 0.0f)
        return;

    // Convert yaw in degrees to radians for trigonometry.
    const float yawRad = transform.rotationEuler.y * DEG2RAD;

    // Compute forward and right directions in XZ plane.
    // Convention: yaw = 0 faces -Z; yaw 90 = +X.
    const Vector3 forwardDir = { std::sinf(yawRad), 0.0f, -std::cosf(yawRad) };
    const Vector3 rightDir   = { std::cosf(yawRad), 0.0f,  std::sinf(yawRad) };

    // Combine the input axes into a world-space velocity vector.
    Vector3 velocity{};
    velocity.x = (forwardDir.x * forwardInput + rightDir.x * rightInput) * moveSpeed;
    velocity.y = 0.0f; // No vertical movement in this controller.
    velocity.z = (forwardDir.z * forwardInput + rightDir.z * rightInput) * moveSpeed;

    // Apply velocity scaled by frame time to the transform position.
    transform.position.x += velocity.x * deltaTime;
    transform.position.y += velocity.y * deltaTime;
    transform.position.z += velocity.z * deltaTime;
}

// SetMoveSpeed: adjusts how fast the player moves per second.
void PlayerController::SetMoveSpeed(float speed)
{
    moveSpeed = speed;
}
