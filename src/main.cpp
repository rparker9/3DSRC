#include <raylib.h>
#include "Transform.h"
#include "InputManager.h"
#include "PlayerController.h"
#include "CameraController.h"

int main()
{
    // Window and context setup.
    const int screenWidth  = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "3DSRC - FPS Controller (multi-file)");
    SetTargetFPS(60);

    // Capture mouse for first-person look.
    DisableCursor(); // Close window or Alt+F4 to quit.

    // Scene setup: player Transform, input, controller, and camera.
    Transform3D playerTransform;
    InputManager inputManager;
    PlayerController playerController(playerTransform, inputManager);
    CameraController cameraController(playerTransform, inputManager);

    // Initial player position and orientation.
    playerTransform.position      = { 0.0f, 1.8f, 5.0f }; // Standing height above ground.
    playerTransform.rotationEuler = { 0.0f, 0.0f, 0.0f }; // Looking straight along -Z.

    // Configure controller parameters.
    playerController.SetMoveSpeed(6.0f);       // Units per second.
    cameraController.InitializeCamera(60.0f);  // Field of view in degrees.
    cameraController.SetMouseSensitivity(0.08f);

    // Main game loop.
    while (!WindowShouldClose())
    {
        const float deltaTime = GetFrameTime();

        // Allow ESC to toggle mouse capture, useful while debugging.
        if (IsKeyPressed(KEY_ESCAPE))
        {
            if (IsCursorHidden())
                EnableCursor();
            else
                DisableCursor();
        }

        // Per-frame update order:
        // 1) Poll input
        inputManager.Update();

        // 2) Move player using input
        playerController.Update(deltaTime);

        // 3) Update camera orientation/position from transform
        cameraController.Update(deltaTime);

        // ---------------------------------------------------------------------
        // Rendering
        // ---------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(DARKGRAY);

        BeginMode3D(cameraController.GetCamera());
        {
            // Ground plane and grid.
            DrawPlane({ 0.0f, 0.0f, 0.0f }, { 50.0f, 50.0f }, DARKGREEN);
            DrawGrid(50, 1.0f);

            // Some test geometry to walk around.
            DrawCube({ 0.0f, 1.0f, 0.0f }, 1.0f, 2.0f, 1.0f, RED);
            DrawCube({ 5.0f, 1.0f, -5.0f }, 2.0f, 2.0f, 2.0f, BLUE);
            DrawCube({ -4.0f, 1.0f, -3.0f }, 1.5f, 3.0f, 1.5f, ORANGE);

            // Origin marker.
            DrawCube({ 0.0f, 0.1f, 0.0f }, 0.2f, 0.2f, 0.2f, YELLOW);
        }
        EndMode3D();

        // 2D overlay HUD.
        DrawFPS(10, 10);
        DrawText("WASD / Arrows: Move",        10, 30, 10, RAYWHITE);
        DrawText("Mouse: Look around",         10, 45, 10, RAYWHITE);
        DrawText("ESC: Toggle mouse capture",  10, 60, 10, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
