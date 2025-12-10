#include "PlayerController.h"
#include "GameObject.h"
#include "Transform3D.h"
#include "BoxCollider.h"
#include "raylib.h"
#include "raymath.h"

PlayerController::PlayerController(float speed, GameObject* scene)
    : moveSpeed(speed), sceneRoot(scene)
{
}

void PlayerController::SetSceneRoot(GameObject* scene)
{
    sceneRoot = scene;
}

void PlayerController::Update(float deltaTime)
{
    // Get the transform and player collider used for movement and collision
    Transform3D* transform = gameObject->GetTransform();
    BoxCollider* playerCol = gameObject->GetComponent<BoxCollider>();

    if (!transform)
        return;

    // -----------------------------------------------------------------
    // 1) Build desired horizontal input direction (XZ plane only)
    // -----------------------------------------------------------------
    // Get local forward/right from the transform
    Vector3 forward = transform->Forward();
    Vector3 right   = transform->Right();

    // We only want planar movement; remove any vertical tilt
    forward.y = 0.0f;
    right.y   = 0.0f;

    if (Vector3Length(forward) > 0.0001f)
        forward = Vector3Normalize(forward);
    if (Vector3Length(right) > 0.0001f)
        right = Vector3Normalize(right);

    // Accumulate WASD input into a single direction vector
    Vector3 inputDir = { 0.0f, 0.0f, 0.0f };

    if (IsKeyDown(KEY_W)) inputDir = Vector3Add(inputDir, forward);
    if (IsKeyDown(KEY_S)) inputDir = Vector3Subtract(inputDir, forward);
    if (IsKeyDown(KEY_A)) inputDir = Vector3Add(inputDir, right);
    if (IsKeyDown(KEY_D)) inputDir = Vector3Subtract(inputDir, right);

    bool hasInput = Vector3Length(inputDir) > 0.0f;
    if (hasInput)
        inputDir = Vector3Normalize(inputDir);

    // -----------------------------------------------------------------
    // 2) Jump + gravity (vertical velocity only)
    // -----------------------------------------------------------------
    if (isGrounded && IsKeyPressed(KEY_SPACE))
    {
        // Start a jump by giving an upward impulse
        velocity.y = jumpSpeed;
        isGrounded = false;
    }

    // Gravity always pulls downwards
    velocity.y += gravity * deltaTime;

    // -----------------------------------------------------------------
    // 3) Horizontal acceleration / deceleration (ground vs air)
    // -----------------------------------------------------------------
    // Extract the horizontal part of the velocity (ignore Y)
    Vector3 horizVel = { velocity.x, 0.0f, velocity.z };

    if (hasInput)
    {
        // We want to accelerate toward moveSpeed in the inputDir direction.
        float maxSpeed = moveSpeed;

        // Current speed along the desired direction
        float currentSpeedAlong = Vector3DotProduct(horizVel, inputDir);

        // How much more speed we can add along that direction
        float addSpeed = maxSpeed - currentSpeedAlong;

        if (addSpeed > 0.0f)
        {
            // Choose acceleration based on grounded vs air
            float accel = isGrounded ? groundAccel : (airAccel * airControl);

            // Amount of speed we can add this frame
            float accelSpeed = accel * deltaTime;
            if (accelSpeed > addSpeed)
                accelSpeed = addSpeed;

            // Apply acceleration along desired direction
            horizVel = Vector3Add(horizVel, Vector3Scale(inputDir, accelSpeed));
        }
    }
    else
    {
        // No input: apply ground friction so we don't stop instantly.
        if (isGrounded)
        {
            float speed = Vector3Length(horizVel);
            if (speed > 0.0f)
            {
                float decelAmount = groundDecel * deltaTime;
                speed -= decelAmount;
                if (speed < 0.0f)
                    speed = 0.0f;

                if (speed > 0.0f)
                {
                    Vector3 dir = Vector3Normalize(horizVel);
                    horizVel = Vector3Scale(dir, speed);
                }
                else
                {
                    horizVel = { 0.0f, 0.0f, 0.0f };
                }
            }
        }
        // In air with no input: leave horizVel as-is (inertia).
    }

    // Clamp horizontal speed to max
    float horizSpeed = Vector3Length(horizVel);
    if (horizSpeed > moveSpeed && horizSpeed > 0.0f)
    {
        Vector3 dir = Vector3Normalize(horizVel);
        horizVel = Vector3Scale(dir, moveSpeed);
    }

    // Write horizontal velocity back into the full velocity vector
    velocity.x = horizVel.x;
    velocity.z = horizVel.z;

    // -----------------------------------------------------------------
    // 4) Convert velocity to a movement delta and resolve collisions
    // -----------------------------------------------------------------
    Vector3 movement = {
        velocity.x * deltaTime,
        velocity.y * deltaTime,
        velocity.z * deltaTime
    };

    if (playerCol && sceneRoot)
    {
        // Axis-separated collision: X, then Z, then Y
        MoveWithCollision(transform, playerCol, movement);
    }
    else
    {
        // No collider or scene root: just move freely
        transform->Translate(movement);
    }

    // -----------------------------------------------------------------
    // 5) Ground raycast (for arbitrary floor geometry)
    // -----------------------------------------------------------------
    // Here we only use the raycast to:
    //   - detect if we are on/near the ground (set isGrounded)
    //   - fix small downward penetration by snapping UP out of the floor
    //
    // We DO NOT pull the player downward with this logic; landing height
    // is determined by the collision resolution in MoveWithCollision.
    isGrounded = false;

    if (playerCol && sceneRoot && velocity.y <= 0.0f)
    {
        Vector3 groundPoint;
        const float maxProbeDistance = 0.15f; // how far below feet we look

        if (RaycastGround(groundPoint, maxProbeDistance))
        {
            BoundingBox bounds = playerCol->GetBounds();
            float halfHeight = (bounds.max.y - bounds.min.y) * 0.5f;

            Vector3 pos = transform->GetPosition();
            float targetY = groundPoint.y + halfHeight;

            // Positive diff: we're slightly INSIDE the ground
            // Negative diff: we're slightly ABOVE the ground
            float diff = targetY - pos.y;

            const float groundEps      = 0.08f; // how far from target we still consider "on ground"
            const float maxPenetration = 0.20f; // max we will snap UP to fix penetration

            // Case 1: we're basically at the right height -> grounded, no Y adjustment.
            if (fabsf(diff) <= groundEps)
            {
                velocity.y = 0.0f;
                isGrounded = true;
            }
            // Case 2: we sank slightly into the ground -> snap UP to surface.
            else if (diff > 0.0f && diff <= maxPenetration)
            {
                pos.y = targetY;
                transform->SetPosition(pos);

                velocity.y = 0.0f;
                isGrounded = true;
            }
            // Else: we're too far above or deeply inside something; treat as airborne.
        }
    }
}

// Axis-separated movement so we can slide along walls
void PlayerController::MoveWithCollision(Transform3D* transform,
                                         BoxCollider* playerCol,
                                         Vector3 movement)
{
    if (!sceneRoot || !playerCol || !transform)
    {
        transform->Translate(movement);
        return;
    }

    Vector3 pos = transform->GetPosition();

    // Helper lambda: test a position and see if it collides.
    // When ignoreFloorLikeContacts is true, we allow "standing on top of"
    // surfaces without treating it as a blocking collision. This is used for
    // horizontal (X/Z) moves so floors/platforms don't lock movement.
    auto testPositionCollides = [&](const Vector3& testPos,
                                    bool ignoreFloorLikeContacts) -> bool
    {
        transform->SetPosition(testPos);
        bool collision = false;
        CheckCollisionRecursive(sceneRoot, playerCol, collision, ignoreFloorLikeContacts);
        return collision;
    };

    // -----------------------------------------------------------------
    // X axis
    // -----------------------------------------------------------------
    if (movement.x != 0.0f)
    {
        Vector3 testPos = pos;
        testPos.x += movement.x;

        // Ignore floor-like contacts so we can walk while standing on something
        if (!testPositionCollides(testPos, true))
        {
            // Free movement along X
            pos.x = testPos.x;
            transform->SetPosition(pos);
        }
        else
        {
            // Blocked along X: keep position and kill X velocity
            transform->SetPosition(pos);
            velocity.x = 0.0f;
        }
    }

    // -----------------------------------------------------------------
    // Z axis
    // -----------------------------------------------------------------
    if (movement.z != 0.0f)
    {
        Vector3 testPos = pos;
        testPos.z += movement.z;

        // Same idea as X: floor-like contacts are allowed
        if (!testPositionCollides(testPos, true))
        {
            // Free movement along Z
            pos.z = testPos.z;
            transform->SetPosition(pos);
        }
        else
        {
            // Blocked along Z: keep position and kill Z velocity
            transform->SetPosition(pos);
            velocity.z = 0.0f;
        }
    }

    // -----------------------------------------------------------------
    // Y axis (floor/ceiling)
    // -----------------------------------------------------------------
    if (movement.y != 0.0f)
    {
        Vector3 testPos = pos;
        testPos.y += movement.y;

        // For vertical moves we DO want floors/ceilings to block
        if (!testPositionCollides(testPos, false))
        {
            // Free vertical move (jumping/falling)
            pos.y = testPos.y;
            transform->SetPosition(pos);
        }
        else
        {
            // Hit floor or ceiling: stop vertical motion but do NOT undo XZ
            transform->SetPosition(pos);

            // On vertical collision, zero vertical velocity so we don't
            // keep accumulating gravity while "stuck".
            velocity.y = 0.0f;

            // isGrounded will be decided by the ground raycast.
        }
    }
}

// Standard recursive AABB overlap check (skips the player object).
// When ignoreFloorLikeContacts is true, collisions where the player's
// bottom is roughly aligned with the other collider's top (i.e., standing
// on it) are ignored. This lets us walk freely on top of colliders.
void PlayerController::CheckCollisionRecursive(GameObject* obj,
                                               const BoxCollider* playerCol,
                                               bool& hasCollision,
                                               bool ignoreFloorLikeContacts)
{
    if (!obj || hasCollision || !playerCol)
        return;

    // Skip the player's own GameObject
    if (obj == gameObject)
    {
        const auto& children = obj->GetChildren();
        for (const auto& child : children)
        {
            CheckCollisionRecursive(child.get(), playerCol, hasCollision, ignoreFloorLikeContacts);
        }
        return;
    }

    BoxCollider* otherCol = obj->GetComponent<BoxCollider>();
    if (otherCol)
    {
        // Compute bounds for both colliders at the current test position
        BoundingBox a = playerCol->GetBounds();
        BoundingBox b = otherCol->GetBounds();

        if (CheckCollisionBoxes(a, b))
        {
            if (ignoreFloorLikeContacts)
            {
                // Treat this as "floor contact" if the player is essentially
                // on top of this collider (bottom aligned with its top).
                const float floorEpsilon = 0.05f;

                float playerBottom = a.min.y;
                float otherTop     = b.max.y;

                // If player's bottom is at or slightly above the other collider's top,
                // we consider this a floor/platform we're standing on, not a blocker
                // for horizontal movement.
                if (playerBottom >= otherTop - floorEpsilon)
                {
                    // Do NOT set hasCollision – allow horizontal slide / standing.
                }
                else
                {
                    // Real blocking collision (wall, side of an object, etc.)
                    hasCollision = true;
                    return;
                }
            }
            else
            {
                // For vertical moves or general checks, anything overlapping blocks.
                hasCollision = true;
                return;
            }
        }
    }

    // Recurse into children
    const auto& children = obj->GetChildren();
    for (const auto& child : children)
    {
        CheckCollisionRecursive(child.get(), playerCol, hasCollision, ignoreFloorLikeContacts);
        if (hasCollision)
            return;
    }
}

// Downward raycast from the player's feet to detect ground geometry
bool PlayerController::RaycastGround(Vector3& outGroundPoint, float maxDistance)
{
    BoxCollider* playerCol = gameObject->GetComponent<BoxCollider>();
    if (!sceneRoot || !playerCol)
        return false;

    // Compute the feet position from the player's bounding box
    BoundingBox bounds = playerCol->GetBounds();
    Vector3 feetCenter = {
        (bounds.min.x + bounds.max.x) * 0.5f,
        bounds.min.y + 0.01f, // slightly above the bottom to avoid self-hitting
        (bounds.min.z + bounds.max.z) * 0.5f
    };

    Ray ray;
    ray.position  = feetCenter;
    ray.direction = { 0.0f, -1.0f, 0.0f }; // straight down

    float   bestDist  = maxDistance;
    Vector3 bestPoint = { 0.0f, 0.0f, 0.0f };
    bool    foundHit  = false;

    // Traverse the scene graph and look for the closest hit
    RaycastGroundRecursive(sceneRoot, ray, bestDist, bestPoint, foundHit);

    if (foundHit)
    {
        outGroundPoint = bestPoint;
        return true;
    }

    return false;
}

// Internal recursive helper for RaycastGround: traverses all children and
// tracks the closest hit against any BoxCollider in the scene.
void PlayerController::RaycastGroundRecursive(GameObject* obj,
                                              const Ray& ray,
                                              float& bestDist,
                                              Vector3& bestPoint,
                                              bool& foundHit)
{
    if (!obj)
        return;

    // Skip the player's own GameObject to avoid self-intersection
    if (obj != gameObject)
    {
        BoxCollider* otherCol = obj->GetComponent<BoxCollider>();
        if (otherCol)
        {
            // Check this object's bounding box against the ray
            BoundingBox box = otherCol->GetBounds();
            RayCollision hit = GetRayCollisionBox(ray, box);

            // If there's a hit and it's closer than anything we've seen,
            // update the best distance and hit point.
            if (hit.hit && hit.distance < bestDist)
            {
                bestDist  = hit.distance;
                bestPoint = hit.point;
                foundHit  = true;
            }
        }
    }

    // Recurse into children
    const auto& children = obj->GetChildren();
    for (const auto& child : children)
    {
        RaycastGroundRecursive(child.get(), ray, bestDist, bestPoint, foundHit);
    }
}
