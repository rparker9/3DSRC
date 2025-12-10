#include "CameraController.h"
#include <cmath>

// Constructor: store references and set default sensitivity.
CameraController::CameraController(Transform3D& transform, InputManager& input)
    : transform(transform)
    , input(input)
    , mouseSensitivity(0.1f)
{
    // Initialize camera struct to safe defaults.
    camera.position   = { 0.0f, 0.0f, 0.0f };
    camera.target     = { 0.0f, 0.0f, 0.0f };
    camera.up         = { 0.0f, 1.0f, 0.0f };
    camera.fovy       = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}


// InitializeCamera: sets base camera properties such as FOV and starting pose.
void CameraController::InitializeCamera(float fovDegrees)
{
    // Use current transform for initial camera position.
    camera.position = transform.position;

    // Target will be computed each frame in Update; set something valid for now.
    camera.target = { 0.0f, 0.0f, 0.0f };

    // World up vector.
    camera.up = { 0.0f, 1.0f, 0.0f };

    // Use specified FOV and perspective projection.
    camera.fovy       = fovDegrees;
    camera.projection = CAMERA_PERSPECTIVE;
}

// Update: applies mouse look to transform and rebuilds camera position/target.
void CameraController::Update(float /*deltaTime*/)
{
    // Read mouse movement since last frame.
    const Vector2 mouseDelta = input.GetMouseDelta();

    // Adjust yaw (Y) and pitch (X) by mouse movement scaled by sensitivity.
    transform.rotationEuler.y += mouseDelta.x * mouseSensitivity;
    transform.rotationEuler.x += mouseDelta.y * mouseSensitivity;

    // Clamp pitch to avoid flipping the camera upside down.
    if (transform.rotationEuler.x > 89.0f)
        transform.rotationEuler.x = 89.0f;

    if (transform.rotationEuler.x < -89.0f)
        transform.rotationEuler.x = -89.0f;

    // Convert pitch and yaw from degrees to radians.
    const float yawRad   = transform.rotationEuler.y * DEG2RAD;
    const float pitchRad = transform.rotationEuler.x * DEG2RAD;

    // Compute forward direction from yaw and pitch.
    // Convention: yaw = 0 faces -Z; yaw 90 = +X.
    Vector3 forward{};
    forward.x = std::cosf(pitchRad) * std::sinf(yawRad);
    forward.y = std::sinf(pitchRad);
    forward.z = -std::cosf(pitchRad) * std::cosf(yawRad);

    // Update camera position directly from transform.
    camera.position = transform.position;

    // Camera target is one unit in front of the camera along forward vector.
    camera.target = Vector3Add(camera.position, forward);

    // Use world up vector (simple first-person camera).
    camera.up = { 0.0f, 1.0f, 0.0f };
}

// GetCamera: returns internal Camera3D struct for use in BeginMode3D.
const Camera3D& CameraController::GetCamera() const
{
    return camera;
}

// SetMouseSensitivity: set how strongly mouse movement affects rotation.
void CameraController::SetMouseSensitivity(float sensitivity)
{
    mouseSensitivity = sensitivity;
}
