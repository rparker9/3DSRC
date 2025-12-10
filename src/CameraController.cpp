#include "CameraController.h"
#include "GameObject.h"
#include "Transform3D.h"

CameraController::CameraController(float sensitivity) 
    : mouseSensitivity(sensitivity) {}

void CameraController::Update(float deltaTime) {
    Transform3D* transform = gameObject->GetTransform();
    Vector2 mousePos = GetMousePosition();
    
    if (firstMouse) {
        lastMousePos = mousePos;
        firstMouse = false;
    }
    
    Vector2 mouseDelta = {
    mousePos.x - lastMousePos.x,
    mousePos.y - lastMousePos.y
    };
    lastMousePos = mousePos;

    // Get current Euler rotation (x = pitch, y = yaw)
    Vector3 rotation = transform->GetRotation();

    // Make controls feel “natural”:
    // - moving mouse right turns camera right
    // - moving mouse up looks up
    rotation.y -= mouseDelta.x * mouseSensitivity; // invert yaw direction
    rotation.x += mouseDelta.y * mouseSensitivity; // invert pitch direction

    transform->SetRotation(rotation);
}

void CameraController::SetSensitivity(float sens) { 
    mouseSensitivity = sens; 
}