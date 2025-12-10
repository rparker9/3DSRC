# 3DSRC – Raylib C++ 3D Sandbox Engine (VS Code, Windows)

3DSRC is a custom C++ 3D rendering and gameplay sandbox built with **raylib** and designed to be used with **Visual Studio Code on Windows**.

This project serves as a lightweight experimental engine and testbed for:
- Custom rendering pipelines
- Shadow mapping
- Component-based entity architecture
- First-person / free-camera gameplay experiments

Tested with **raylib 5.0** on **Windows 10 and Windows 11** using MinGW (w64devkit).

---

# How to build and run

1. Double-click `main.code-workspace` to open the project in Visual Studio Code.
2. Ensure **raylib (w64devkit)** is installed and the path matches the one in:
   - `Makefile`
   - `.vscode/tasks.json`
3. Press **F5** to build and run the project.

The build system will compile all source files and produce a single executable:

3DSRC.exe

---

# Project layout

The project is organized for clarity and scalability:

3DSRC/
├── shaders/ # GLSL shaders (lighting + shadow mapping)
│ ├── lighting.vs
│ ├── lighting.fs
│ ├── shadow.vs
│ └── shadow.fs
│
├── src/
│ ├── app/ # Application entry point
│ │ └── main.cpp
│
│ ├── core/ # Core engine systems
│ │ ├── GameObject.*
│ │ └── Transform3D.*
│
│ ├── rendering/ # Rendering + camera + lighting
│ │ ├── RenderPipeline.*
│ │ ├── CameraComponent.*
│ │ ├── CameraController.*
│ │ ├── MeshFilter.*
│ │ ├── MeshRenderer.*
│ │ ├── ShadowMap.*
│ │ └── LightComponent.*
│
│ ├── physics/ # Collision and physics
│ │ └── BoxCollider.*
│
│ └── game/ # Gameplay logic and demo scenes
│ ├── DemoScene3D.*
│ └── PlayerController.*
│
├── Makefile # Project build configuration (MinGW + raylib)
├── main.code-workspace # VS Code workspace file
└── .vscode/
├── tasks.json
└── launch.json

---

# What's changed from a basic raylib template

- Single executable output (`3DSRC.exe`)
- Makefile-driven build instead of per-file compilation
- Organized source directories by responsibility
- Custom render pipeline with shadow mapping
- External shader folder loaded at runtime
- Debug and release configurations via VS Code tasks

---

# Notes

- Shaders are loaded at runtime from the `shaders/` folder.
- The working directory is expected to be the project root.
- All `.cpp` files are compiled and linked together in one step.
- The project intentionally avoids heavy frameworks to keep iteration fast.

---

# Preview

<p align="center">
  <img src="preview.jpg" alt="" width="800">
</p>

---

<br>
<p align="center">
Built with ⚡ raylib • C++ • Visual Studio Code  
</p>




