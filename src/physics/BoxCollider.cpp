#include "BoxCollider.h"
#include "GameObject.h"
#include "Transform3D.h"
#include "raymath.h"

BoxCollider::BoxCollider(Vector3 size, Vector3 offset, bool visible) 
    : size(size), offset(offset), visible(visible) {}

Vector3 BoxCollider::GetSize() const { return size; }
void BoxCollider::SetSize(Vector3 s) { size = s; }

Vector3 BoxCollider::GetOffset() const { return offset; }
void BoxCollider::SetOffset(Vector3 o) { offset = o; }

BoundingBox BoxCollider::GetBounds() const {
    Transform3D* transform = gameObject->GetTransform();
    Vector3 pos = Vector3Add(transform->GetPosition(), offset);
    Vector3 halfSize = Vector3Scale(size, 0.5f);
    return {
        Vector3Subtract(pos, halfSize),
        Vector3Add(pos, halfSize)
    };
}

bool BoxCollider::CheckCollision(const BoxCollider* other) const {
    return CheckCollisionBoxes(GetBounds(), other->GetBounds());
}

void BoxCollider::Draw() {
    if (!visible) return;
    BoundingBox bounds = GetBounds();
    DrawBoundingBox(bounds, GREEN);
}