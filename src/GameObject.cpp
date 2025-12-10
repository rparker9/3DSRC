#include "GameObject.h"

GameObject::GameObject(const std::string& name) : name(name) {
    AddComponent<Transform3D>();
}

const std::string& GameObject::GetName() const { return name; }
void GameObject::SetName(const std::string& n) { name = n; }

bool GameObject::IsActive() const { return active; }
void GameObject::SetActive(bool value) { active = value; }

Transform3D* GameObject::GetTransform() {
    return GetComponent<Transform3D>();
}

void GameObject::AddChild(std::shared_ptr<GameObject> child) {
    child->parent = this;
    children.push_back(child);
}

const std::vector<std::shared_ptr<GameObject>>& GameObject::GetChildren() const {
    return children;
}

void GameObject::Start() {
    if (!active) return;
    for (auto& comp : components) {
        if (comp->IsEnabled()) comp->Start();
    }
    for (auto& child : children) {
        child->Start();
    }
}

void GameObject::Update(float deltaTime) {
    if (!active) return;
    for (auto& comp : components) {
        if (comp->IsEnabled()) comp->Update(deltaTime);
    }
    for (auto& child : children) {
        child->Update(deltaTime);
    }
}

void GameObject::LateUpdate(float deltaTime) {
    if (!active) return;
    for (auto& comp : components) {
        if (comp->IsEnabled()) comp->LateUpdate(deltaTime);
    }
    for (auto& child : children) {
        child->LateUpdate(deltaTime);
    }
}

void GameObject::Draw() {
    if (!active) return;
    for (auto& comp : components) {
        if (comp->IsEnabled()) comp->Draw();
    }
    for (auto& child : children) {
        child->Draw();
    }
}

void GameObject::DrawShadow()
{
    if (!active) return;

    // 1) Components
    for (auto& comp : components)
    {
        if (comp->IsEnabled())
            comp->DrawShadow();
    }

    // 2) Children
    for (auto& child : children)
    {
        child->DrawShadow();
    }
}