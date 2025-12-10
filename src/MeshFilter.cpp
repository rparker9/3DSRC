#include "MeshFilter.h"

MeshFilter::MeshFilter(const char* modelPath)
{
    LoadModelFromFile(modelPath);
}

MeshFilter::~MeshFilter()
{
    if (hasModel && ownsModel)
    {
        UnloadModel(model);
        hasModel  = false;
        ownsModel = false;
    }
}

void MeshFilter::LoadModelFromFile(const char* modelPath)
{
    // Unload previous model if we own it
    if (hasModel && ownsModel)
    {
        UnloadModel(model);
    }

    model = LoadModel(modelPath);
    hasModel  = true;
    ownsModel = true;
}

void MeshFilter::SetModel(Model m, bool takeOwnership)
{
    if (hasModel && ownsModel)
    {
        UnloadModel(model);
    }

    model     = m;
    hasModel  = true;
    ownsModel = takeOwnership;
}

bool MeshFilter::HasModel() const
{
    return hasModel;
}

Model& MeshFilter::GetModel()
{
    return model;
}

const Model& MeshFilter::GetModel() const
{
    return model;
}
