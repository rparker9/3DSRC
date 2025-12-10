#pragma once
#include <raylib.h>

// InputManager
//  - Polls keyboard and mouse each frame.
//  - Provides high-level movement axes and mouse delta.
class InputManager
{
public:
    InputManager();

    // Reads input state from raylib. Call once per frame.
    void Update();

    // +1 = forward, -1 = backward
    float GetMoveForward() const;

    // +1 = right, -1 = left
    float GetMoveRight() const;

    // Mouse movement since last frame (pixels)
    Vector2 GetMouseDelta() const;

private:
    float   moveForward;    // Cached forward/backward axis
    float   moveRight;      // Cached right/left axis
    Vector2 mouseDelta;     // Cached mouse delta
};
