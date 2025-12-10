    #pragma once
#include <raylib.h>
#include "Transform.h"
#include "InputManager.h"

// CameraController
//  - First-person camera attached to a Transform.
//  - Uses mouse delta from InputManager to modify yaw/pitch.
class CameraController
{
public:
    // Construct with references to the camera/player Transform and InputManager.
    CameraController(Transform3D& transform, InputManager& input);

    // Initializes internal Camera3D parameters.
    void InitializeCamera(float fovDegrees);

    // Updates Transform rotation from mouse input and rebuilds Camera3D.
    void Update(float deltaTime);

    // Returns the current raylib camera for rendering.
    const Camera3D& GetCamera() const;

    // Sets mouse sensitivity (degrees of rotation per pixel).
    void SetMouseSensitivity(float sensitivity);

private:
    Transform3D&   transform;        // Shared transform (position + rotation)
    InputManager& input;           // Shared input manager
    Camera3D     camera;           // Internal raylib camera
    float        mouseSensitivity; // Degrees per pixel of mouse movement
};
