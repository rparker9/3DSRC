#pragma once

#include "raylib.h"
#include "Component.h"

/// <summary>
/// MeshFilter is responsible for holding a Model (geometry) for a GameObject.
/// MeshRenderer in CUSTOM mode will query MeshFilter on the same GameObject
/// to obtain the Model to draw.
/// </summary>
class MeshFilter : public Component {
private:
    Model model{};
    bool  hasModel = false;
    bool  ownsModel = false;

public:
    /// <summary>
    /// Default constructor: initially has no model.
    /// </summary>
    MeshFilter() = default;

    /// <summary>
    /// Constructs a MeshFilter that immediately loads a Model from file.
    /// </summary>
    explicit MeshFilter(const char* modelPath);

    /// <summary>
    /// Destructor: releases the Model if owned.
    /// </summary>
    ~MeshFilter();

    /// <summary>
    /// Loads a Model from file and takes ownership of it.
    /// If a previous owned model exists, unloads it first.
    /// </summary>
    void LoadModelFromFile(const char* modelPath);

    /// <summary>
    /// Assigns an existing Model to this filter.
    /// If takeOwnership is true, this filter is responsible for UnloadModel.
    /// </summary>
    void SetModel(Model m, bool takeOwnership);

    /// <summary>
    /// Returns true if this MeshFilter currently holds a valid Model.
    /// </summary>
    bool HasModel() const;

    /// <summary>
    /// Returns a reference to the underlying Model. Caller must ensure HasModel() is true.
    /// </summary>
    Model& GetModel();
    const Model& GetModel() const;
};
