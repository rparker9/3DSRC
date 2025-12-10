#include "raylib.h"

#include "RenderPipeline.h"
#include "SceneManager.h"
#include "DemoScene3D.h"

int main()
{
    // Declare screen dimensions
    const int SCREEN_WIDTH  = 1280;
    const int SCREEN_HEIGHT = 720;

    // Initialize raylib window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "3DSRC");
    SetTargetFPS(60);
    DisableCursor();

    // Initialize render pipeline
    RenderPipeline pipeline(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!pipeline.Initialize())
    {
        CloseWindow();
        return -1;
    }

    // Create and start the 3D demo scene, bound to this pipeline
    SceneManager sceneManager;
    sceneManager.MakeScene<DemoScene3D>(pipeline);

    // Main loop
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // GAME / SIMULATION STEP
        sceneManager.Update(dt);
        sceneManager.LateUpdate(dt);

        // RENDER STEP
        sceneManager.Draw(pipeline);
    }

    pipeline.Shutdown();
    CloseWindow();
    return 0;
}
