#include "CameraComponent.h"
#include "GameObject.h"
#include "Transform3D.h"
#include "raymath.h"

CameraComponent::CameraComponent() {
    camera.projection = CAMERA_PERSPECTIVE;
    camera.fovy = fov;
    camera.up = {0, 1, 0};
}

void CameraComponent::Update(float deltaTime) {
    Transform3D* transform = gameObject->GetTransform();
    camera.position = transform->GetPosition();
    camera.target = Vector3Add(camera.position, transform->Forward());
    camera.up = transform->Up();
}

void CameraComponent::BeginMode() {
    BeginMode3D(camera);
}

void CameraComponent::EndMode() {
    EndMode3D();
}

float CameraComponent::GetFOV() const { return fov; }
void CameraComponent::SetFOV(float f) { fov = f; camera.fovy = f; }