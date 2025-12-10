#pragma once

#include "raylib.h"
#include "Component.h"

class CameraComponent : public Component {
private:
    float fov = 60.0f;
    Camera3D camera = {0};
    
public:
    CameraComponent();
    
    void Update(float deltaTime) override;
    void BeginMode();
    void EndMode();
    
    float GetFOV() const;
    void SetFOV(float f);
};