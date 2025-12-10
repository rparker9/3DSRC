#pragma once

#include "raylib.h"
#include "Component.h"

class CameraController : public Component {
private:
    float mouseSensitivity = 0.1f;
    Vector2 lastMousePos = {0, 0};
    bool firstMouse = true;
    
public:
    CameraController(float sensitivity = 0.1f);
    
    void Update(float deltaTime) override;
    void SetSensitivity(float sens);
};