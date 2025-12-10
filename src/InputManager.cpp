#include "InputManager.h"
#include <cmath>

// Constructor: initialize all cached values to zero.
InputManager::InputManager()
    : moveForward(0.0f)
    , moveRight(0.0f)
    , mouseDelta{ 0.0f, 0.0f }
{
}

// Update: poll keyboard and mouse, normalize movement vector.
void InputManager::Update()
{
    // Reset axes each frame before reading new input
    moveForward = 0.0f;
    moveRight   = 0.0f;

    // Forward/back (W/S or Up/Down arrows)
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        moveForward += 1.0f;

    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        moveForward -= 1.0f;

    // Right/left (D/A or Right/Left arrows)
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        moveRight += 1.0f;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        moveRight -= 1.0f;

    // Normalize diagonal movement so diagonals are not faster
    const float magnitude = std::sqrt(moveForward * moveForward +
                                      moveRight   * moveRight);

    if (magnitude > 1.0f)
    {
        moveForward /= magnitude;
        moveRight   /= magnitude;
    }

    // Read mouse delta since last frame
    mouseDelta = GetMouseDelta();
}

// GetMoveForward: returns cached forward/backward axis.
float InputManager::GetMoveForward() const
{
    return moveForward;
}

// GetMoveRight: returns cached right/left axis.
float InputManager::GetMoveRight() const
{
    return moveRight;
}

// GetMouseDelta: returns cached mouse delta for camera look.
Vector2 InputManager::GetMouseDelta() const
{
    return mouseDelta;
}
