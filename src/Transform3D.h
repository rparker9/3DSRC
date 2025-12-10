#pragma once

#include "raylib.h"
#include "Component.h"

class Transform3D : public Component {
private:
    Vector3 position = {0, 0, 0};
    Vector3 rotation = {0, 0, 0};
    Vector3 scale = {1, 1, 1};
    
public:
    Vector3 GetPosition() const;
    void SetPosition(Vector3 pos);
    
    Vector3 GetRotation() const;
    void SetRotation(Vector3 rot);
    
    Vector3 GetScale() const;
    void SetScale(Vector3 scl);
    
    Vector3 Forward() const;
    Vector3 Right() const;
    Vector3 Up() const;
    
    void Translate(Vector3 offset);
    void Rotate(Vector3 eulerAngles);
};
