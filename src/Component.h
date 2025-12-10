#pragma once

class GameObject;

class Component {
protected:
    GameObject* gameObject = nullptr;
    bool enabled = true;

public:
    virtual ~Component() = default;

    void SetGameObject(GameObject* go) { gameObject = go; }
    GameObject* GetGameObject() const { return gameObject; }

    bool IsEnabled() const { return enabled; }
    void SetEnabled(bool value) { enabled = value; }

    virtual void Start() {}
    virtual void Update(float deltaTime) {}
    virtual void LateUpdate(float deltaTime) {}
    virtual void Draw() {}

    // NEW: depth-only pass for shadow rendering
    virtual void DrawShadow() {}
};
