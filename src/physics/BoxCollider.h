#pragma once

#include "raylib.h"
#include "Component.h"

class BoxCollider : public Component {
private:
    Vector3 size = {1, 1, 1};
    Vector3 offset = {0, 0, 0};
    bool visible = true;
    
public:
    BoxCollider(Vector3 size = {1, 1, 1}, Vector3 offset = {0, 0, 0}, bool visible = true);
    
    Vector3 GetSize() const;
    void SetSize(Vector3 s);
    
    Vector3 GetOffset() const;
    void SetOffset(Vector3 o);
    
    BoundingBox GetBounds() const;
    bool CheckCollision(const BoxCollider* other) const;
    
    void Draw() override;
};