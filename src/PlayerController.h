#pragma once

#include "Component.h"
#include "raylib.h"

class GameObject;
class BoxCollider;
class Transform3D;

// PlayerController handles first-person style movement:
// - WASD horizontal movement
// - Gravity and jumping
// - Sliding against walls using axis-separated collision
// - Ground detection using a downward raycast against BoxColliders
// - Velocity-based ground and air control (accel/decel, reduced air control)
class PlayerController : public Component
{
private:
    // Maximum horizontal speed on ground and in air (units per second)
    float moveSpeed = 6.0f;

    // Constant downward acceleration (units per second^2, negative = down)
    float gravity = -25.0f;

    // Initial upward velocity when jumping (units per second)
    float jumpSpeed = 10.0f;

    // Full 3D velocity of the player:
    // - X/Z = horizontal velocity
    // - Y   = vertical velocity
    Vector3 velocity = { 0.0f, 0.0f, 0.0f };

    // Ground movement tuning
    float groundAccel = 40.0f;   // how fast we accelerate to max speed
    float groundDecel = 80.0f;   // how fast we slow down when no input

    // Air movement tuning
    float airAccel   = 10.0f;    // how fast we accelerate to max speed in air (lower = harder to control, range 0..groundAccel)
    float airControl = 0.6f;     // multiplier for how strong air control is (0..1+)

    // True if we consider the player to be standing on some surface
    bool isGrounded = false;

    // Root of the scene graph used to search for colliders
    GameObject* sceneRoot = nullptr;

    // === Internal helpers ===

    // Recursively checks if playerCol intersects any BoxCollider in the scene
    // (excluding the player's own GameObject). Sets hasCollision to true if hit.
    // When ignoreFloorLikeContacts is true, collisions where the player's bottom
    // is roughly aligned with another collider's top (standing on it) are ignored.
    void CheckCollisionRecursive(GameObject* obj,
                                 const BoxCollider* playerCol,
                                 bool& hasCollision,
                                 bool ignoreFloorLikeContacts);

    // Moves the player with axis-separated collision:
    // 1) Apply X move, resolve collision
    // 2) Apply Z move, resolve collision
    // 3) Apply Y move, resolve collision (floor/ceiling)
    void MoveWithCollision(Transform3D* transform,
                           BoxCollider* playerCol,
                           Vector3 movement);

    // Casts a downward ray from the player's feet to find the ground within maxDistance.
    // Returns true if something was hit and writes the hit point into outGroundPoint.
    bool RaycastGround(Vector3& outGroundPoint, float maxDistance);

    // Internal recursive helper used by RaycastGround to traverse the scene graph.
    // Tracks the closest hit against any BoxCollider in the scene.
    void RaycastGroundRecursive(GameObject* obj,
                                const Ray& ray,
                                float& bestDist,
                                Vector3& bestPoint,
                                bool& foundHit);

public:
    // Constructs a controller with a given move speed and scene root.
    PlayerController(float speed = 6.0f, GameObject* scene = nullptr);

    // Sets the scene root used for all collision / raycast queries.
    void SetSceneRoot(GameObject* scene);

    // Per-frame update: handles input, gravity, jumping, velocity,
    // collision, and ground raycast.
    void Update(float deltaTime) override;
};
