#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Component.h"
#include "Transform3D.h"

class GameObject {
private:
    std::string name;
    std::vector<std::shared_ptr<Component>> components;
    std::vector<std::shared_ptr<GameObject>> children;
    GameObject* parent = nullptr;
    bool active = true;
    
public:
    explicit GameObject(const std::string& name = "GameObject");
    
    const std::string& GetName() const;
    void SetName(const std::string& n);
    
    bool IsActive() const;
    void SetActive(bool value);
    
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        auto component = std::make_shared<T>(std::forward<Args>(args)...);
        component->SetGameObject(this);
        components.push_back(component);
        return component.get();
    }
    
    template<typename T>
    T* GetComponent() {
        for (auto& comp : components) {
            if (auto ptr = dynamic_cast<T*>(comp.get())) {
                return ptr;
            }
        }
        return nullptr;
    }
    
    Transform3D* GetTransform();
    
    void AddChild(std::shared_ptr<GameObject> child);
    const std::vector<std::shared_ptr<GameObject>>& GetChildren() const;
    
    void Start();
    void Update(float deltaTime);
    void LateUpdate(float deltaTime);
    void Draw();

    void DrawShadow();
};